# SPDX-License-Identifier: GPL-3.0-only
# KitNote: a recording -> a drum tab. The drums are separated from the other instruments (Demucs, when
# installed), their strokes found and sorted (bass drum, snare, toms, hi-hat, crash, ride), the beat
# tracked, and the strokes written on a grid of sixteenth notes, four beats to a measure - as a chart writes
# them: one groove for a passage, the time kept on one thing, strokes the drum model barely heard off the
# groove left out (to_tab, snap_to_groove). KitNote then writes the tab into a score (src/kitnotemcp/main.cpp,
# AudioToScoreModel).
#
#   python kitnote_audio.py song.mp3 --json out.json [--no-separation] [--tempo 120] [--stems folder]
#
# Progress goes to stdout as "PROGRESS <percent> <what>" lines. Needs numpy, scipy and soundfile;
# separation needs torch and demucs (Preferences > AI installs them). The result is a starting point:
# an automatic transcription of a mixed recording is never exact.

import argparse
import json
import os
import shutil
import subprocess
import sys
import tempfile

import numpy as np
from scipy import signal

# the model of the engine KitNote installed (Preferences > AI): its Python is <root>/venv/Scripts/python.exe
# (Windows) or <root>/venv/bin/python (a Mac) - either way three folders up is the engine's root
_ENGINE_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(sys.executable))))
if os.path.isdir(os.path.join(_ENGINE_ROOT, "torch")):
    os.environ.setdefault("TORCH_HOME", os.path.join(_ENGINE_ROOT, "torch"))

RATE = 44100
HOP = 256                      # 5.8 ms
FFT = 2048
# a frame of the spectrogram (no padding at the start) is heard around its middle: frame f at f * HOP + FFT / 2
FRAME_CENTRE = FFT / 2 / RATE  # 23 ms
# Tuned on real recordings (a pop-rock song against a published chart of it):
HAT_RISE = 3.0                 # dB a hi-hat jumps at least (an open one struck again while it rings: little)
HAT_LEVEL = -20.0              # dB a hi-hat may be under the loudest highs of the song (a snare's rattle is louder)
HAT_WEAK = 4.0                 # dB under the measure's hats on the eighths: a hat between them is the noise of another drum
GHOST_DB = 6.0                 # dB under the usual snare: a ghost note
CYM_RATIO = 2.0                # a cymbal: the upper middle (3-7 kHz) over the highest band (7-16 kHz), at least
CYM_RISE = 5.0                 # dB a cymbal jumps at least
CYM_BRIGHTNESS = 7500.0        # Hz: time kept on a cymbal is darker than on a hi-hat (a hi-hat about 9 kHz)
CLOSED_RING = 0.05             # a hat as loud as this, 70-100 ms on, as a share of the stroke: closed
OPEN_RING = 0.4                # ... and this: open (in between: not clear, it does not vote)
OPEN_ACCENT = 0.8              # an open one of its own in a closed measure
KEPT_OPEN_RING = 0.3           # time kept on metal ringing so (as a hat is measured): an open hi-hat, not a ride
CRASH_ACCENT = 0.5             # a crash ringing so, in a measure kept on the hi-hat: an accent of its own
HAT_RINGS = {}                 # find_strokes: the ring of every hat stroke by its time (None: struck with the snare)
CYMBAL_RINGS = {}              # engine 2: how long every cymbal stroke rings, by its time
CONFIDENCE = {}                # engine 2: how surely the drum model heard each stroke (0 to 1), by (time, tab line)
SNAP_SURE = 0.5                # a drum's stroke off the groove heard less surely than this is left out (snap_to_groove)
METAL_SURE = 0.8               # ... a hi-hat's or a cymbal's (their wash is often heard as a stroke, or as an accent)
SNAP_ALIKE = 0.5               # ... in a measure at least this like one of those around it (not a fill)
OFF_GRID = 0.35                # metal this far between two steps (0.5: halfway), heard weakly: the wash, not a stroke
FILL_HEARD = 0.5               # metal heard this much (share of the model's threshold) where the groove has it: a stroke
MODEL_ACTIVATIONS = []         # engine 2: (the drum model's activations, its thresholds), for snap_to_groove
METAL = {}                     # find_strokes: (brightness in Hz, ring of the upper middle) of a hat or cymbal struck
                               # alone, by its time - a measure kept on a cymbal is told from one on the hi-hat by it
TOM_RING = 0.2                 # a low tom under the snare: its band over the snare's, 0.12-0.22 s on


def progress(percent, what):
    print(f"PROGRESS {int(percent)} {what}", flush=True)


# --- reading ----------------------------------------------------------------------------------------

def find_ffmpeg():
    """ffmpeg on the PATH - or where Homebrew puts it on a Mac: a program started from the Dock or by an AI tool
    may not have Homebrew's folders on its PATH."""
    found = shutil.which("ffmpeg")
    if found:
        return found
    for folder in ("/opt/homebrew/bin", "/usr/local/bin"):
        path = os.path.join(folder, "ffmpeg")
        if os.path.isfile(path) and os.access(path, os.X_OK):
            return path
    return None


def load_audio(path):
    """The recording as float32 [channels, samples] at 44.1 kHz. soundfile reads wav, flac, ogg and mp3;
    anything else goes through ffmpeg when it is there."""
    import soundfile as sf
    try:
        data, rate = sf.read(path, dtype="float32", always_2d=True)
    except Exception:
        ffmpeg = find_ffmpeg()
        if not ffmpeg:
            raise SystemExit("This kind of file needs ffmpeg (or convert it to wav, flac or mp3 first): " + path)
        with tempfile.TemporaryDirectory() as tmp:
            wav = os.path.join(tmp, "audio.wav")
            subprocess.run([ffmpeg, "-y", "-loglevel", "error", "-i", path, "-ac", "2", "-ar", str(RATE), wav], check=True)
            data, rate = sf.read(wav, dtype="float32", always_2d=True)
    data = data.T
    if data.shape[0] == 1:
        data = np.vstack([data, data])
    data = data[:2]
    if rate != RATE:
        g = np.gcd(int(rate), RATE)
        data = signal.resample_poly(data, RATE // g, int(rate) // g, axis=1).astype(np.float32)
    return data


def separate_drums(audio, stems_dir=None, model_name="htdemucs"):
    """The drums of the mix, by Demucs: htdemucs, or htdemucs_ft (four models fine-tuned one for each
    instrument: better, four times slower, and downloaded - about 320 MB - the first time)."""
    import torch
    from demucs.apply import apply_model
    from demucs.pretrained import get_model

    progress(8, "loading the separation model")
    model = get_model(model_name)
    model.eval()
    device = "cuda" if torch.cuda.is_available() else "cpu"
    wav = torch.from_numpy(audio)
    ref = wav.mean(0)
    mean, std = ref.mean(), ref.std() + 1e-8
    wav = (wav - mean) / std

    progress(12, "separating the drums from the other instruments")
    with torch.no_grad():
        sources = apply_model(model, wav[None], device=device, split=True, overlap=0.25, progress=False)[0]
    sources = sources * std + mean
    names = list(model.sources)
    drums = sources[names.index("drums")].cpu().numpy()

    if stems_dir:
        import soundfile as sf
        os.makedirs(stems_dir, exist_ok=True)
        for name, source in zip(names, sources.cpu().numpy()):
            sf.write(os.path.join(stems_dir, f"{name}.wav"), source.T, RATE)
    return drums


def remove_hum(audio):
    """The recording without the hum of the mains (50 or 60 Hz and its harmonics: an ungrounded cable, a guitar
    amplifier), and the frequencies taken out. A hum sounds all through the recording at exactly those
    frequencies - a needle in the spectrum of even its quietest seconds, which no drum leaves (a drum rings
    for a moment, and at its own pitch) - and the drum model would hear it as a floor tom now and then. Each
    is notched out, half a hertz wide, where it is measured to be (a floor tom tuned near the hum keeps its
    pitch); without a hum the recording stays as it is."""
    mono = audio.mean(axis=0)
    window = RATE                                   # one second: a hertz apart
    if len(mono) < 4 * window:
        return audio, []
    # (only the lowest thousand hertz are looked at: the transform is kept for those alone)
    freqs = np.fft.rfftfreq(window, 1.0 / RATE)
    low = freqs < 1000
    freqs = freqs[low]
    z = stft_frames(mono, signal.get_window("hann", window), window // 2, bins=int(low.sum()))
    low = np.ones(len(freqs), bool)
    level = 20.0 * np.log10(np.percentile(np.abs(z[low]), 10, axis=1) + 1e-12)

    def needle(f):
        """(how many dB the quietest seconds have near f over the spectrum a few hertz around, the frequency of
        that peak, finer than the bins)"""
        i = int(np.argmin(np.abs(freqs - f)))
        peak = i - 1 + int(np.argmax(level[i - 1:i + 2]))
        sides = np.concatenate([level[max(0, peak - 8):peak - 3], level[peak + 4:peak + 9]])
        left, middle, right = level[peak - 1:peak + 2]
        curve = left - 2.0 * middle + right
        offset = 0.5 * (left - right) / curve if curve < 0 else 0.0
        return float(middle - np.median(sides)), float(freqs[peak] + np.clip(offset, -0.5, 0.5) * (freqs[1] - freqs[0]))

    def precisely(f):
        """(the frequency of the steady tone near f, to a hundredth of a hertz - how far its phase turns from one
        second to the next in the quietest seconds, where the hum is most of what sounds there - and in how
        many pairs of seconds it was measured)"""
        i = int(np.argmin(np.abs(freqs - f)))
        row = z[low][i]
        quiet = np.abs(row) <= 1.5 * np.percentile(np.abs(row), 10)
        hop = (window // 2) / RATE
        turns = row[1:] * np.conj(row[:-1]) * np.exp(-2j * np.pi * freqs[i] * hop)
        turns = turns[quiet[1:] & quiet[:-1]]
        if not len(turns):
            return f, 0
        return float(freqs[i] + np.angle(turns.sum()) / (2.0 * np.pi * hop)), len(turns)

    best, best_height = [], 0.0
    for mains in (50.0, 60.0):
        needles = {k: needle(k * mains) for k in range(1, 9)}
        # the hum is there when its first or second harmonic stands out clearly (one high harmonic alone may be
        # a drum's ring); then the harmonics that stand out a little are taken out with it
        if max(needles[1][0], needles[2][0]) > 18.0:
            found = [k for k in needles if needles[k][0] > 12.0]
            height = sum(needles[k][0] for k in found)
            if height > best_height:
                # (the harmonics are whole multiples of the mains' frequency; the higher a harmonic, the finer it tells the mains' frequency: a phase measured as well is
                # k times less of a hertz there)
                measured = [(k, *precisely(needles[k][1])) for k in found]
                # (a harmonic measured off the others - where next to nothing sounds, its phase turns at random -
                # does not count)
                usual = float(np.median([f / k for k, f, _ in measured]))
                measured = [(k, f, count) for k, f, count in measured if abs(f / k - usual) < 0.1] or measured
                weights = [k * k * count for k, _, count in measured]
                f0 = sum(w * f / k for w, (k, f, _) in zip(weights, measured)) / sum(weights) if sum(weights) else mains
                best, best_height = [k * f0 for k in found], height
    if not best:
        return audio, []
    # (a channel at a time: the filtering works in double precision, and two of an hour's channels at once
    # would take more memory than needed)
    out = np.empty(audio.shape, np.float32)
    for c in range(audio.shape[0]):
        channel = audio[c].astype(np.float64)
        for f in best:
            b, a = signal.iirnotch(f, f / 0.5, fs=RATE)
            channel = signal.filtfilt(b, a, channel)
        out[c] = channel
    return out, [round(f, 2) for f in best]


# --- strokes ----------------------------------------------------------------------------------------

def stft_frames(mono, window, hop, bins=None, magnitude=False):
    """What signal.stft(mono, nperseg=len(window), noverlap=len(window) - hop, boundary=None, padded=False)
    gives - [bins, frames], scaled by the window's sum - computed a few thousand frames at a time and in single
    precision, with only the first "bins" rows (all when None) and only their magnitude when asked: the whole
    transform of an hour's recording at once would take gigabytes."""
    mono = np.ascontiguousarray(mono, dtype=np.float32)
    size = len(window)
    count = 0 if len(mono) < size else 1 + (len(mono) - size) // hop
    rows = size // 2 + 1 if bins is None else bins
    out = np.empty((rows, count), np.float32 if magnitude else np.complex64)
    window = (window / window.sum()).astype(np.float32)
    frames = np.lib.stride_tricks.sliding_window_view(mono, size)[::hop]
    block = max(1, (1 << 25) // size)            # about 128 MB of samples at a time
    for start in range(0, count, block):
        z = np.fft.rfft(frames[start:start + block] * window, axis=1)[:, :rows]
        out[:, start:start + len(z)] = (np.abs(z) if magnitude else z).T
    return out


def spectrogram(mono):
    return stft_frames(mono, signal.get_window("hann", FFT), HOP, magnitude=True)


def band(spec, low, high):
    freqs = np.linspace(0, RATE / 2, spec.shape[0])
    return spec[(freqs >= low) & (freqs < high)]


def level_db(spec_band):
    """The loudness of a band over time, in dB, a little smoothed."""
    power = (spec_band.astype(np.float64) ** 2).sum(axis=0)
    power = np.convolve(power, np.ones(2) / 2, mode="same")
    return 10.0 * np.log10(power + 1e-10)


def above_floor(power):
    """A band's power over its noise floor - the hiss of a room or a phone, which is there when nothing is
    struck (the quietest hundredth of the recording: the moment before the first stroke, the pauses) - so
    that how long a hi-hat or a cymbal rings is measured on the metal alone: a closed hi-hat in a noisy
    recording does not ring on in the noise. (Of a separated drum stem the floor is all but nothing.)"""
    return np.maximum(power - np.percentile(power, 1), 0.0)


def rise(db, back=6, ahead=3):
    """How many dB a band jumps at every frame: the loudest of the next frames over the quietest of the
    frames before. A drum that is struck jumps; one that rings on does not, whatever its ripples."""
    n = len(db)
    # silence before the start: a stroke on the very first frame is a jump too
    padded = np.concatenate([np.full(back, -100.0), db, np.full(ahead, db[-1])])
    before = np.lib.stride_tricks.sliding_window_view(padded[:n + back - 1], back).min(axis=1)[:n]
    after = np.lib.stride_tricks.sliding_window_view(padded[back:], ahead + 1).max(axis=1)[:n]
    return after - before


def onsets(r, gap, min_rise):
    # (a stroke on the very first frame is a peak too: find_peaks passes over the edges)
    found, _ = signal.find_peaks(np.concatenate([[0.0], r]), height=min_rise, distance=max(1, gap))
    return found - 1


def steady_tones(drum_band):
    """What sounds all through the recording in a band of the spectrum, frequency by frequency: the hum of the
    mains (50 or 60 Hz and its harmonics), an amplifier's buzz - what is still there in the quietest moments
    (a drum rings for a moment, so even one played all the time leaves gaps). Next to nothing in a clean
    recording or a separated drum stem."""
    return np.percentile(drum_band, 5, axis=1)


def drum_body(drum_band, f, hum):
    """The spectrum of a drum's body at a stroke at frame f (60 to 400 Hz), without the steady tones of the
    recording, and whether nothing new sounds there: a stroke rises over what sounded just before it, and a
    moment where the band rises by less than the hum's own power is no tom (else the hum - the lowest tone
    of the recording, so a floor tom by its pitch - would be one, all through the recording). The hum's
    power is taken away, not its level: a floor tom tuned to the hum's own pitch keeps its pitch."""
    at = drum_band[:, f:f + 6].astype(np.float64).mean(axis=1)
    before = drum_band[:, max(0, f - 10):max(1, f - 2)].astype(np.float64).min(axis=1)
    body = np.sqrt(np.maximum(at ** 2 - 1.2 * hum ** 2, 0.0))
    rise = np.maximum(at - before, 0.0)
    return body, bool(np.sum(rise ** 2) < 0.5 * np.sum(hum.astype(np.float64) ** 2))


def find_strokes(drums):
    """[(time in seconds, drum, strength)] for the drum stem. Drums: BD SD T1 T2 FT HH HO CC RD."""
    mono = drums.mean(axis=0)
    spec = spectrogram(mono)
    frame_time = HOP / RATE
    gap = int(0.06 / frame_time)

    bands = {
        "low": band(spec, 30, 110),
        "body": band(spec, 140, 350),
        "noise": band(spec, 1500, 5000),
        "high": band(spec, 7000, 16000),
        "upper": band(spec, 3000, 7000),
    }
    db = {name: level_db(b) for name, b in bands.items()}
    rises = {name: rise(d) for name, d in db.items()}
    loud = {name: np.percentile(d, 98) for name, d in db.items()}

    # the moments something is struck: a jump in any band
    # (a cymbal struck again while it rings rises little: 5 dB in the upper middle)
    candidates = sorted(set(np.concatenate([onsets(rises[name], gap, 6.0) for name in ("low", "noise", "high", "body")]
                                           + [onsets(rises["upper"], gap, 5.0), onsets(rises["high"], gap, HAT_RISE)]).tolist()))
    merged = []
    for f in candidates:
        if merged and f - merged[-1] < int(0.03 / frame_time):
            continue
        merged.append(f)

    after_short = int(0.1 / frame_time)
    high_power = above_floor(10 ** (db["high"] / 10))
    # the upper middle (3 to 7 kHz): strong in a crash or a ride, weak in a hi-hat
    upper_power = above_floor(10 ** (db["upper"] / 10))
    after_long = int(0.45 / frame_time)

    drum_band = band(spec, 60, 400)
    drum_freqs = np.linspace(60, 400, drum_band.shape[0])
    hum = steady_tones(drum_band)
    # a low tom rings on at its pitch after the snare's body has gone: the power of the two later on (over what
    # sounds all the time there: the hum of the mains is at 120 Hz too)
    tom_power = above_floor((band(spec, 110, 150).astype(np.float64) ** 2).sum(axis=0))
    snare_power = above_floor((band(spec, 170, 260).astype(np.float64) ** 2).sum(axis=0))

    strokes = []
    HAT_RINGS.clear()
    METAL.clear()
    # the brightness of a metal stroke: the centre of its spectrum above 1 kHz (a hi-hat about 9 kHz, the
    # crash or ride a drummer keeps time on about 6)
    bright_rows = np.linspace(0, RATE / 2, spec.shape[0]) > 1000
    bright_freqs = np.linspace(0, RATE / 2, spec.shape[0])[bright_rows]
    toms = []           # (time, pitch, strength, with the snare): sorted into high, mid and floor tom at the end
    for index, f in enumerate(merged):
        next_onset = merged[index + 1] if index + 1 < len(merged) else len(tom_power)
        # a little after the moment too: the bands of one stroke do not all jump in the same frame
        window = slice(max(0, f - 2), f + 6)
        r = {name: float(rises[name][window].max()) for name in rises}
        lvl = {name: float(db[name][f:f + 7].max() - loud[name]) for name in db}
        t = f * frame_time + FRAME_CENTRE
        kick = r["low"] > 6 and lvl["low"] > -5
        # the pitch of a drum's body; above the bass drum when that sounds too (and above a steady hum)
        spectrum, humming = drum_body(drum_band, f, hum)
        if kick:
            spectrum = np.where(drum_freqs >= 130, spectrum, 0)
        pitch = float(drum_freqs[int(np.argmax(spectrum))])
        if kick:
            strokes.append((t, "BD", lvl["low"]))

        # a drum with a body: the snare (high and with the rattle of its wires) or a tom
        # (a drum struck with the bass drum - a tom, say - is no cymbal either)
        drum_struck = kick and r["body"] > 8
        with_snare = False
        if r["body"] > 6 or r["noise"] > 8:
            if pitch >= 160 and lvl["body"] > -16 and lvl["noise"] > -14:
                strokes.append((t, "SD", lvl["body"]))
                drum_struck = True
                with_snare = True
                # a tom struck with the snare: a second, lower peak of the body, nearly as strong - or a
                # low tom that rings on after the snare (up to the next stroke, which would ring too)
                low_part = np.where((drum_freqs >= 70) & (drum_freqs < 150), spectrum, 0)
                later = slice(f + int(0.12 / frame_time), min(f + int(0.22 / frame_time), next_onset))
                rings = later.stop - later.start >= 3 and \
                    tom_power[later].mean() > TOM_RING * snare_power[later].mean()
                if not kick and (low_part.max() > 0.6 * spectrum.max() or rings):
                    toms.append((t, float(drum_freqs[int(np.argmax(low_part))]), lvl["body"], True))
            elif r["body"] > 8 and lvl["body"] > -12 and not kick and not humming:
                toms.append((t, pitch, lvl["body"], False))
                drum_struck = True

        # Cymbals. A crash or a ride sounds much in the upper middle (several times the highest band), a
        # hi-hat little. A crash rings on (half a second later still a third as loud); with a snare stroke,
        # only a ringing one is a cymbal (the snare's own rattle reaches the upper middle too). A closed
        # hi-hat is gone in a tenth of a second, an open one rings until it is closed.
        upper = upper_power[f:f + 6].max() / (high_power[f:f + 6].max() + 1e-12)
        upper_at = upper_power[max(0, f - 3):f + 6].max() + 1e-12
        # (the quietest moment of a window, not one frame: the next stroke may fall on it)
        ring_window = upper_power[min(len(upper_power) - 1, f + int(0.3 / frame_time)):f + after_long + 1]
        upper_ring = (ring_window.min() if len(ring_window) else 0.0) / upper_at
        metal = None
        if not (kick or drum_struck):
            power = spec[bright_rows, f:f + 6].astype(np.float64).mean(axis=1) ** 2
            metal = (float((power * bright_freqs).sum() / (power.sum() + 1e-12)), float(upper_ring))
        if upper > CYM_RATIO and r["upper"] > CYM_RISE and lvl["upper"] > -12 and (not drum_struck or upper_ring > 0.35):
            strokes.append((t, "CC" if upper_ring > 0.3 else "RD", lvl["upper"]))
            METAL[t] = metal
        elif r["high"] > HAT_RISE and lvl["high"] > HAT_LEVEL:
            METAL[t] = metal
            at = high_power[max(0, f - 3):f + 4].max() + 1e-12
            hat_window = high_power[min(len(high_power) - 1, f + int(0.07 / frame_time)):f + after_short + 1]
            ring = (hat_window.min() if len(hat_window) else 0.0) / at
            # (with the snare, its wires ring in the same band: how long the hat rings is not heard then)
            HAT_RINGS[t] = None if with_snare else ring
            strokes.append((t, "HO" if ring > OPEN_RING else "HH", lvl["high"]))

    # (the pitch of each tom stroke measured finer, heard up to the next drum stroke: which tom, from them all)
    drum_times = [t for t, d, _ in strokes if d in ("BD", "SD")] + [tom[0] for tom in toms]
    kick_times = np.array([t for t, d, _ in strokes if d == "BD"])
    with_kick = [bool(len(kick_times)) and bool(np.min(np.abs(kick_times - tom[0])) < 0.03) for tom in toms]
    measured = measure_toms(mono, [tom[0] for tom in toms], drum_times, with_kick, [tom[3] for tom in toms])
    strokes += name_toms([(t, pitch if new is None else new, strength, clarity, power, other)
                          for (t, pitch, strength, _), (new, clarity, power, other) in zip(toms, measured)])

    onset_curve = sum(np.maximum(rises[name], 0) for name in ("low", "noise", "high"))
    return sorted(strokes), onset_curve, frame_time


# Which tom: a tom has a clear pitch (its fundamental, gliding down a little as it rings), and every stroke on
# one drum has about the same one. The pitch of every tom stroke is measured, the pitches of the whole song
# gathered into as many drums as they clearly make (2 to 4, a semitone or two wide each, at least two
# semitones apart), and the drums named from the highest down - as a chart names them. Measured on fills
# played on 21 sampled kits: their toms lie 3 to 11 semitones apart (rack toms 100-310 Hz, floor toms
# 57-117 Hz), and one stroke's pitch is within a semitone of its drum's three times out of four - the others
# have an overtone, the bass drum or the drum struck before louder.
TOM_BAND = (50.0, 450.0)       # Hz: where the fundamental of a tom is
TOM_FFT = 1 << 15              # samples of the transform (a resolution of 1.3 Hz)
TOM_WINDOW = 0.15              # s of a stroke heard, unless the next stroke comes sooner
TOM_APART = 2.0                # semitones two drums are apart at least
TOM_NAMES = {2: ("T1", "FT"), 3: ("T1", "T2", "FT"), 4: ("T1", "T2", "FT", "FT")}
_TOM_FREQS = np.fft.rfftfreq(TOM_FFT, 1.0 / RATE)
_TOM_ROWS = (_TOM_FREQS >= TOM_BAND[0]) & (_TOM_FREQS <= TOM_BAND[1])
TOM_FREQS = _TOM_FREQS[_TOM_ROWS]


def tom_power(mono, start, end):
    """The power spectrum (TOM_BAND) of the sound between two times (seconds)."""
    a, b = max(0, int(start * RATE)), min(len(mono), int(end * RATE))
    if b - a < 256:
        return np.zeros(len(TOM_FREQS))
    piece = mono[a:b].astype(np.float64) * np.hanning(b - a)
    return np.abs(np.fft.rfft(piece, TOM_FFT))[_TOM_ROWS] ** 2 / (b - a)


def tom_pitch(power):
    """(the fundamental in Hz, how clearly - the share of the power within a semitone of it) of a stroke's
    spectrum, or (None, 0). Its strongest peak, finer than the bins - unless a peak at half its frequency is
    nearly as strong: that is the fundamental, the strongest one its overtone."""
    if power.max() <= 0:
        return None, 0.0
    k = int(np.argmax(power))
    offset = 0.0
    if 0 < k < len(power) - 1:
        left, middle, right = np.log(power[k - 1:k + 2] + 1e-30)
        curve = left - 2.0 * middle + right
        offset = float(np.clip(0.5 * (left - right) / curve, -0.5, 0.5)) if curve < 0 else 0.0
    f = float(TOM_FREQS[k] + offset * (TOM_FREQS[1] - TOM_FREQS[0]))
    half = (TOM_FREQS > 0.475 * f) & (TOM_FREQS < 0.525 * f)
    if f / 2 >= 55 and half.any() and power[half].max() > 0.3 * power[k]:
        f = float(TOM_FREQS[half][np.argmax(power[half])])
    near = (TOM_FREQS > f * 2 ** (-1 / 12)) & (TOM_FREQS < f * 2 ** (1 / 12))
    return f, float(power[near].sum() / (power.sum() + 1e-30))


def measure_toms(mono, times, onsets, kicks, snares=None):
    """[(pitch, clarity, power spectrum, struck with the bass drum or the snare)] of the tom strokes at these
    times (seconds). A stroke is heard from just after it is struck to the next drum stroke (onsets: the times
    of the bass drum, snare and tom strokes) or 0.15 s. (What still rings of the stroke before is not taken
    away: in a fill of strokes on one drum that would take its own pitch away.) With the bass drum (kicks:
    whether it was struck with it), the pitch is looked for above 90 Hz, and the stroke counts for little in
    finding the drums; with the snare (snares), whose own pitch may be the strongest, it is named by what
    sounds at the drums' pitches (name_toms)."""
    onsets = np.sort(np.asarray(onsets, dtype=float))
    measured = []
    for t, kick, snare in zip(times, kicks, snares if snares is not None else [False] * len(times)):
        later = onsets[onsets > t + 0.02]
        end = max(t + 0.04, min(t + TOM_WINDOW, (later[0] if len(later) else t + 1.0) - 0.005))
        power = tom_power(mono, t + 0.008, end)
        pitch, clarity = tom_pitch(np.where(TOM_FREQS >= 90, power, 0.0) if kick else power)
        measured.append((pitch, clarity * (0.3 if kick else 1.0), power, bool(kick or snare)))
    return measured


def tom_drums(pitches, weights):
    """The pitches (Hz, high to low) of the toms played in a song: the places where the strokes' pitches
    gather - with weight enough (two strokes, or a fourteenth of them all), at least TOM_APART semitones from a
    weightier one and with a clear dip between them; four at most. weights: how much each stroke counts."""
    x = np.log2(np.asarray(pitches, dtype=float))
    w = np.asarray(weights, dtype=float)
    grid = np.arange(np.log2(TOM_BAND[0]), np.log2(TOM_BAND[1] + 10.0), 1.0 / 96.0)
    density = (w[None, :] * np.exp(-0.5 * ((grid[:, None] - x[None, :]) / (0.6 / 12.0)) ** 2)).sum(axis=1)
    peaks = signal.find_peaks(np.concatenate([[0.0], density, [0.0]]))[0] - 1
    total = float(w.sum())
    least = max(min(1.2, 0.3 * total), 0.07 * total)
    found = []
    for i in peaks:
        weight = float(w[np.abs(x - grid[i]) < 1.5 / 12.0].sum())
        if weight >= least:
            found.append((weight, grid[i], density[i]))
    kept = []
    for weight, place, height in sorted(found, reverse=True):
        clear = True
        for _, other, other_height in kept:
            low, high = sorted((place, other))
            between = density[(grid > low) & (grid < high)]
            if high - low < TOM_APART / 12.0 or \
                    (len(between) and between.min() > 0.6 * min(height, other_height)):
                clear = False
                break
        if clear:
            kept.append((weight, place, height))
        if len(kept) == 4:
            break
    return sorted((2.0 ** place for _, place, _ in kept), reverse=True)


def overtone_of(drum, drums, toms):
    """Whether the pitch of a drum found (Hz) is an overtone of a lower one - the strokes of a tom whose
    overtone (about one and a half or two times its pitch) rang loudest gather there too: those strokes (toms:
    as name_toms has them) sound loudly at the lower drum's pitch as well."""
    members = [tom[4] for tom in toms if tom[4] is not None and abs(np.log2(tom[1] / drum)) < 1.0 / 12.0]
    for lower in drums:
        ratio = drum / lower
        if not members or not (1.45 <= ratio <= 1.7 or 1.9 <= ratio <= 2.1):
            continue
        shares = [power[np.abs(np.log2(TOM_FREQS / lower)) < 0.7 / 12].max() / (power.max() + 1e-30) for power in members]
        if np.median(shares) >= 0.3:
            return True
    return False


def name_toms(toms):
    """[(time, tom line, strength)] of the tom strokes: [(time, pitch, strength)], or [(time, pitch, strength,
    clarity, power spectrum, struck with another drum)] as measure_toms measures them. The drums of the song
    (tom_drums) are named from the highest: two are the rack tom and the floor tom (T1, FT - a four-piece kit),
    three T1 T2 FT, four T1 T2 FT FT; a single one by its pitch. A stroke goes to the drum nearest its pitch, so
    a fill that goes down the toms goes down the lines. One struck with the bass drum or the snare (their own
    pitch may be the strongest), or whose strongest peak is near none of the drums (an overtone), goes to the
    drum that sounds most in it."""
    toms = [tuple(tom) + (1.0, None, False)[len(tom) - 3:] if len(tom) < 6 else tuple(tom) for tom in toms]
    heard = [tom for tom in toms if tom[1]]
    if not heard:
        return [(t, "T2", strength) for t, _, strength, *_ in toms]
    drums = tom_drums([tom[1] for tom in heard], [max(0.2 if tom[5] else 0.6, tom[3]) for tom in heard])
    if not drums:
        drums = [float(np.median([tom[1] for tom in heard]))]
    drums = [d for d in drums if not overtone_of(d, drums, heard)]
    if len(drums) == 1:
        names = ("T1",) if drums[0] >= 135 else ("T2",) if drums[0] >= 100 else ("FT",)
    else:
        names = TOM_NAMES[len(drums)]
    places = np.log2(drums)
    named = []
    for t, pitch, strength, _, power, kick in toms:
        nearest = int(np.argmin(np.abs(places - np.log2(pitch)))) if pitch else None
        if power is not None and len(drums) > 1 and \
                (kick or nearest is None or abs(places[nearest] - np.log2(pitch)) > 1.0 / 12.0):
            sounding = [power[np.abs(np.log2(TOM_FREQS / d)) < 0.7 / 12].max() for d in drums]
            k = int(np.argmax(sounding))
        elif nearest is not None:
            k = nearest
        else:
            k = len(drums) // 2
        named.append((t, names[k], strength))
    return named


# --- engine 2: models ----------------------------------------------------------------------------------
# Beat This! (CPJKU, MIT licence) finds the beats and the downbeats of the mix. The strokes come from KitNote's
# own drum model (kitnote_drums.pt, trained by KitNote on openly licensed data) when it is installed, else from
# the bands above; what a model does not tell - open or closed hi-hat, crash or ride, which tom, a ghost note -
# is measured on the drums the separation gives. Without Beat This! (an engine installed before), engine 1
# does it all.

SD_MAIN = 0.0                  # a drum model's snare activation under this: a ghost note (or another drum's noise)
MODEL_FPS = 100                # frames a second of the drum model
DRUM_CLASSES = ("BD", "SD", "TT", "HH", "CY")      # the drum model's outputs, in order
DRUM_THRESHOLDS = (0.3, 0.3, 0.35, 0.3, 0.3)
MODEL_CLASS = {"BD": "BD", "SD": "SD", "SG": "SD", "T1": "TT", "T2": "TT", "FT": "TT", "HH": "HH", "HO": "HH",
               "CC": "CY", "RD": "CY"}


def beat_model():
    """beat_this.inference when installed, else None."""
    try:
        import beat_this.inference
        return beat_this.inference
    except Exception:
        return None


def drum_model_path():
    """KitNote's drum model, when installed: next to this script, or in the engine's folder (KITNOTE_DRUM_MODEL:
    another weights file, to try one)."""
    if os.environ.get("KITNOTE_DRUM_MODEL"):
        return os.environ["KITNOTE_DRUM_MODEL"] if os.path.exists(os.environ["KITNOTE_DRUM_MODEL"]) else None
    for folder in (os.path.dirname(os.path.abspath(__file__)), _ENGINE_ROOT):
        path = os.path.join(folder, "kitnote_drums.pt")
        if os.path.exists(path):
            return path
    return None


def model_beats(beat_this_inference, audio):
    """Beat and downbeat times (seconds) of the mix."""
    a2b = beat_this_inference.Audio2Beats(checkpoint_path="final0", device="cpu", dbn=False)
    beats, downbeats = a2b(audio.mean(axis=0).astype(np.float32), RATE)
    return np.asarray(beats, dtype=float), np.asarray(downbeats, dtype=float)


def model_activations(path, drums):
    """The drum model's activations of the drums: [frames at MODEL_FPS a second, len(DRUM_CLASSES)], and the
    thresholds its training found best for each class."""
    import torch
    import kitnote_drum_model
    model, features = kitnote_drum_model.load(path)
    thresholds = torch.load(path, map_location="cpu", weights_only=True).get("thresholds") or DRUM_THRESHOLDS
    return kitnote_drum_model.activations(model, features(drums)), thresholds


def pick_peaks(curve, threshold, fps=MODEL_FPS):
    """Times of the peaks of an activation curve: over the threshold, the highest within 30 ms around, and
    at least 30 ms apart."""
    found, _ = signal.find_peaks(np.concatenate([[0.0], curve, [0.0]]), height=threshold, distance=max(1, int(0.03 * fps)))
    return [(i - 1) / fps for i in found]


def model_strokes(activations, drums, thresholds=DRUM_THRESHOLDS):
    """The strokes a drum model found, as engine 1 gives them: [(time, drum, strength)]; the kind of
    hi-hat, cymbal and tom, and the ghost notes, measured on the drums."""
    times = {name: pick_peaks(activations[:, i], thresholds[i]) for i, name in enumerate(DRUM_CLASSES)}
    column = {name: i for i, name in enumerate(DRUM_CLASSES)}

    mono = drums.mean(axis=0)
    spec = spectrogram(mono)
    frame_time = HOP / RATE
    high_db = level_db(band(spec, 7000, 16000))
    loud_high = np.percentile(high_db, 98)
    high_power = above_floor(10 ** (high_db / 10))
    upper_power = above_floor(10 ** (level_db(band(spec, 3000, 7000)) / 10))
    body_db = level_db(band(spec, 140, 350))
    last = len(body_db) - 1         # (the model may hear a stroke in the very last frames, after the spectrogram's)
    loud_body = np.percentile(body_db, 98)
    drum_band = band(spec, 60, 400)
    bright_freqs = np.linspace(0, RATE / 2, spec.shape[0])
    bright_rows = bright_freqs > 1000
    bright_freqs = bright_freqs[bright_rows]

    def brightness(f):
        power = spec[bright_rows, f:f + 6].astype(np.float64).mean(axis=1) ** 2
        return float((power * bright_freqs).sum() / (power.sum() + 1e-12))

    def hat_ring(f):
        at = high_power[max(0, f - 3):f + 4].max() + 1e-12
        hat_window = high_power[min(len(high_power) - 1, f + int(0.07 / frame_time)):f + int(0.1 / frame_time) + 1]
        return (hat_window.min() if len(hat_window) else 0.0) / at

    def near(name, t, within=0.03):
        return any(abs(u - t) < within for u in times[name])

    def activation(name, t):
        f = int(round(t * MODEL_FPS))
        return float(activations[max(0, f - 2):f + 3, column[name]].max())

    strokes, toms = [], []
    HAT_RINGS.clear()
    METAL.clear()
    CYMBAL_RINGS.clear()
    CONFIDENCE.clear()
    MODEL_ACTIVATIONS[:] = [(activations, thresholds)]
    for t in times["BD"]:
        strokes.append((t, "BD", 0.0))
    for t in times["SD"]:
        # (how loud, as engine 1 measures it: to_tab takes one far under the usual snare for a ghost note too)
        f = min(int(t / frame_time), last)
        strokes.append((t, "SD" if activation("SD", t) >= SD_MAIN else "SG", float(body_db[f:f + 7].max() - loud_body)))
    hum = steady_tones(drum_band)
    for t in times["TT"]:
        f = min(int(t / frame_time), last)
        _, humming = drum_body(drum_band, f, hum)
        if humming:
            # (the model heard a tom where the hum is most of what sounds low: another stroke over the hum - a
            # hi-hat in a recording with mains hum - not a tom)
            continue
        toms.append(t)
    # (each tom heard up to the next drum stroke; which tom, from the pitches of them all)
    measured = measure_toms(mono, toms, times["BD"] + times["SD"] + times["TT"], [near("BD", t) for t in toms],
                            [near("SD", t) for t in toms])
    strokes += name_toms([(t, pitch, 0.0, clarity, power, other)
                          for t, (pitch, clarity, power, other) in zip(toms, measured)])
    for t in times["HH"]:
        f = min(int(t / frame_time), last)
        ring = hat_ring(f)
        HAT_RINGS[t] = None if near("SD", t) else ring
        if not (near("SD", t) or near("BD", t)):
            METAL[t] = (brightness(f), 0.0)
        strokes.append((t, "HO" if ring > OPEN_RING else "HH", float(high_db[f:f + 7].max() - loud_high)))
    for t in times["CY"]:
        f = min(int(t / frame_time), last)
        at = upper_power[max(0, f - 3):f + 6].max() + 1e-12
        ring_window = upper_power[min(len(upper_power) - 1, f + int(0.3 / frame_time)):f + int(0.45 / frame_time) + 1]
        ring = (ring_window.min() if len(ring_window) else 0.0) / at
        CYMBAL_RINGS[t] = ring
        HAT_RINGS[t] = None if near("SD", t) else hat_ring(f)      # (should it be a hi-hat after all)
        if not (near("SD", t) or near("BD", t)):
            METAL[t] = (brightness(f), float(ring))
        strokes.append((t, "CC" if ring > 0.3 else "RD", 0.0))
    # how surely the model heard each stroke: 0 at its threshold, 1 at full activation
    for t, drum, _ in strokes:
        threshold = thresholds[column[MODEL_CLASS[drum]]]
        CONFIDENCE[(t, SIGNS[drum][0])] = (activation(MODEL_CLASS[drum], t) - threshold) / (1.0 - threshold + 1e-9)
    return sorted(strokes)


def clean_beats(beats):
    """The beats of the beat model, a beat it heard twice taken out (two short ones that make one usual one)
    and a beat it missed put back (one twice or three times as long as those around). A beat too many or too
    few moves every measure after it by a beat - in 6/8, by half a measure."""
    beats = [float(b) for b in beats]
    for _ in range(len(beats)):
        if len(beats) < 8:
            break
        intervals = np.diff(beats)
        changed = False
        for k, interval in enumerate(intervals):
            usual = float(np.median(intervals[max(0, k - 4):k + 5]))
            if interval < 0.6 * usual:
                # with the neighbour that makes the usual beat with it: the beat between them goes
                options = [(abs(intervals[k - 1] + interval - usual), k)] if k > 0 else []
                if k + 1 < len(intervals):
                    options.append((abs(interval + intervals[k + 1] - usual), k + 1))
                if options and min(options)[0] < 0.2 * usual:
                    del beats[min(options)[1]]
                    changed = True
                    break
            missed = int(round(interval / usual))
            if missed in (2, 3) and abs(interval / usual - missed) < 0.12 * missed:
                beats[k + 1:k + 1] = list(np.linspace(beats[k], beats[k + 1], missed + 1)[1:-1])
                changed = True
                break
        if not changed:
            break
    return np.array(beats)


def ternary_evidence(strokes, beats):
    """How the strokes divide the beats: the share of those between the beats that fall on thirds of the beat
    rather than on its halves and quarters, how many fall between the beats, and how often the middle third
    is struck where the last one is - by the hi-hat or cymbal keeping the time when it plays between the
    beats, else by anything. A 6/8 or 12/8 plays the middle eighth of a beat (a hi-hat or ride on every
    eighth); a shuffle or a swing in 4/4 leaves it out (long-short: the first and the last third)."""
    _, thirds_share, between = beat_contents(strokes, beats) if len(beats) > 2 else ([], 0.0, 0)
    if len(beats) < 3:
        return thirds_share, between, 0.0
    beat_len = float(np.median(np.diff(beats)))
    places = {"metal": [0, 0], "all": [0, 0]}
    for time, drum, _ in strokes:
        i = int(np.searchsorted(beats, time + beat_len / 8.0) - 1)
        if not 0 <= i < len(beats) - 1:
            continue
        frac = (time - beats[i]) / (beats[i + 1] - beats[i])
        for third, place in ((1, 1.0 / 3.0), (2, 2.0 / 3.0)):
            if abs(frac - place) < 0.07:
                places["all"][third - 1] += 1
                if SIGNS[drum][0] in ("HH", "RD"):
                    places["metal"][third - 1] += 1
    middle, last = places["metal"] if sum(places["metal"]) >= 8 else places["all"]
    return thirds_share, between, middle / max(1, last)


def grouped_in_threes(beats, strokes):
    """Whether the beats are the eighths of a 6/8 or 12/8 (a beat model may count them): the bass drum and the
    snare fall on every third of them - the first eighth of each dotted quarter, the bass drum on one and
    the snare on the other - and the strokes are on the beats, not between them. [the index of each beat
    that begins a dotted quarter]; False when the strokes say otherwise; None when there are too few to
    tell. The groups are found in passages of 24 beats, so that a beat the model missed or added further on
    does not shift them all."""
    if len(beats) < 12:
        return None
    beat_len = float(np.median(np.diff(beats)))
    index_of = {"BD": [], "SD": []}
    on_beats = between = 0
    for time, drum, _ in strokes:
        k = int(np.argmin(np.abs(beats - time)))
        if abs(beats[k] - time) < 0.2 * beat_len:
            on_beats += 1
            if SIGNS[drum][0] in index_of and drum != "SG":
                index_of[SIGNS[drum][0]].append(k)
        else:
            between += 1
    accents = sorted(index_of["BD"] + index_of["SD"])
    if len(index_of["BD"]) < 4 or len(index_of["SD"]) < 4:
        return None
    if on_beats < 4 * between:
        return False
    phases = np.zeros(len(beats), dtype=int)
    for start in range(0, len(beats), 12):
        here = [k for k in accents if start - 6 <= k < start + 18]
        if here:
            counts = np.bincount([k % 3 for k in here], minlength=3)
            phases[start:start + 12] = int(np.argmax(counts))
        elif start:
            phases[start:start + 12] = phases[start - 1]
    on_group = [k for k in accents if k % 3 == phases[k]]
    snares_on_group = [k for k in index_of["SD"] if k % 3 == phases[k]]
    if len(on_group) < 0.8 * len(accents) or len(snares_on_group) < 0.6 * len(index_of["SD"]):
        return False
    return [k for k in range(len(beats)) if k % 3 == phases[k]]


def downbeat_phase(beats, downbeats, per_measure):
    """The time of a beat that begins a measure, as most of the downbeats the beat model found say (a downbeat
    it misplaced now and then does not move the measures), or None when they do not agree."""
    if not len(downbeats) or len(beats) < 2:
        return None
    beat_len = float(np.median(np.diff(beats)))
    votes = np.zeros(per_measure)
    for d in downbeats:
        k = int(np.argmin(np.abs(beats - d)))
        if abs(beats[k] - d) < 0.25 * beat_len:
            votes[k % per_measure] += 1
    if votes.sum() < 2 or votes.max() < 0.7 * votes.sum():
        return None
    return float(beats[int(np.argmax(votes))])


HALF_SNARES = 0.5              # share of the backbeats followed by one four beats on (not two): at half the speed
HALF_SLOWEST = 50.0            # beats a minute the halved tempo is at least


def half_tempo(beats, strokes):
    """The beat a minute a chart counts, when the beats found are twice as fast as that, else None. A slow
    groove with the hi-hat in sixteenths (a ballad at 70) sounds like one in eighths at 140, and a beat model
    may count it so; then its snare - the backbeat, on two and four - comes every four beats, not every two.
    A groove at the speed it was counted has its backbeat every other beat, as most charts write it: fast
    punk at 180 with eighths on the hi-hat stays as it is. A groove with the snare on three alone all through
    is counted at the slower tempo, with the backbeat on two and four - what a chart of it writes; where it is
    a passage of a song otherwise on two and four (a half-time chorus), most of the snare's intervals are two
    beats long: the song keeps its tempo. Measured in seconds, not in beats: a beat model that hears such a
    groove at the faster tempo often misses a beat here and there, or counts a passage at the slower one. Not
    when the hi-hat's strokes come a quarter of a beat apart (they would be thirty-second notes; swung
    sixteenths, a third of a beat and two, are not), or the tempo halved would be slower than HALF_SLOWEST."""
    beats = np.asarray(beats, dtype=float)
    if len(beats) < 16:
        return None
    beat_len = float(np.median(np.diff(beats)))
    if 30.0 / beat_len < HALF_SLOWEST:
        return None
    # (the backbeat: the snare strokes about as loud as the loud ones - not ghost notes, or another drum's noise)
    levels = [strength for _, d, strength in strokes if d == "SD"]
    loud = np.percentile(levels, 80) - GHOST_DB if levels else 0.0
    snares = np.array(sorted(t for t, d, strength in strokes if d == "SD" and strength >= loud))
    hats = np.array(sorted(t for t, d, _ in strokes if d in ("HH", "HO", "RD")))
    if len(snares) < 9:
        return None
    # for each snare stroke: is there one two beats later, or (and none two beats later) four beats later
    two, four = 0, []
    for t in snares:
        later = (snares - t) / beat_len
        if np.any(np.abs(later - 2.0) < 0.2):
            two += 1
        elif np.any(np.abs(later - 4.0) < 0.35):
            four.append(float(later[np.argmin(np.abs(later - 4.0))]))
    if len(four) < HALF_SNARES * len(snares) or two > 0.25 * len(four):
        return None
    if len(hats) > 8:
        hat_gaps = np.diff(hats) / beat_len
        hat_gaps = hat_gaps[hat_gaps > 0.1]
        if len(hat_gaps) and np.mean(hat_gaps < 0.3) > 0.25:
            return None
    return 120.0 / (float(np.median(four)) * beat_len)


DOUBLE_FASTEST = 210.0         # beats a minute the doubled tempo is at most


def double_tempo(beats, strokes):
    """The beat a minute a chart counts, when the beats found are half as fast as that, else None: fast punk
    (bass drum on one and three, snare on two and four, at 180) counted at 90 has its snare between the beats,
    on the "and", and its bass drum on them. (A groove with the snare on every beat - Motown - has it on the
    beats.)"""
    beats = np.asarray(beats, dtype=float)
    if len(beats) < 8:
        return None
    beat_len = float(np.median(np.diff(beats)))
    if 120.0 / beat_len > DOUBLE_FASTEST:
        return None
    levels = [strength for _, d, strength in strokes if d == "SD"]
    loud = np.percentile(levels, 80) - GHOST_DB if levels else 0.0

    def offsets(drum, quiet=-1000.0):
        found = []
        for t, d, strength in strokes:
            i = int(np.searchsorted(beats, t + 0.25 * beat_len)) - 1
            if d == drum and strength >= quiet and 0 <= i < len(beats) - 1:
                found.append((t - beats[i]) / (beats[i + 1] - beats[i]))
        return np.array(found)

    snares, kicks = offsets("SD", loud), offsets("BD")
    if len(snares) < 8 or len(kicks) < 8:
        return None
    if np.mean(np.abs(snares - 0.5) < 0.12) >= 0.7 and np.mean(np.abs(kicks) < 0.12) >= 0.45:
        return 120.0 / beat_len
    return None


def find_meter(beats, downbeats, strokes):
    """Engine 2's time signature (meter "auto"): (meter, the beats - a quarter, or in 6/8 and 12/8 a dotted
    quarter - and the time of a downbeat, or None to find it from the groove). The downbeats the beat model
    found give the beats to a measure; the strokes tell a 6/8 or a 12/8: beats grouped in threes (the model
    counted the eighths), or beats divided in three with their middle eighth struck - a shuffle in 4/4 is
    divided in three too, but leaves the middle out. Where the downbeats do not agree on the beats to a measure
    (a beat model hears a measure less surely in drums alone than in a song), the groove tells it: how it
    repeats every three (or six) beats or every four. Two beats to a measure (simple time) are written as 4/4: a chart
    of a groove with the snare on two and four writes it so."""
    counts = [int(((beats >= a - 0.05) & (beats < b - 0.05)).sum()) for a, b in zip(downbeats, downbeats[1:])]
    first_downbeat = float(downbeats[0]) if len(downbeats) else None
    usual = max(set(counts), key=counts.count) if counts else 4
    if counts and counts.count(usual) >= 0.6 * len(counts):
        per_bar = usual
    else:
        contents = beat_contents(strokes, beats)[0] if len(beats) > 2 else []
        triple = max(repetition(contents, 3), repetition(contents, 6))
        per_bar = 3 if triple > repetition(contents, 4) + 0.08 else 4
        first_downbeat = downbeat_phase(beats, downbeats, per_bar)
    beat_tempo = 60.0 / float(np.median(np.diff(beats)))

    # the eighths of a compound meter: every third beat begins a dotted quarter
    groups = grouped_in_threes(beats, strokes) if beat_tempo > 100 else None
    if groups:
        dotted = beats[groups]
        # two dotted quarters to a measure, or four when the downbeats come every twelve eighths
        per_downbeat = [int(((dotted >= a - 0.05) & (dotted < b - 0.05)).sum()) for a, b in zip(downbeats, downbeats[1:])]
        meter = "12/8" if per_downbeat and np.median(per_downbeat) >= 3.5 else "6/8"
        return meter, dotted, downbeat_phase(dotted, downbeats, METERS[meter][0])

    thirds_share, between, middle = ternary_evidence(strokes, beats)
    compound = thirds_share > 0.7 and between >= max(8, len(strokes) // 6) and middle >= 0.5
    if per_bar == 2:
        if compound:
            return "6/8", beats, downbeat_phase(beats, downbeats, 2)
        return "4/4", beats, None
    if per_bar == 4 and compound:
        return "12/8", beats, downbeat_phase(beats, downbeats, 4)
    if per_bar == 3:
        # three fast beats to the "measure", too few bass drums and snares to tell how they group: the eighths
        # of a 6/8 likely, a dotted quarter to each "measure" - two of them to the measure, which of them begins
        # it found from the groove (a waltz has its snare on two and three: no groups of three, a 3/4)
        if groups is None and beat_tempo > 150 and len(downbeats) > 2:
            return "6/8", downbeats, None
        return "3/4", beats, first_downbeat
    if compound and per_bar in (6, 12):
        return ("6/8" if per_bar == 6 else "12/8"), beats, first_downbeat
    return "4/4", beats, first_downbeat


def compound_beats(beats, downbeats, strokes, per_measure):
    """6/8 or 12/8 asked for: the dotted quarters, from beats that may be the eighths (grouped in threes by the
    groove, or by the downbeats), and the time of a downbeat (or None)."""
    first_downbeat = float(downbeats[0]) if len(downbeats) else None
    groups = grouped_in_threes(beats, strokes)
    if groups and len(groups) >= 2:
        dotted = beats[groups]
        return dotted, downbeat_phase(dotted, downbeats, per_measure)
    if first_downbeat is not None and len(downbeats) > 1:
        # a model that counts the eighths of a 6/8: every third of them, from the downbeat, is the beat
        per_bar = np.median(np.diff(np.searchsorted(beats, downbeats - 0.05)))
        if per_bar >= 3 * per_measure - 0.5:
            # (from the downbeats, the measures divided evenly: an eighth the model missed or heard twice
            # does not move the beats after it a third of a beat off)
            measure_len = float(np.median(np.diff(downbeats)))
            beats = np.concatenate(
                [np.linspace(a, b, per_measure * max(1, int(round((b - a) / measure_len))), endpoint=False)
                 for a, b in zip(downbeats[:-1], downbeats[1:])] + [downbeats[-1:]])
            return beats, first_downbeat
    if 60.0 / float(np.median(np.diff(beats))) > 110:
        # beats far too fast for dotted quarters, and no groups found in them: every third one, from the downbeat
        start = int(np.argmin(np.abs(beats - first_downbeat))) % 3 if first_downbeat is not None else 0
        # (a clip of a few eighths gives one or two dotted quarters: too few to make measures of, kept as they are)
        if len(beats[start::3]) >= 2:
            return beats[start::3], first_downbeat
    return beats, downbeat_phase(beats, downbeats, per_measure)


# --- the beat ---------------------------------------------------------------------------------------

def estimate_tempo(onset_curve, frame_time, low=60.0, high=200.0, usual=110.0):
    """Beats a minute, from the regularity of the onsets (autocorrelation), between "low" and "high",
    leaning to "usual"."""
    curve = onset_curve - onset_curve.mean()
    corr = signal.fftconvolve(curve, curve[::-1], mode="full")[len(curve) - 1:]
    best, best_score = usual, -1.0
    for bpm in np.arange(low, high, 0.25):
        lag = 60.0 / bpm / frame_time
        score = 0.0
        for multiple, weight in ((1, 1.0), (2, 0.5), (0.5, 0.25), (4, 0.25)):
            index = int(round(lag * multiple))
            if 0 < index < len(corr):
                score += weight * corr[index]
        score *= np.exp(-0.5 * (np.log2(bpm / usual) / 0.9) ** 2)
        if score > best_score:
            best, best_score = bpm, score
    return best


def track_beats(onset_curve, frame_time, bpm):
    """Beat times by dynamic programming (as in Ellis 2007): beats on onsets, about a beat apart."""
    period = 60.0 / bpm / frame_time
    curve = onset_curve / (onset_curve.std() + 1e-9)
    n = len(curve)
    score = curve.copy()
    back = np.full(n, -1)
    tightness = 100.0
    lo, hi = int(round(period * 0.5)), int(round(period * 2.0))
    offsets = np.arange(lo, hi + 1)
    penalty = -tightness * (np.log(offsets / period)) ** 2
    for t in range(hi, n):
        candidates = score[t - offsets] + penalty
        k = int(np.argmax(candidates))
        score[t] = curve[t] + candidates[k]
        back[t] = t - offsets[k]
    # from the best ending, back to the start
    end = int(np.argmax(score[max(0, n - hi):])) + max(0, n - hi)
    beats = [end]
    while back[beats[-1]] >= 0:
        beats.append(back[beats[-1]])
    beats = np.array(beats[::-1], dtype=float)
    # before the first tracked beat: continue at the tempo, down to the very start
    while beats[0] - period > -0.2 * period:
        beats = np.insert(beats, 0, beats[0] - period)
    return beats * frame_time + FRAME_CENTRE


# --- the tab ----------------------------------------------------------------------------------------

LINES = ["CC", "RD", "HH", "T1", "T2", "SD", "FT", "BD"]


SIGNS = {
    "BD": ("BD", "o"), "SD": ("SD", "o"), "T1": ("T1", "o"), "T2": ("T2", "o"), "FT": ("FT", "o"),
    "HH": ("HH", "x"), "HO": ("HH", "o"), "CC": ("CC", "X"), "RD": ("RD", "x"),
    "SG": ("SD", "g"),        # a snare stroke engine 2 heard weakly: a ghost note
}


# beats to the measure, and whether a beat is a dotted quarter of three eighths (compound time)
# beats on each side of a measure's first one that tell where it begins (beat_times): two measures of 4/4
MEASURE_FIT = 8
TIMING_SIGNS = {("BD", "o"), ("SD", "o"), ("SD", "g"), ("HH", "x")}
METERS = {"4/4": (4, False), "3/4": (3, False), "2/4": (2, False), "6/8": (2, True), "12/8": (4, True)}


def to_tab(strokes, beats, duration, meter="4/4", ghost_notes=False, first_downbeat=None, measure_times=None):
    """The beats in measures of the meter; within a beat the strokes go on sixteenth notes, or - in a
    measure whose strokes fall on thirds of the beat (a shuffle, a triplet fill) - on triplet eighths. In
    compound time a beat holds three eighths, or six sixteenths. The first beat of a measure is where the
    bass drum and the snare usually fall (bass drum on the strong beats, snare on the backbeats).
    measure_times, a list, gets where each measure of the tab begins in the recording (seconds), and where
    the last one ends: the beat it begins on."""
    if len(beats) < 2:
        raise SystemExit("No beat could be found in this recording.")
    per_measure, compound = METERS.get(meter, METERS["4/4"])
    beat_len = float(np.median(np.diff(beats)))
    # a beat before the first one too: a pickup is not lost
    beats = [beats[0] - beat_len] + list(beats)
    while beats[-1] < duration + beat_len:
        beats.append(beats[-1] + beat_len)
    beats = np.array(beats)

    snare_levels = [s for _, d, s in strokes if d == "SD"]
    # well under the usual snare - the strong ones (a groove full of ghost notes would pull a median down to them)
    ghost_level = np.percentile(snare_levels, 80) - GHOST_DB if snare_levels else -100

    # every stroke as (beat, fraction of the beat)
    placed = []
    for time, drum, strength in strokes:
        i = int(np.searchsorted(beats, time + beat_len / 8.0) - 1)
        if i < 0 or i >= len(beats) - 1:
            continue
        length = beats[i + 1] - beats[i]
        frac = (time - beats[i]) / length
        line, sign = SIGNS[drum]
        if drum == "SG" and not ghost_notes:
            continue
        if drum == "SD" and strength < ghost_level:
            sign = "g"          # (for now: the backbeats tell the usual snare better, below)
        placed.append((i, frac, line, sign, strength, time))
    if not placed:
        return "", 0, []

    def grid_position(i, frac, parts):
        step = int(round(frac * parts))
        if step >= parts:           # the very end of a beat is the start of the next one
            return i + 1, 0, abs(frac * parts - step) / parts
        return i, max(step, 0), abs(frac * parts - step) / parts

    strong = {4: (0, 2), 3: (0,), 2: (0,)}[per_measure]
    backbeats = {4: (1, 3), 3: (1, 2), 2: (1,)}[per_measure]

    # which beat is the first of the measure
    def fit(shift):
        score = 0
        for i, frac, line, sign, *_ in placed:
            beat, step, _ = grid_position(i, frac, 4)
            if step != 0:
                continue
            b = (beat + shift) % per_measure
            if line == "BD":
                score += 2 if b in strong else (-1 if b in backbeats else 0)
            if line == "SD" and sign == "o":
                score += 2 if b in backbeats else (-1 if b in strong else 0)
            if line == "CC" and b == 0:
                score += 2
        return score

    first_beat = min(stroke[0] for stroke in placed)
    # A groove of bass drum on 1 and 3 and snare on 2 and 4 fits a shift of half a measure as well:
    # the earlier shift wins a tie, and a recording usually begins on a downbeat.
    shift = max(range(per_measure),
                key=lambda shift: (fit(shift) + (3 if (first_beat + shift) % per_measure == 0 else 0), -shift))
    if first_downbeat is not None:
        # the downbeats found (engine 2): the beat nearest the first one begins a measure
        shift = (-int(np.argmin(np.abs(beats - first_downbeat)))) % per_measure

    # Ghost notes: a snare far under the usual one - the snare of the backbeats, when the groove has them (the
    # loudest snares may be the accents of a fill). A snare on a backbeat is never a ghost note. A chart
    # leaves ghost notes out (and a weak "snare" is often another drum's noise) unless they are asked for.
    def on_backbeat(i, frac):
        beat, step, _ = grid_position(i, frac, 4)
        return step == 0 and (beat + shift) % per_measure in backbeats
    main_snares = [s for i, frac, line, _, s, _ in placed if line == "SD" and on_backbeat(i, frac)]
    if len(main_snares) >= 8:
        ghost_level = float(np.median(main_snares)) - GHOST_DB
    kept = []
    for i, frac, line, sign, strength, time in placed:
        if line == "SD" and sign in "og":
            sign = "g" if strength < ghost_level and not on_backbeat(i, frac) else "o"
            if sign == "g" and not ghost_notes:
                continue
        kept.append((i, frac, line, sign, strength, time))
    placed = kept
    if not placed:
        return "", 0, []

    # per measure: the plain division of the beat, or the other one when the strokes fit it clearly better
    plain, other = (3, 6) if compound else (4, 3)
    by_measure = {}
    for stroke in placed:
        i, frac = stroke[0], stroke[1]
        by_measure.setdefault((grid_position(i, frac, plain)[0] + shift) // per_measure, []).append(stroke)
    # the score begins with the first measure that has a stroke (the beat added before the first one may
    # have made an empty one)
    first_measure = min(by_measure)
    by_measure = {m - first_measure: strokes_here for m, strokes_here in by_measure.items()}
    shift -= first_measure * per_measure
    measures = max(by_measure) + 1

    rows = {}
    measure_state = []
    for m in range(measures):
        strokes_here = by_measure.get(m, [])
        error_plain = sum(grid_position(i, f, plain)[2] for i, f, *_ in strokes_here)
        error_other = sum(grid_position(i, f, other)[2] for i, f, *_ in strokes_here)
        # sixteenths in compound time: strokes between the eighths; triplets in simple time: on the thirds
        between = any(grid_position(i, f, other)[1] % (2 if compound else 3) != 0 for i, f, *_ in strokes_here) if compound \
            else any(grid_position(i, f, other)[1] in (1, 2) for i, f, *_ in strokes_here)
        parts = other if strokes_here and between and error_other < 0.6 * error_plain else plain
        cells = {}
        hat_strength = {}
        hat_ring = {}
        metal = {}          # (line, step) -> (brightness, ring) of the hats and cymbals struck alone
        cymbal_ring = {}    # step -> how long the cymbal struck there rings (engine 2)
        cymbal_hat_ring = {}    # step -> how it rings, measured as a hi-hat is
        sure = {}           # (line, step) -> how surely the drum model heard the stroke (engine 2)
        for i, frac, line, sign, strength, time in strokes_here:
            beat, step, error = grid_position(i, frac, parts)
            beat_in_measure = beat + shift - m * per_measure
            if not 0 <= beat_in_measure < per_measure:
                continue
            # metal halfway between two steps (a 32nd note) and heard weakly: a cymbal's wash struck up again,
            # not a stroke a chart writes (a drum there is more likely a flam, a drag or a swung note)
            if line in ("HH", "RD", "CC") and error * parts > OFF_GRID \
                    and CONFIDENCE.get((time, line), 1.0) < METAL_SURE:
                continue
            row = cells.setdefault(line, ["-"] * (parts * per_measure))
            index = beat_in_measure * parts + step
            # an open hi-hat wins over a closed one on the same step
            if not (line == "HH" and row[index] == "o"):
                row[index] = sign
            if line == "HH":
                hat_strength[index] = max(hat_strength.get(index, -100.0), strength)
                if HAT_RINGS.get(time) is not None:
                    hat_ring[index] = max(hat_ring.get(index, 0.0), HAT_RINGS[time])
            if line in ("HH", "CC", "RD") and METAL.get(time) is not None:
                metal[(line, index)] = METAL[time]
            if (time, line) in CONFIDENCE:
                sure[(line, index)] = max(sure.get((line, index), 0.0), CONFIDENCE[(time, line)])
            if line in ("CC", "RD") and time in CYMBAL_RINGS:
                cymbal_ring[index] = max(cymbal_ring.get(index, 0.0), CYMBAL_RINGS[time])
                if HAT_RINGS.get(time) is not None:
                    cymbal_hat_ring[index] = max(cymbal_hat_ring.get(index, 0.0), HAT_RINGS[time])
        keeper, clear = time_keeper(cells, hat_ring, metal, cymbal_ring, cymbal_hat_ring)
        measure_state.append([parts, cells, hat_strength, hat_ring, cymbal_ring, cymbal_hat_ring, metal, keeper, clear,
                              sure])

    # A drummer keeps the time on one thing for a passage, not for a measure: a measure whose own sound tells
    # it less surely than the measures around it tell another goes with them.
    keepers = [state[7] for state in measure_state]
    for m, state in enumerate(measure_state):
        if keepers[m] is None:
            continue
        votes = {keepers[m]: 2.0 * state[8]}
        for n in range(max(0, m - 2), min(measures, m + 3)):
            if n != m and keepers[n] is not None:
                votes[keepers[n]] = votes.get(keepers[n], 0.0) + measure_state[n][8]
        state[7] = max(votes, key=lambda k: (votes[k], k == keepers[m]))

    hat_kind = "x"          # open ("o") or closed ("x"): the last measure's hi-hat
    sure_of = {}
    for m, state in enumerate(measure_state):
        parts, cells, hat_strength, hat_ring, cymbal_ring, cymbal_hat_ring, metal, keeper, _, sure = state
        sure_of[m] = sure
        width = parts * per_measure

        def move(line_name, to, x):
            if (line_name, x) in sure:
                sure[(to, x)] = max(sure.get((to, x), 0.0), sure.pop((line_name, x)))

        if keeper == "HH":
            # the cymbal strokes that do not ring on as a crash does are hi-hat strokes the model took for a
            # cymbal; the first stroke of the measure on the crash is an accent, and so is one later in the
            # measure that rings far on and that the model heard surely (an open hi-hat rings on too)
            row = cells.setdefault("HH", ["-"] * width)
            for line_name in ("RD", "CC"):
                for x, c in enumerate(cells.pop(line_name, [])):
                    if c == "-":
                        continue
                    if line_name == "CC" and (x == 0 or cymbal_ring.get(x, 0.0) > CRASH_ACCENT
                                              and sure.get(("CC", x), 1.0) >= METAL_SURE):
                        cells.setdefault("CC", ["-"] * width)[x] = "X"
                    elif row[x] == "-":
                        row[x] = "x"
                        move(line_name, "HH", x)
                        if x in cymbal_hat_ring:
                            hat_ring[x] = cymbal_hat_ring[x]
                        hat_strength.setdefault(x, 0.0)
        elif keeper:
            # the hats and the other cymbal go to it; only the first stroke of the measure may be an accent on
            # the crash (time kept is not accented)
            row = cells.setdefault(keeper, ["-"] * width)
            for line_name in ("HH", "RD", "CC"):
                if line_name == keeper:
                    continue
                for x, c in enumerate(cells.pop(line_name, [])):
                    if c == "-":
                        continue
                    if x == 0 and line_name == "CC":
                        cells.setdefault("CC", ["-"] * width)[0] = "X"
                    elif row[x] == "-":
                        row[x] = "x"
                        move(line_name, keeper, x)
            # (time kept on the crash: the accent only where the passage begins)
            first = m == 0 or measure_state[m - 1][7] != keeper
            cells[keeper] = [c if (x == 0 and keeper == "CC" and first) or c == "-" else "x"
                             for x, c in enumerate(cells[keeper])]
        if keeper and parts == 4 and not compound and keeper in cells:
            # Time kept in eighths: strokes between them that the model heard weakly are the wash of the metal,
            # not sixteenths a chart would write (the hi-hat's are weighed by loudness below, too).
            row = cells[keeper]
            between = [x for x in range(1, width, 2) if row[x] != "-"]
            eighths = sum(row[x] != "-" for x in range(0, width, 2))
            if eighths >= 5 and len(between) <= eighths // 2:
                for x in between:
                    if sure.get((keeper, x), 1.0) < METAL_SURE:
                        row[x] = "-"
        hats = cells.get("HH")
        if hats:
            # In sixteenths: a hat between the eighths far weaker than those on them is the noise of another
            # drum (a ghost note's wires), not a stroke of its own.
            on_eighths = [hat_strength[x] for x in hat_strength if x % 2 == 0]
            if parts == 4 and not compound and len(on_eighths) >= 3:
                usual = float(np.median(on_eighths))
                for x in [x for x in hat_strength if x % 2 and hat_strength[x] < usual - HAT_WEAK]:
                    hats[x] = "-"
            # Open or closed, for the measure (a chart says it so, "open hi-hat" for a passage): the hats that
            # clearly ring on, or are clearly cut short, vote; those struck with the snare cannot be told, and
            # a measure without a clear one follows the one before. One that rings on far longer than the
            # rest of a closed measure is an open one of its own.
            votes = [hat_ring[x] for x in hat_ring if hats[x] != "-" and not CLOSED_RING <= hat_ring[x] <= OPEN_RING]
            if votes:
                hat_kind = "o" if sum(v > OPEN_RING for v in votes) > len(votes) / 2 else "x"
            for x, c in enumerate(hats):
                if c != "-":
                    hats[x] = "o" if hat_kind == "o" or hat_ring.get(x, 0.0) > OPEN_ACCENT else "x"
            if all(c == "-" for c in hats):
                del cells["HH"]
        if not CYMBAL_RINGS:
            # Time kept on a cymbal (a crash or a ride in eighths, as in a chorus) sounds like a hi-hat to the
            # bands above, but is darker: when the hats and cymbals struck alone in a measure are, the measure's
            # hats go to the cymbal - the crash if it rings on, the ride if not.
            alone = list(metal.values())
            if "HH" in cells and len(alone) >= 3 and float(np.median([b for b, _ in alone])) < CYM_BRIGHTNESS:
                line = "CC" if float(np.median([ring for _, ring in alone])) > 0.3 else "RD"
                row = cells.setdefault(line, ["-"] * len(cells["HH"]))
                for x, c in enumerate(cells.pop("HH")):
                    if c != "-" and row[x] == "-":
                        row[x] = "x"
        rows[m] = (parts, cells)

    def heard(m, x, parts):
        """how strongly the drum model heard a hi-hat or a cymbal at step x of measure m (1: at its threshold)"""
        if not MODEL_ACTIVATIONS:
            return 0.0
        activations, thresholds = MODEL_ACTIVATIONS[0]
        beat = m * per_measure - shift + x // parts
        if not 0 <= beat < len(beats) - 1:
            return 0.0
        f = int(round((beats[beat] + (x % parts) / parts * (beats[beat + 1] - beats[beat])) * MODEL_FPS))
        near = slice(max(0, f - 3), f + 4)
        return max(float(activations[near, c].max(initial=0.0)) / thresholds[c]
                   for c in (DRUM_CLASSES.index("HH"), DRUM_CLASSES.index("CY")))
    snap_to_groove(rows, sure_of, measures, heard)

    # no empty measures at the end (a stroke that rang past the last beat counts for none)
    while measures > 1 and not rows[measures - 1][1]:
        measures -= 1

    sections = find_sections(rows, measures)

    if measure_times is not None:
        # the strokes struck on a beat that tell where it is: the drum model hears a bass drum, a snare and a
        # closed hi-hat within a few milliseconds of where they are struck (toms and cymbals less surely)
        struck = [(i + (frac > 0.5), time) for i, frac, line, sign, _, time in placed
                  if (line, sign) in TIMING_SIGNS and min(abs(frac), abs(1.0 - frac)) < 0.08]
        measure_times[:] = [round(t, 4) for t in beat_times(beats, [m * per_measure - shift for m in range(measures + 1)], struck)]

    used = [line for line in LINES if any(line in cells for _, cells in rows.values())]
    text = []
    for row_start in range(0, measures, 4):
        for line in used:
            cells = []
            for m in range(row_start, min(measures, row_start + 4)):
                parts, measure_cells = rows[m]
                cells.append("".join(measure_cells.get(line, ["-"] * (parts * per_measure))))
            text.append(line + "|" + "|".join(cells) + "|")
        text.append("")
    return "\n".join(text).strip() + "\n", measures, sections


def beat_times(beats, indices, struck=(), around=MEASURE_FIT):
    """Where the beats of these indices are, finer than the beats found (the beat model gives them on a grid of
    20 ms, and a little off the strokes): read off the line through the strokes struck on the beats around -
    struck: (beat index, time) - that fits them best: the strokes before the beat, after it, or both, whichever
    lie on a line more closely (so that a change of tempo stays where it is). Without strokes enough there, off
    the line through the beats around. A beat before the first or after the last is one usual beat on."""
    beats = np.asarray(beats, dtype=float)
    beat_len = float(np.median(np.diff(beats)))
    grid = np.array([g for g, _ in struck], dtype=float)
    times_struck = np.array([t for _, t in struck], dtype=float)

    def line(g, t, at, within):
        """(mean square error, time at "at") of the line through the points, those far off it left out. The
        points kept must still be most of them, on four beats or more: a line through the strokes on either
        side of a sudden change of tempo fits a few of them (two beats with two strokes each fit exactly),
        not the strokes - it is no line through them."""
        if len(g) < 4 or np.ptp(g) < 2:
            return None
        slope, start = np.polyfit(g - at, t, 1)
        keep = np.abs(t - (slope * (g - at) + start)) < within
        if keep.sum() < 4 or keep.sum() < 0.6 * len(g) or len(np.unique(g[keep])) < 4 or np.ptp(g[keep]) < 2:
            return None
        if not keep.all():
            slope, start = np.polyfit(g[keep] - at, t[keep], 1)
        if not 0.7 * beat_len < slope < 1.4 * beat_len:
            return None
        return float(np.mean((t[keep] - (slope * (g[keep] - at) + start)) ** 2)), float(start)

    times = []
    for i in indices:
        fits = []
        for weight, low, high in ((1.0, i - around, i + around), (2.0, i - around, i), (2.0, i, i + around)):
            near = (grid >= low) & (grid <= high)
            found = line(grid[near], times_struck[near], i, 0.03)
            if found:
                fits.append((found[0] * weight, found[1]))
        if fits:
            times.append(min(fits)[1])
        elif 0 <= i < len(beats):
            near = np.arange(max(0, i - around), min(len(beats), i + around + 1))
            found = line(near.astype(float), beats[near], i, 0.05)
            times.append(found[1] if found else float(beats[i]))
        elif i < 0:
            times.append(float(beats[0] + i * beat_len))
        else:
            times.append(float(beats[-1] + (i - len(beats) + 1) * beat_len))
    return times


def snap_to_groove(rows, sure_of, measures, heard=None):
    """A chart writes a passage's groove as one. A stroke that none of the measures around plays, and that the
    drum model heard less surely than a stroke is heard, is left out - most often another drum's sound (the
    low end of a snare taken for a bass drum, an open hi-hat's wash taken for a stroke between the eighths).
    And where the measures around keep the time on a step this one has nothing on, while the model heard
    metal there - only not enough to call it a stroke (an open hi-hat struck again while it rings, a hi-hat
    under the snare) - the stroke is written. Fills stay: a measure unlike those around it is left as it is,
    and so are strokes off the groove that come together (a figure, not a stray stroke). Engine 2 only: the
    confidences (sure_of: per measure, (line, step) -> 0 to 1) and heard(m, step, parts) - how strongly the
    model heard metal at that step of measure m, 1 at its threshold - come from the drum model."""
    metal = ("HH", "RD", "CC")

    def strokes_of(cells):
        # (line, step), the hi-hat and the cymbals as one line: what keeps the time
        return {("metal" if line in metal else line, x)
                for line, row in cells.items() for x, c in enumerate(row) if c != "-"}

    for m in range(measures):
        parts, cells = rows[m]
        sure = sure_of.get(m, {})
        around = [strokes_of(rows[n][1]) for n in range(m - 2, m + 3)
                  if n != m and 0 <= n < measures and rows[n][0] == parts and rows[n][1]]
        if len(around) < 2:
            continue
        here = strokes_of(cells)
        if not here or max(len(here & other) / len(here | other) for other in around) < SNAP_ALIKE:
            continue            # a fill, or a break
        off = {(k, x) for k, x in here if not any((k, x) in other for other in around)}
        toms = [cells[tom] for tom in ("T1", "T2", "FT") if tom in cells]
        for line in ("BD", "SD", "HH", "RD"):
            row = cells.get(line)
            if not row:
                continue
            kind = "metal" if line in metal else line
            for x, c in enumerate(row):
                cutoff = METAL_SURE if kind == "metal" else SNAP_SURE
                if c == "-" or (kind, x) not in off or sure.get((line, x), 1.0) >= cutoff:
                    continue
                if any(abs(y - x) <= parts // 2 and (k, y) != (kind, x) for k, y in off) or \
                        any(tom[x] != "-" for tom in toms):
                    continue    # a figure, or with a tom: a fill
                row[x] = "-"
            if all(c == "-" for c in row):
                del cells[line]
        # the time kept: one line (the hi-hat, the ride or the crash), and what the measures around keep it on
        keepers = [line for line in ("HH", "RD") if line in cells] or [line for line in metal if line in cells]
        if heard is None or len(around) < 3 or len(keepers) != 1:
            continue
        row = cells[keepers[0]]
        signs = [c for c in row if c != "-"]
        sign = max(set(signs), key=signs.count)
        for x in range(len(row)):
            if any(cells[line][x] != "-" for line in metal if line in cells):
                continue
            if sum(("metal", x) in other for other in around) < len(around) - 1:
                continue
            if heard(m, x, parts) >= FILL_HEARD:
                row[x] = sign


def time_keeper(cells, hat_ring, metal, cymbal_ring, cymbal_hat_ring):
    """Engine 2: what keeps the time in a measure - the hi-hat ("HH"), the ride ("RD") or the crash ("CC"), one
    of them as a chart writes it, or None (no metal, or engine 1) - and how surely the measure's own sound
    tells it (0 to 1). The drum model takes an open hi-hat for a cymbal often, and a ride for a hi-hat now and
    then. Three or more cymbal strokes keep the time on a cymbal, unless the metal of the measure sounds like
    a hi-hat: as bright as one, and cut short (closed) or ringing on (open) as one does - a ride rings in
    between, and a crash rings on in the upper middle too. Fewer, with a hi-hat playing: the hi-hat."""
    cymbal_steps = sorted({x for ln in ("CC", "RD") for x, c in enumerate(cells.get(ln, [])) if c != "-"})
    hat_steps = [x for x, c in enumerate(cells.get("HH", [])) if c != "-"]
    if not CYMBAL_RINGS or not (cymbal_steps or hat_steps):
        return None, 0.0
    if not cymbal_steps:
        return "HH", 1.0
    rings = [hat_ring[x] for x in hat_steps if x in hat_ring] + \
        [cymbal_hat_ring[x] for x in cymbal_steps if x in cymbal_hat_ring]
    brightness = [b for b, _ in metal.values()]
    # (the brightness is measured on metal struck alone, the ring on metal struck without the snare: a
    # measure with little of either tells little)
    sure = max(0.2, min(1.0, min(len(rings), len(brightness)) / 3.0))
    hat_like = len(rings) >= 2 and len(brightness) >= 2 and float(np.median(brightness)) > CYM_BRIGHTNESS \
        and not CLOSED_RING <= float(np.median(rings)) <= KEPT_OPEN_RING
    if len(cymbal_steps) >= 3 and not hat_like:
        crash = float(np.median([cymbal_ring.get(x, 0.0) for x in cymbal_steps])) > 0.3
        # (the model's own word counts too: cymbal strokes, and hi-hat strokes of their own)
        alone = len(set(hat_steps) - set(cymbal_steps))
        return ("CC" if crash else "RD"), max(sure, len(cymbal_steps) / (len(cymbal_steps) + 2.0 * alone))
    if hat_steps or len(cymbal_steps) >= 3:
        return "HH", (sure if len(cymbal_steps) >= 3 else 1.0)
    return None, 0.0


def beat_contents(strokes, beats):
    """What is struck in each beat: a set of (drum line, place in the beat on a grid of twelfths), and the
    share of the strokes between the beats that fall on thirds of the beat rather than on sixteenths."""
    beat_len = float(np.median(np.diff(beats)))
    contents = [set() for _ in beats]
    thirds = quarters = 0
    for time, drum, _ in strokes:
        i = int(np.searchsorted(beats, time + beat_len / 8.0) - 1)
        if not 0 <= i < len(beats) - 1:
            continue
        frac = (time - beats[i]) / (beats[i + 1] - beats[i])
        step = int(round(frac * 12)) % 12
        contents[i].add((SIGNS[drum][0], step))
        if 0.12 < frac < 0.88:
            # nearer a third (4, 8 of 12) or a sixteenth (3, 6, 9 of 12)
            if min(abs(frac - 1 / 3), abs(frac - 2 / 3)) < min(abs(frac - 0.25), abs(frac - 0.5), abs(frac - 0.75)):
                thirds += 1
            else:
                quarters += 1
    return contents, thirds / max(1, thirds + quarters), thirds + quarters


def repetition(contents, lag):
    """How alike each beat is to the beat "lag" beats later (0 to 1); the bass drum and the snare count
    most, the hi-hat that plays on every beat less."""
    weight = {"BD": 3.0, "SD": 3.0, "CC": 1.0, "T1": 1.0, "T2": 1.0, "FT": 1.0, "HH": 0.5, "RD": 0.5}
    total = count = 0.0
    for a, b in zip(contents, contents[lag:]):
        if not a and not b:
            continue
        union = a | b
        total += sum(weight.get(line, 1.0) for line, _ in a & b) / sum(weight.get(line, 1.0) for line, _ in union)
        count += 1
    return total / count if count else 0.0


def guess_meter(strokes, onset_curve, frame_time):
    """The time signature, the beat a minute and the beats, from how the groove repeats: 3/4 when it repeats
    every three beats rather than every two or four; 6/8 when the beats divide in three and the groove
    repeats every two of them. Otherwise 4/4 (a shuffle stays in 4/4, with triplets)."""
    bpm = estimate_tempo(onset_curve, frame_time)
    beats = track_beats(onset_curve, frame_time, bpm)
    contents, triplet_share, between = beat_contents(strokes, beats)
    lag = {n: repetition(contents, n) for n in (2, 3, 4, 6)}

    # the beat as a dotted quarter: a 6/8 divides it in three and repeats every two of them
    slow = estimate_tempo(onset_curve, frame_time, 35.0, 100.0, 62.0)
    slow_beats = track_beats(onset_curve, frame_time, slow)
    slow_contents, slow_triplets, slow_between = beat_contents(strokes, slow_beats)
    slow_lag = {n: repetition(slow_contents, n) for n in (2, 3, 4)}
    compound = slow_triplets > 0.6 and slow_between >= max(8, len(strokes) // 6) \
        and slow_lag[2] > max(slow_lag[3], slow_lag[4]) + 0.08

    if triplet_share > 0.7 and between >= max(8, len(strokes) // 6):
        # the quarter divides in three: 6/8 if it groups so, a shuffle in 4/4 otherwise
        return ("6/8", slow, slow_beats) if compound else ("4/4", bpm, beats)

    if lag[3] > max(lag[2], lag[4]) + 0.08:
        # the groove repeats every three quarters: 3/4 - or two dotted quarters of a 6/8 (the same length;
        # a 6/8 groups the eighths in threes, a 3/4 in twos)
        return ("6/8", slow, slow_beats) if compound else ("3/4", bpm, beats)
    return "4/4", bpm, beats


def find_sections(rows, measures):
    """Where the groove changes and stays changed: [(first measure from 0, name)]. A one-measure change is a
    fill, not a section. A groove that comes again gets its old letter (A B A B C ...)."""
    def signature(m):
        if m < 0 or m >= measures:
            return frozenset()
        parts, cells = rows[m]
        # what the hands and feet do, on a common grid; the crash marks a downbeat, not a groove
        return frozenset((line, round(i * 48 / len(row)), sign.lower()) for line, row in cells.items() if line != "CC"
                         for i, sign in enumerate(row) if sign != "-")

    def alike(a, b):
        return len(a & b) / len(a | b) if a | b else 1.0

    # A section goes on while its measures are like its groove (its first two measures). Two measures in a
    # row unlike it, and like each other, start the next one; a single odd measure is a fill.
    starts = [0]
    groove = signature(1)
    for m in range(2, measures - 1):
        new, after = signature(m), signature(m + 1)
        if m - starts[-1] >= 2 and alike(new, groove) < 0.65 and alike(after, groove) < 0.65 and alike(new, after) >= 0.6:
            starts.append(m)
            groove = after
    if len(starts) < 2:
        return []

    names, representatives = [], []
    for start in starts:
        groove = signature(start + 1)     # (its first measure may open with a crash or end a fill)
        for name, known in zip(names, representatives):
            if alike(groove, known) >= 0.68:
                names.append(name)
                representatives.append(known)
                break
        else:
            names.append(chr(ord("A") + min(len(set(names)), 25)))
            representatives.append(groove)
    return [(start, name) for start, name in zip(starts, names)]


def main():
    parser = argparse.ArgumentParser(description="A recording to a drum tab (KitNote)")
    parser.add_argument("audio")
    parser.add_argument("--json", required=True, help="where to write the result")
    parser.add_argument("--no-separation", action="store_true", help="the recording is of drums alone")
    parser.add_argument("--tempo", type=float, default=0, help="the tempo, if known")
    parser.add_argument("--stems", help="also write the separated instruments here")
    parser.add_argument("--drums", help="the drums of the recording, separated before (a wav): no separation now")
    parser.add_argument("--meter", default="4/4", choices=sorted(METERS) + ["auto"],
                        help="the time signature (default 4/4; auto: found from the groove)")
    parser.add_argument("--accurate", action="store_true", help="separate with htdemucs_ft (better, slower)")
    parser.add_argument("--ghost-notes", action="store_true", help="also write the weak snare strokes, as ghost notes")
    parser.add_argument("--engine", type=int, choices=(1, 2), default=2,
                        help="2: the beat model (Beat This!) and KitNote's drum model, when installed; 1: band analysis")
    args = parser.parse_args()

    if args.tempo and not 30 <= args.tempo <= 300:
        raise SystemExit("The tempo must be between 30 and 300.")

    progress(2, "reading the recording")
    audio = load_audio(args.audio)
    duration = audio.shape[1] / RATE
    if audio.shape[1] < RATE // 2:
        raise SystemExit("The recording is too short (less than half a second).")

    if args.drums:
        drums = load_audio(args.drums)
    elif args.no_separation:
        drums = audio
    else:
        try:
            drums = separate_drums(audio, args.stems, "htdemucs_ft" if args.accurate else "htdemucs")
        except ImportError:
            raise SystemExit("The separation model is not installed (KitNote: Preferences > AI).")
    # (the hum of the mains, which a recording of drums alone keeps - the separation leaves it with the drums
    # now and then too - is no tom)
    drums, hum = remove_hum(drums)
    if args.no_separation:
        audio = drums

    engine = beat_model() if args.engine == 2 else None
    drum_model = None
    if engine:
        progress(72, "finding the beat and the measures")
        beats, downbeats = model_beats(engine, audio)
        progress(84, "finding the strokes")
        model = drum_model_path()
        drum_model = os.path.basename(model) if model else None
        if model:
            activations, thresholds = model_activations(model, drums)
            strokes = model_strokes(activations, drums, thresholds)
        else:
            strokes = find_strokes(drums)[0]
        if len(beats) < 4:
            engine = None       # too short or too quiet for the model: engine 1
            drum_model = None
            # (what the drum model measured is not engine 1's: its confidences, cymbal rings and activations
            # would change what engine 1 writes)
            CONFIDENCE.clear()
            CYMBAL_RINGS.clear()
            MODEL_ACTIVATIONS.clear()
    if engine:
        tab, measures, sections, bpm, measure_times = neural_tab(strokes, beats, downbeats, duration, args, drums)
        compound = METERS[args.meter][1]
    else:
        tab, measures, sections, bpm, strokes, measure_times = band_tab(drums, duration, args)
        compound = METERS[args.meter][1]

    # the tempo as quarter notes a minute, as KitNote writes it (a dotted quarter is one and a half)
    quarters = min(400.0, max(20.0, bpm * 1.5 if compound else bpm))
    result = {"tempo": round(float(quarters), 1), "beat_tempo": round(float(bpm), 1), "time_signature": args.meter,
              "measures": measures, "tab": tab, "strokes": len(strokes),
              "sections": [{"measure": start + 1, "name": name} for start, name in sections],
              "duration": round(duration, 2), "separated": not args.no_separation, "engine": 2 if engine else 1,
              # the weights the strokes were found with (KitNote's drum model), or null: the band analysis
              "drum_model": drum_model,
              # where each measure of the tab begins in the recording, and where the last one ends (seconds):
              # KitNote's playback follows them (hidden tempo changes), and measure 1 begins at the first
              "measure_times": measure_times,
              # the hum of the mains taken out before the strokes were looked for (Hz), if there was one
              "hum_removed": hum}
    with open(args.json, "w", encoding="utf-8") as out:
        json.dump(result, out, ensure_ascii=False, indent=1)
    progress(100, "done")


def beats_on_kicks_and_snares(beats, strokes):
    """Beats found on the onsets alone may lie on the hi-hat between the beats (the eighths of a 6/8 are all
    struck alike): they are moved by the share of a beat at which most bass drums and snares fall."""
    beats = np.asarray(beats, dtype=float)
    fracs = []
    for t, drum, _ in strokes:
        i = int(np.searchsorted(beats, t)) - 1
        if drum in ("BD", "SD") and 0 <= i < len(beats) - 1:
            fracs.append((t - beats[i]) / (beats[i + 1] - beats[i]))
    if len(fracs) < 8:
        return beats
    bins = 24
    counts = np.histogram(fracs, bins=bins, range=(0.0, 1.0))[0]
    counts = counts + np.roll(counts, 1) + np.roll(counts, -1)
    peak = (int(np.argmax(counts)) + 0.5) / bins
    near = [((f - peak + 0.5) % 1.0) - 0.5 for f in fracs]
    near = [x for x in near if abs(x) < 1.5 / bins]
    shift = (peak + (float(np.median(near)) if near else 0.0) + 0.5) % 1.0 - 0.5
    if abs(shift) < 0.1:
        return beats       # on them already (the strokes are placed to the nearest step anyway)
    return beats + shift * float(np.median(np.diff(beats)))


def onset_curve_of(drums):
    """(onset curve, frame time) of the drums, as engine 1 finds the beat on them - what engine 2 measured of
    its strokes (HAT_RINGS, METAL) kept."""
    kept = dict(HAT_RINGS), dict(METAL)
    _, onset_curve, frame_time = find_strokes(drums)
    HAT_RINGS.clear()
    HAT_RINGS.update(kept[0])
    METAL.clear()
    METAL.update(kept[1])
    return onset_curve, frame_time


def neural_tab(strokes, beats, downbeats, duration, args, drums=None):
    """Engine 2's beats and downbeats: the time signature (unless given), and the tab."""
    # The two models do not mark the same instant of a stroke (the beat model some 20 to 30 ms later): the
    # beats are moved onto the bass drums and snares struck on them, as the grid the strokes go on.
    beat_len = float(np.median(np.diff(beats)))
    near = [t - beats[np.argmin(np.abs(beats - t))] for t, d, _ in strokes if d in ("BD", "SD")]
    near = [o for o in near if abs(o) < 0.15 * beat_len]
    if len(near) >= 8:
        beats = beats + float(np.median(near))
        downbeats = downbeats + float(np.median(near))
    # (a beat heard twice or missed would move every measure after it)
    beats = clean_beats(beats)
    octave = None
    if not args.tempo and args.meter in ("auto", "4/4", "2/4"):
        octave = half_tempo(beats, strokes) or double_tempo(beats, strokes)
    if octave and drums is not None:
        # A slow groove in sixteenths that the beat model counted in eighths, or fast punk counted in halves:
        # the beats found again on the strokes at the tempo a chart counts (the beat model's downbeats were
        # those of half measures, or of two).
        beats = beats_on_kicks_and_snares(track_beats(*onset_curve_of(drums), octave), strokes)
        downbeats = np.array([])
    first_downbeat = float(downbeats[0]) if len(downbeats) else None
    if args.meter == "auto":
        args.meter, beats, first_downbeat = find_meter(beats, downbeats, strokes)
    elif METERS[args.meter][1]:
        beats, first_downbeat = compound_beats(beats, downbeats, strokes, METERS[args.meter][0])
    # (the beats are on a grid of 20 ms: the length of one beat is a rough measure of the tempo; the mean length of
    # eight, those of a passage at another tempo left out, is a fine one)
    span = max(1, min(8, len(beats) - 1))
    spans = beats[span:] - beats[:-span]
    usual = spans[np.abs(spans - np.median(spans)) < 0.03 * np.median(spans)]
    bpm = 60.0 * span / float(usual.mean() if len(usual) else np.median(spans))
    # The tempo given (of the beat: a quarter note, in compound time a dotted quarter) wins over a beat model
    # that heard another one: then the beats are found on the strokes at that tempo, as engine 1 finds them.
    if args.tempo and abs(bpm / args.tempo - 1.0) > 0.04 and drums is not None:
        beats = beats_on_kicks_and_snares(track_beats(*onset_curve_of(drums), args.tempo), strokes)
        first_downbeat = None
        bpm = float(args.tempo)
    progress(95, "writing the tab")
    if not strokes:
        return "", 0, [], bpm, []
    measure_times = []
    tab, measures, sections = to_tab(strokes, beats, duration, args.meter, args.ghost_notes, first_downbeat=first_downbeat,
                                     measure_times=measure_times)
    return tab, measures, sections, bpm, measure_times


def band_tab(drums, duration, args):
    """Engine 1: strokes and beat from the bands of the drums."""
    progress(75, "finding the strokes")
    strokes, onset_curve, frame_time = find_strokes(drums)

    progress(85, "finding the beat")
    beats = None
    if args.meter == "auto":
        args.meter = "4/4"
        if strokes:
            args.meter, bpm, beats = guess_meter(strokes, onset_curve, frame_time)
        if args.tempo:      # the tempo given wins; the meter found stays
            beats = None
    compound = METERS[args.meter][1]
    if beats is None:
        # the beat: a quarter note, or in compound time a dotted quarter (slower: 35 to 100 a minute)
        bpm = args.tempo or (estimate_tempo(onset_curve, frame_time, 35.0, 100.0, 62.0) if compound
                             else estimate_tempo(onset_curve, frame_time))
        beats = track_beats(onset_curve, frame_time, bpm)
    octave = None
    if not args.tempo and not compound and args.meter != "3/4":
        octave = half_tempo(beats, strokes) or double_tempo(beats, strokes)
    if octave:
        # (a slow groove in sixteenths, its eighths taken for the beat; fast punk, its halves taken for it)
        beats = beats_on_kicks_and_snares(track_beats(onset_curve, frame_time, octave), strokes)
    if not args.tempo and len(beats) > 4:
        bpm = 60.0 / np.median(np.diff(beats))

    progress(95, "writing the tab")
    measure_times = []
    if strokes:
        tab, measures, sections = to_tab(strokes, beats, duration, args.meter, args.ghost_notes, measure_times=measure_times)
    else:
        tab, measures, sections = "", 0, []       # nothing struck: no score
    return tab, measures, sections, bpm, strokes, measure_times


if __name__ == "__main__":
    main()
