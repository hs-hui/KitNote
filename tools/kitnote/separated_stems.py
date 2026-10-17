# SPDX-License-Identifier: GPL-3.0-only
"""Puts the drum performances render_kits.py played into songs - a bass that plays with the bass drum, chords on
a piano, an organ or a guitar, sometimes a lead or a voice-like line, sometimes pads - mixes and masters them,
and takes the drums out again with Demucs (htdemucs, on the CPU, as KitNote's audio import does). The drum stem
that comes out, with what bled into it from the band and what the separation took from the drums, is written
with the MIDI of the performance: training data for KitNote's drum model in the sound it hears in the app.

    python separated_stems.py --data <folder> [--sources rendered_g3a rendered_newgmd ...] [--batch 150]
                              [--workers 3 --worker 0] [--threads 4] [--until 17:00] [--seed 1]

Run it with a Python that has Demucs and its weights (KitNote's engine: Preferences > AI installs them), NumPy,
SciPy and soundfile. Several workers can run at once (--workers N, each with its own --worker 0..N-1): they share
out the performances and each claims folders of its own.

Reads <data>/<source>/<source>.csv of each source (render_kits.py's tables: midi_filename, audio_filename -
mono 44.1 kHz - split, kit_name ...). Writes <data>/rendered_sepNN/<n>.wav (the drum stem, mono 44.1 kHz,
16 bit, its peak at 90 %) and <n>.mid (a copy of the performance's MIDI), a batch of performances a folder, and
the table <data>/rendered_sepNN/rendered_sepNN.csv last, when the batch is complete, so that a training that
watches the data folder never reads half a batch. A performance is used once: those in the tables already
written are left out, and each performance keeps the split it had.

The band plays the SoundFont MS Basic (MuseScore, MIT; share/sound) through convert_kits.py's SoundFont player,
with a few synthesizers of its own (basses, pads, a voice-like lead). The beat and the bars are found in the
drum MIDI itself. --debug-dir writes each example's mix, its drums as mixed and the stem, to listen to.
"""
import argparse
import csv
import datetime
import gc
import math
import os
import random
import shutil
import struct
import sys
import time
import zlib

os.environ["CUDA_VISIBLE_DEVICES"] = ""          # (the separation runs on the CPU, whatever else has the GPU)

import numpy as np
import soundfile as sf
from scipy import signal
from scipy.ndimage import maximum_filter1d, minimum_filter1d, uniform_filter1d

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from convert_kits import SoundFont, envelope, timecents  # noqa: E402

RATE = 44100
HERE = os.path.dirname(os.path.abspath(__file__))
SOUNDFONT = os.path.join(HERE, "..", "..", "share", "sound", "MS Basic.sf3")
SOURCES = ("rendered_g3a", "rendered_g3b", "rendered_g3c", "rendered_newgmd")
FIELDS = ("midi_filename", "audio_filename", "split", "duration", "style", "kit_name", "source", "drums_db",
          "band", "lag_ms", "stem_corr")


# --- the drum MIDI ------------------------------------------------------------------------------------------------

def _varlen(data, pos):
    value = 0
    while True:
        b = data[pos]
        pos += 1
        value = (value << 7) | (b & 0x7F)
        if not b & 0x80:
            return value, pos


def read_notes(path):
    """[(seconds, pitch, velocity)] of the note-ons of a standard MIDI file (all tracks, its tempo map)."""
    data = open(path, "rb").read()
    if data[:4] != b"MThd":
        raise ValueError(f"not a MIDI file: {path}")
    size = struct.unpack(">I", data[4:8])[0]
    _, tracks, division = struct.unpack(">HHH", data[8:14])
    pos, events = 8 + size, []
    for _ in range(tracks):
        length = struct.unpack(">I", data[pos + 4:pos + 8])[0]
        p, end, tick, status = pos + 8, pos + 8 + length, 0, 0
        while p < end:
            delta, p = _varlen(data, p)
            tick += delta
            b = data[p]
            if b == 0xFF:
                kind = data[p + 1]
                n, p = _varlen(data, p + 2)
                if kind == 0x51:
                    events.append((tick, 0, int.from_bytes(data[p:p + 3], "big"), 0))
                p += n
            elif b in (0xF0, 0xF7):
                n, p = _varlen(data, p + 1)
                p += n
            else:
                if b & 0x80:
                    status, p = b, p + 1
                count = 1 if status & 0xF0 in (0xC0, 0xD0) else 2
                d = data[p:p + count]
                p += count
                if status & 0xF0 == 0x90 and d[1] > 0:
                    events.append((tick, 1, d[0], d[1]))
        pos = end
    events.sort(key=lambda e: (e[0], e[1]))
    notes, tempo, last_tick, now = [], 500000, 0, 0.0
    for tick, kind, a, b in events:
        now += (tick - last_tick) * tempo / 1e6 / division
        last_tick = tick
        if kind == 0:
            tempo = a
        else:
            notes.append((now, a, b))
    return notes


KICKS = (35, 36)
SNARES = (37, 38, 40)
CRASHES = (49, 52, 55, 57)
# how much a stroke tells of the beat
BEAT_WEIGHT = {35: 1.0, 36: 1.0, 38: 1.0, 40: 1.0, 37: 0.6, 42: 0.5, 22: 0.5, 46: 0.5, 26: 0.5, 44: 0.4, 51: 0.5,
               59: 0.5, 53: 0.4, 49: 0.8, 57: 0.8, 52: 0.7, 55: 0.6}


class Grid:
    """The beats of a performance (seconds, extended to before its start and after its end), how many make a
    bar and which one is a downbeat. Positions are in beats: beat k at position k, between them linearly."""

    def __init__(self, beats, per_bar, first):
        self.beats, self.per_bar, self.first = np.asarray(beats, float), per_bar, first
        self.index = np.arange(len(self.beats), dtype=float)

    def time(self, pos):
        return np.interp(pos, self.index, self.beats)

    def pos(self, t):
        return np.interp(t, self.beats, self.index)

    def bar_starts(self):
        return list(range(self.first % self.per_bar, len(self.beats) - 1, self.per_bar))

    def period(self, pos):
        k = int(min(max(0, pos), len(self.beats) - 2))
        return self.beats[k + 1] - self.beats[k]


def find_grid(notes, duration, per_bar=4, fps=100):
    """The beats a performance implies: a tempo from the autocorrelation of its strokes (kick, snare, hi-hat
    weighed most, tempos near 110 BPM preferred), beats tracked along it (dynamic programming, Ellis 2007), the
    downbeat where the bass drum and crashes are."""
    n = int(duration * fps) + 2
    env = np.zeros(n)
    for t, p, v in notes:
        f = int(round(t * fps))
        if 0 <= f < n:
            env[f] += BEAT_WEIGHT.get(p, 0.3) * (0.3 + v / 127)
    env = np.convolve(env, signal.windows.gaussian(9, 1.5), mode="same")
    if len(notes) >= 8 and env.std() > 0:
        x = env - env.mean()
        size = 1 << (2 * n).bit_length()
        ac = np.fft.irfft(np.abs(np.fft.rfft(x, size)) ** 2, size)[:n]
        lags = np.arange(fps * 60 / 220, fps * 60 / 50, 0.25)

        def at(lag):
            return np.interp(lag, np.arange(len(ac)), ac, right=0.0)

        bpm = fps * 60 / lags
        score = (at(lags) + 0.5 * at(2 * lags) + 0.25 * at(4 * lags)) * np.exp(-0.5 * (np.log2(bpm / 110) / 0.9) ** 2)
        period = float(lags[int(np.argmax(score))])
        # the beats: strokes on them, steps between them near the period
        norm = env / (env.std() + 1e-9)
        best = norm.copy()
        back = -np.ones(n, int)
        for t in range(int(round(period / 2)), n):
            lo, hi = max(0, t - int(round(2 * period))), t - int(round(period / 2))
            prev = np.arange(lo, hi + 1)
            cand = best[prev] - 100 * np.log((t - prev) / period) ** 2
            k = int(np.argmax(cand))
            if cand[k] > 0:
                best[t], back[t] = norm[t] + cand[k], prev[k]
        tail = n - int(period) - 1
        t = tail + int(np.argmax(best[tail:])) if tail > 0 else int(np.argmax(best))
        frames = [t]
        while back[frames[-1]] >= 0:
            frames.append(back[frames[-1]])
        beats = np.array(frames[::-1], float) / fps
        if len(beats) < 4:
            beats = np.arange(notes[0][0], duration, period / fps)
    else:
        start = notes[0][0] if notes else 0.0
        beats = np.arange(start, duration + 0.5, 0.5)
    # extended at both ends, at the tempo there
    head = beats[1] - beats[0] if len(beats) > 1 else 0.5
    tail = beats[-1] - beats[-2] if len(beats) > 1 else 0.5
    before = beats[0] - head * np.arange(int((beats[0] + 2 * head) / head) + 1, 0, -1)
    after = beats[-1] + tail * np.arange(1, int((duration + 4 * tail - beats[-1]) / tail) + 2)
    beats = np.concatenate([before, beats, after])
    # the downbeat: the bass drum and crashes on it, the snare not
    weight = np.zeros(len(beats))
    for t, p, v in notes:
        k = int(np.argmin(np.abs(beats - t)))
        if abs(beats[k] - t) < 0.07:
            weight[k] += 1.0 if p in KICKS else 2.0 if p in CRASHES else -0.7 if p in SNARES else 0.0
    phase = int(np.argmax([weight[k::per_bar].sum() for k in range(per_bar)]))
    return Grid(beats, per_bar, phase)


def beats_per_bar(row):
    meter = (row.get("time_signature") or "").replace("/", "-")
    style = row.get("style") or ""
    if meter.startswith("3-") or style == "waltz":
        return 3
    if meter.startswith("5-") or style == "five4":
        return 5
    if meter == "6-8" or style == "six8":
        return 2
    return 4


# --- instruments --------------------------------------------------------------------------------------------------

def polyblep_saw(phase, dt):
    """A saw of a phase (in cycles) rising dt a sample, its jumps smoothed (PolyBLEP): little aliasing."""
    t = phase % 1.0
    y = 2 * t - 1
    dt = np.broadcast_to(dt, t.shape)
    m = t < dt
    x = t[m] / dt[m]
    y[m] -= x + x - x * x - 1
    m = t > 1 - dt
    x = (t[m] - 1) / dt[m]
    y[m] -= x * x + x + x + 1
    return y


def lowpass(x, cutoff, order=2):
    sos = signal.butter(order, min(cutoff, RATE * 0.45), fs=RATE, output="sos")
    return signal.sosfilt(sos, x)


# the formants (Hz) of a few vowels, as sung
VOWELS = {"a": (800, 1150, 2900), "e": (400, 1700, 2600), "i": (300, 2200, 3000), "o": (450, 800, 2830),
          "u": (325, 700, 2530)}


class Synth:
    """The notes of the band: SoundFont presets (bank 0) played as a SoundFont synthesizer would - zones,
    pitch, the volume and modulation envelopes and filter, loops, released when the note ends - and a few
    synthesizers of numpy's. Rendered notes are kept (by preset, key, strength band and length) to be played
    again."""

    def __init__(self, path=SOUNDFONT, cache_mb=120):
        self.sf2 = SoundFont(path)
        self.presets = {}
        self.notes, self.size, self.limit = {}, 0, cache_mb * 1e6

    def preset_index(self, number):
        if number not in self.presets:
            index, _ = self.sf2.preset(number, bank=0)
            if index is None:
                raise ValueError(f"no preset 0:{number} in the SoundFont")
            self.presets[number] = index
        return self.presets[number]

    def forget(self):
        """Lets go of rendered notes and decoded samples (between songs, when they take much memory)."""
        if self.size > self.limit:
            self.notes, self.size = {}, 0
        if sum(a[0].nbytes for a in self.sf2.cache.values()) > self.limit:
            self.sf2.cache = {}

    def note(self, instrument, key, velocity, duration):
        """Mono float32 of a note: instrument ("sf", preset) or ("synth", kind, params) - held for duration
        seconds, then released."""
        band = min(7, velocity // 16)
        length = round(duration * 40) / 40 if duration < 1 else round(duration * 10) / 10
        length = max(0.05, length)
        cache_key = (instrument, key, band, length)
        audio = self.notes.get(cache_key)
        if audio is None:
            if instrument[0] == "sf":
                audio = self._soundfont_note(instrument[1], key, band * 16 + 8, length)
            else:
                audio = synth_note(instrument[1], dict(instrument[2]), key, length)
            audio = audio.astype(np.float32)
            self.notes[cache_key] = audio
            self.size += audio.nbytes
        return audio * (velocity / 127.0) ** 2

    def _soundfont_note(self, preset, key, velocity, duration):
        index = self.preset_index(preset)
        sounds = [self._voice(g, s, key, duration) for g, s in self.sf2.voices(index, key)
                  if g[44][0] <= velocity <= g[44][1]]
        sounds = [s for s in sounds if len(s)]
        if not sounds:
            return np.zeros(1, np.float32)
        n = max(len(s) for s in sounds)
        return sum(np.pad(s, (0, n - len(s))) for s in sounds)

    def _voice(self, g, index, key, duration):
        """One zone of a note (convert_kits.render, but released at the note's end)."""
        audio, rate, root_key, corr, ls, le, kind = self.sf2.sample(index)
        start = g.get(0, 0) + 32768 * g.get(4, 0)
        end = len(audio) + g.get(1, 0) + 32768 * g.get(12, 0)
        ls, le = ls + g.get(2, 0) + 32768 * g.get(45, 0) - start, le + g.get(3, 0) + 32768 * g.get(50, 0) - start
        audio = audio[max(0, start):max(start + 1, end)]
        loops = g.get(54, 0) in (1, 3) and 0 <= ls < le <= len(audio) and le - ls > 8
        root = g[58] if g.get(58, -1) >= 0 else root_key
        cents = (key - root) * g.get(56, 100) + g.get(51, 0) * 100 + g.get(52, 0) + corr
        step = 2 ** (cents / 1200) * rate / RATE
        release = min(1.5, timecents(g.get(38, -12000)))
        t = np.arange(int((duration + release + 0.01) * RATE)) / RATE
        vol = envelope(g, 33, key, t, duration)
        mod = envelope(g, 25, key, t, duration)
        rates = step * 2 ** (mod * g.get(7, 0) / 1200)
        pos = np.concatenate([[0.0], np.cumsum(rates[:-1])])
        if loops:
            over = pos >= le
            pos[over] = ls + (pos[over] - ls) % (le - ls)
        else:
            keep = pos < len(audio) - 1
            pos, vol, mod, t = pos[keep], vol[keep], mod[keep], t[keep]
        out = np.interp(pos, np.arange(len(audio)), audio).astype(np.float32)
        amp = np.where(t < timecents(g.get(33, -12000)) + timecents(g.get(34, -12000)), vol, 10 ** (-96 * (1 - vol) / 20))
        amp[vol <= 0] = 0
        out *= amp * 10 ** (-0.4 * max(0, g.get(48, 0)) / 200)
        cutoff = g.get(8, 13500) + mod * g.get(11, 0)
        if len(out) and (cutoff < 13400).any():
            q = max(0.5, 10 ** ((g.get(9, 0) / 10 - 3.01) / 20))
            block = 256 if g.get(11, 0) else len(out)
            filtered, state = np.zeros_like(out), np.zeros(2)
            for b in range(0, len(out), block):
                fc = min(8.176 * 2 ** (cutoff[b] / 1200), RATE * 0.45)
                w = 2 * np.pi * fc / RATE
                alpha = np.sin(w) / (2 * q)
                bq = np.array([(1 - np.cos(w)) / 2, 1 - np.cos(w), (1 - np.cos(w)) / 2])
                aq = np.array([1 + alpha, -2 * np.cos(w), 1 - alpha])
                filtered[b:b + block], state = signal.lfilter(bq / aq[0], aq / aq[0], out[b:b + block], zi=state)
            out = filtered.astype(np.float32)
        return out * (0.5 if kind & 0x6 else 1.0)


def synth_note(kind, p, key, duration):
    """A note of one of the synthesizers: bass (saw or square through a closing filter), sub, pad (detuned
    saws, slow), lead (saw or square), voice (a buzz with vibrato through the formants of a vowel)."""
    f = 440.0 * 2 ** ((key - 69) / 12)
    release = {"pad": 0.6, "voice": 0.15}.get(kind, 0.08)
    n = int((duration + release) * RATE)
    t = np.arange(n) / RATE
    attack = {"pad": p.get("attack", 0.4), "voice": 0.04, "lead": 0.01}.get(kind, 0.004)
    amp = np.minimum(1.0, t / attack) * np.where(t > duration, np.exp(-(t - duration) / (release / 4)), 1.0)
    if kind in ("bass", "sub"):
        amp *= 0.55 + 0.45 * np.exp(-t / p.get("decay", 0.3))
    if kind == "sub":
        y = np.sin(2 * np.pi * f * t) + 0.15 * polyblep_saw(f * t, f / RATE)
        return y * amp
    if kind == "voice":
        vibrato = 1 + 0.012 * np.sin(2 * np.pi * p.get("rate", 5.5) * t) * np.minimum(1, t / 0.35)
        freq = f * vibrato
        phase = np.cumsum(freq) / RATE
        buzz = polyblep_saw(phase, freq / RATE) + 0.05 * np.random.default_rng(key).standard_normal(n)
        y = np.zeros(n)
        vowel = VOWELS["aeiou"[(key + int(p.get("vowel", 0))) % 5]]
        for formant, gain in zip(vowel, (1.0, 0.5, 0.25)):
            b, a = signal.iirpeak(formant, formant / 80, fs=RATE)
            y += gain * signal.lfilter(b, a, buzz)
        return y * amp
    if kind == "pad":
        y = np.zeros(n)
        for d in (-p.get("detune", 12), -p.get("detune", 12) / 3, 0, p.get("detune", 12) / 3, p.get("detune", 12)):
            fd = f * 2 ** (d / 1200)
            y += polyblep_saw(fd * t + random.random(), fd / RATE)
        return lowpass(y, p.get("cutoff", 2000)) * amp
    # bass and lead: saw or square
    wave = polyblep_saw(f * t, f / RATE)
    if p.get("square"):
        wave = wave - polyblep_saw(f * t + 0.5, f / RATE)
    if kind == "bass":
        bright, dark = lowpass(wave, p.get("cutoff", 1500)), lowpass(wave, p.get("cutoff", 1500) / 5)
        open_ = np.exp(-t / p.get("sweep", 0.12))
        return (bright * open_ + dark * (1 - open_)) * amp
    return lowpass(wave, p.get("cutoff", 4000)) * amp


# --- the song -----------------------------------------------------------------------------------------------------

MODES = {"major": (0, 2, 4, 5, 7, 9, 11), "minor": (0, 2, 3, 5, 7, 8, 10), "dorian": (0, 2, 3, 5, 7, 9, 10),
         "mixolydian": (0, 2, 4, 5, 7, 9, 10)}
# progressions, as degrees of the scale; a chord a bar (or two, or half)
PROGRESSIONS = {
    "major": [(0, 4, 5, 3), (0, 5, 3, 4), (5, 3, 0, 4), (0, 3, 4, 4), (0, 3, 0, 4), (0, 0, 3, 3), (3, 0, 4, 5),
              (0, 4, 3, 4), (0, 1, 4, 0), (0, 3), (0, 4)],
    "minor": [(0, 5, 2, 6), (0, 6, 5, 6), (0, 3, 4, 0), (0, 3, 6, 2), (0, 5, 3, 4), (0, 6), (0, 3), (5, 6, 0, 0)],
    "dorian": [(0, 3), (0, 6, 3, 0), (0, 1)],
    "mixolydian": [(0, 6, 3, 0), (0, 6), (0, 0, 0, 0, 3, 3, 0, 0, 4, 3, 0, 4)],      # (the last: a 12-bar blues)
}
BASSES = [32, 33, 33, 34, 34, 35, 36, 37, 38, 39, 87]
KEYS = [0, 1, 2, 3, 4, 5, 7, 16, 17, 18, 19, 21]
GUITARS = [24, 25, 26, 27, 27, 28]
DRIVEN = [29, 30, 30, 84]
OTHER_CHORDS = [48, 49, 50, 61, 62, 81, 90, 11, 6]
LEADS = [80, 81, 85, 87, 56, 64, 65, 66, 73, 71, 30, 29, 53, 54, 52, 22, 40]
PADS = [88, 89, 90, 91, 94, 95, 48, 49, 50, 51, 52, 53, 19, 16]


class Song:
    """The harmony of a song on a performance's grid: a key, a mode, a progression; which part plays when."""

    def __init__(self, rng, grid, start, end):
        self.rng, self.grid = rng, grid
        self.mode = rng.choice(list(MODES))
        self.scale = MODES[self.mode]
        self.tonic = rng.randrange(12)
        self.progression = rng.choice(PROGRESSIONS[self.mode])
        bar = grid.per_bar
        self.chord_beats = rng.choice([bar, bar, bar, 2 * bar] + ([bar / 2] if bar % 2 == 0 else []))
        self.sevenths = rng.random() < 0.3
        self.start, self.end = start, end            # (seconds: the band plays from the first drum stroke to the last)
        self.first_pos = math.floor(grid.pos(start))
        self.last_pos = math.ceil(grid.pos(end))
        # sections of 4 bars: which part plays in each
        self.section_beats = 4 * bar

    def section(self, pos):
        return int(math.floor((pos - self.grid.first) / self.section_beats))

    def chord(self, pos):
        """(the pitch classes of the chord at a position, its root first)."""
        k = int(math.floor((pos - self.grid.first) / self.chord_beats)) % len(self.progression)
        degree = self.progression[k]
        steps = (0, 2, 4, 6) if self.sevenths else (0, 2, 4)
        return [(self.tonic + self.scale[(degree + s) % 7]) % 12 for s in steps]

    def chord_changes(self):
        """Positions where a chord starts, from before the first stroke to after the last."""
        p = self.grid.first + self.chord_beats * math.floor((self.first_pos - self.grid.first) / self.chord_beats)
        out = []
        while p < self.last_pos:
            out.append(p)
            p += self.chord_beats
        return out

    def scale_pitches(self, low, high):
        return [k for k in range(low, high + 1) if (k - self.tonic) % 12 in self.scale]


def near(pitch_class, low, high, around):
    """The pitch of a pitch class in low..high nearest around."""
    options = [k for k in range(low, high + 1) if k % 12 == pitch_class]
    return min(options, key=lambda k: abs(k - around)) if options else low


def voicing(chord, low, high, rng):
    """A chord's pitches in a range: closed, from an inversion."""
    pcs = chord[:]
    turn = rng.randrange(len(pcs))
    pcs = pcs[turn:] + pcs[:turn]
    out, last = [], low - 1
    for pc in pcs:
        k = last + 1 + (pc - (last + 1)) % 12
        out.append(k)
        last = k
    while out[-1] > high and out[0] - 12 >= low - 6:
        out = [k - 12 for k in out]
    return out


def bass_part(song, notes, rng):
    """The bass: with the bass drum (most often), in eighths, on the downbeats and the bass drum, or walking."""
    grid = song.grid
    kicks = sorted(t for t, p, v in notes if p in KICKS)
    style = rng.choices(["kick", "eighths", "downbeat", "walking"], [0.55, 0.2, 0.15, 0.1])[0]
    low = rng.choice([28, 28, 31, 33])
    events = []                                     # (seconds, pitch, velocity)
    root_of = lambda pos: near(song.chord(pos)[0], low, low + 11, low + 5)
    if style in ("kick", "downbeat"):
        times = [t for k, t in enumerate(kicks) if k == 0 or t - kicks[k - 1] > 0.07]
        if style == "downbeat":
            times = sorted(set(times) | {float(grid.time(b)) for b in grid.bar_starts()})
        else:
            # a gap of more than a beat and a half: notes on the beats in it now and then
            extra = []
            for a, b in zip(times, times[1:] + [song.end]):
                pa, pb = grid.pos(a), grid.pos(b)
                for q in range(math.ceil(pa + 0.75), math.floor(pb - 0.5) + 1):
                    if rng.random() < 0.5:
                        extra.append(float(grid.time(q)))
            times = sorted(times + extra)
        times = [t for t in times if song.start - 0.05 <= t <= song.end]
        for t in times:
            pos = grid.pos(t)
            pitch = root_of(pos + 0.05)
            if rng.random() < 0.12:
                pitch += rng.choice([7, 12, -5 if pitch - 5 >= low else 7])
            events.append((t + rng.gauss(0, 0.006), pitch, rng.randint(80, 120)))
    else:
        step = 0.5 if style == "eighths" else 1.0
        pos = song.first_pos
        while pos < song.last_pos:
            chord = song.chord(pos + 0.01)
            pitch = root_of(pos + 0.01)
            if style == "walking" and (pos - grid.first) % 1 == 0:
                beat = int(round(pos - grid.first)) % grid.per_bar
                if beat:
                    pitch = near(chord[rng.choice([0, 1, 2])], low, low + 14, pitch + rng.choice([-3, 2, 4]))
                    nxt = song.chord(pos + 1.01)[0]
                    if beat == grid.per_bar - 1 and rng.random() < 0.5:
                        pitch = near(nxt, low, low + 14, pitch) + rng.choice([-1, 1])
            t = float(grid.time(pos))
            if song.start - 0.05 <= t <= song.end:
                accent = 1.0 if (pos - grid.first) % 1 == 0 else 0.85
                events.append((t + rng.gauss(0, 0.006), pitch, int(rng.randint(85, 118) * accent)))
            pos += step
    legato = rng.uniform(0.5, 0.97)
    out = []
    for k, (t, pitch, vel) in enumerate(events):
        nxt = events[k + 1][0] if k + 1 < len(events) else t + grid.period(grid.pos(t))
        dur = max(0.06, min(nxt - t, 2 * grid.period(grid.pos(t))) * legato)
        out.append((t, dur, pitch, vel))
    return out, style


def chord_part(song, notes, rng, instrument_kind):
    """Chords: held, on the beats, in eighths, off the beat (a skank), with the drums' accents, or broken up."""
    grid = song.grid
    if instrument_kind == "driven":
        styles, weights = ["eighths", "held", "accents", "quarters"], [0.4, 0.25, 0.2, 0.15]
    elif instrument_kind == "guitar":
        styles, weights = ["quarters", "eighths", "offbeat", "accents", "arpeggio", "held"], [0.2, 0.2, 0.15, 0.15, 0.15, 0.15]
    elif instrument_kind == "pad":
        styles, weights = ["held"], [1]
    else:
        styles, weights = ["held", "quarters", "offbeat", "accents", "arpeggio", "eighths"], [0.3, 0.2, 0.1, 0.15, 0.15, 0.1]
    style = rng.choices(styles, weights)[0]
    low = {"driven": 40, "pad": 50}.get(instrument_kind, rng.choice([48, 52, 55]))
    high = low + 16
    events = []                                     # (seconds, dur seconds, pitch, velocity)
    strum = rng.uniform(0.004, 0.015) if instrument_kind in ("guitar", "driven") else 0.0
    accents = [t for t, p, v in notes if p in KICKS + SNARES + CRASHES and v > 70]

    def chord_at(t, dur, pos, vel):
        chord = song.chord(pos + 0.01)
        if instrument_kind == "driven":
            root = near(chord[0], low, low + 11, low + 4)
            pitches = [root, root + 7, root + 12] if rng.random() < 0.85 else voicing(chord, low + 12, high + 12, rng)
        else:
            pitches = voicing(chord, low, high, rng)
        down = rng.random() < 0.7
        for k, pitch in enumerate(pitches if down else pitches[::-1]):
            events.append((t + k * strum, max(0.05, dur - k * strum), pitch, int(vel * rng.uniform(0.85, 1.0))))

    if style == "held":
        changes = song.chord_changes()
        for k, pos in enumerate(changes):
            t = float(grid.time(pos))
            end = float(grid.time(changes[k + 1])) if k + 1 < len(changes) else song.end + 1.0
            if t < song.end:
                chord_at(max(t, song.start - 0.02), end - max(t, song.start), pos, rng.randint(70, 105))
    elif style == "accents":
        last = -1.0
        for t in accents:
            if t - last > 0.2 and song.start <= t <= song.end:
                chord_at(t, rng.uniform(0.15, 0.6), grid.pos(t), rng.randint(80, 115))
                last = t
    elif style == "arpeggio":
        step = rng.choice([0.5, 0.25]) if grid.period(0) > 0.4 else 0.5
        pos, k = song.first_pos, 0
        while pos < song.last_pos:
            chord = song.chord(pos + 0.01)
            pitches = voicing(chord, low, high, rng) if k % 8 == 0 or (pos - grid.first) % song.chord_beats == 0 else pitches
            t = float(grid.time(pos))
            if song.start - 0.02 <= t <= song.end:
                pitch = pitches[k % len(pitches)] + (12 if (k // len(pitches)) % 2 and rng.random() < 0.5 else 0)
                events.append((t, float(grid.time(pos + step * 1.5)) - t, pitch, rng.randint(65, 100)))
            pos += step
            k += 1
    else:
        step, offset, length = {"quarters": (1, 0, 0.8), "eighths": (0.5, 0, 0.45), "offbeat": (1, 0.5, 0.3)}[style]
        pos = song.first_pos + offset
        while pos < song.last_pos:
            t = float(grid.time(pos))
            if song.start - 0.02 <= t <= song.end:
                accent = 1.0 if (pos - grid.first) % grid.per_bar == 0 else 0.88
                dur = float(grid.time(pos + step * length)) - t
                chord_at(t + rng.gauss(0, 0.004), dur, pos, int(rng.randint(75, 112) * accent))
            pos += step
    return events, style


def lead_part(song, rng, low, high):
    """A tune: phrases of one or two bars with rests between, in eighths and longer, moving by step mostly and
    landing on the chord on the beat."""
    grid = song.grid
    pitches = song.scale_pitches(low, high)
    k = len(pitches) // 2
    events = []
    bar = grid.per_bar
    for start in grid.bar_starts():
        if start + bar < song.first_pos or start > song.last_pos:
            continue
        if (start - grid.first) // bar % 2 and rng.random() < 0.5:
            continue                                # (a rest a bar, now and then)
        pos = start + rng.choice([0, 0, 0.5, 1])
        stop = start + bar * rng.choice([1, 1, 2])
        while pos < stop:
            dur = rng.choice([0.5, 0.5, 0.5, 1, 1, 1.5, 2])
            if rng.random() < 0.15:
                pos += dur
                continue
            k = max(0, min(len(pitches) - 1, k + rng.choice([-2, -1, -1, 0, 1, 1, 2, rng.choice([-4, 3, 4])])))
            if (pos - grid.first) % 1 == 0:
                chord = song.chord(pos + 0.01)
                tones = [j for j, q in enumerate(pitches) if q % 12 in chord]
                if tones:
                    k = min(tones, key=lambda j: abs(j - k))
            t = float(grid.time(pos))
            if song.start <= t <= song.end:
                events.append((t + rng.gauss(0, 0.008), float(grid.time(pos + dur)) - t, pitches[k], rng.randint(70, 115)))
            pos += dur
    return events


def render_part(synth, instrument, events, length):
    out = np.zeros(length, np.float32)
    for t, dur, pitch, vel in events:
        s = int(round(t * RATE))
        if s >= length:
            continue
        audio = synth.note(instrument, pitch, vel, dur)
        if s < 0:
            audio, s = audio[-s:], 0
        e = min(length, s + len(audio))
        out[s:e] += audio[:e - s]
    return out


def make_band(song, notes, synth, rng, length):
    """The parts of the band: [(name, description, mono audio, gain dB, pan -1..1, width s)]."""
    parts = []

    def sf_or_synth(presets, synth_kind, synth_chance, params):
        if rng.random() < synth_chance:
            return ("synth", synth_kind, tuple(sorted(params.items()))), synth_kind
        preset = rng.choice(presets)
        return ("sf", preset), str(preset)

    # bass
    if rng.random() < 0.95:
        events, style = bass_part(song, notes, rng)
        kind = rng.choice(["bass", "bass", "sub"])
        params = {"cutoff": rng.uniform(600, 3000), "square": rng.random() < 0.3, "decay": rng.uniform(0.15, 0.8),
                  "sweep": rng.uniform(0.04, 0.3)}
        instrument, label = sf_or_synth(BASSES, kind, 0.25, params)
        parts.append(("bass", f"bass {label} {style}", render_part(synth, instrument, events, length), 0.0,
                      rng.uniform(-0.1, 0.1), 0.0))
    # chords, one or two instruments
    for n in range(2):
        if rng.random() > (0.9 if n == 0 else 0.35):
            continue
        family = rng.choices(["keys", "guitar", "driven", "other"], [0.35, 0.25, 0.25, 0.15])[0]
        preset = rng.choice({"keys": KEYS, "guitar": GUITARS, "driven": DRIVEN, "other": OTHER_CHORDS}[family])
        events, style = chord_part(song, notes, rng, family)
        width = rng.uniform(0.008, 0.025) if family in ("guitar", "driven") and rng.random() < 0.5 else 0.0
        parts.append(("chords", f"chords {preset} {style}", render_part(synth, ("sf", preset), events, length),
                      rng.uniform(-6, 0) - 3 * n, rng.uniform(-0.7, 0.7), width))
    # a lead or a voice
    if rng.random() < 0.5:
        kind = rng.choice(["sf", "sf", "voice", "lead"])
        if kind == "voice":
            instrument, label = ("synth", "voice", (("rate", rng.uniform(4.5, 6.5)), ("vowel", rng.randrange(5)))), "voice"
            low = rng.choice([55, 60])
        elif kind == "lead":
            instrument, label = ("synth", "lead", (("cutoff", rng.uniform(1500, 6000)), ("square", rng.random() < 0.5))), "lead"
            low = 60
        else:
            preset = rng.choice(LEADS)
            instrument, label, low = ("sf", preset), str(preset), 58 if preset not in (30, 29, 22, 66) else 52
        events = lead_part(song, rng, low, low + 19)
        parts.append(("lead", f"lead {label}", render_part(synth, instrument, events, length), rng.uniform(-4, 1),
                      rng.uniform(-0.25, 0.25), 0.0))
    # a pad (always, when no chords)
    if rng.random() < 0.4 or not any(p[0] == "chords" for p in parts):
        params = {"attack": rng.uniform(0.1, 0.8), "detune": rng.uniform(5, 20), "cutoff": rng.uniform(800, 4000)}
        instrument, label = sf_or_synth(PADS, "pad", 0.3, params)
        events, _ = chord_part(song, notes, rng, "pad")
        audio = render_part(synth, instrument, events, length)
        if instrument[0] == "sf":                   # (a pad swells in)
            fade = np.ones(length, np.float32)
            for t, dur, _, _ in events:
                s = min(length, max(0, int(t * RATE)))
                k = min(length - s, int(params["attack"] * RATE))
                if k > 0:
                    fade[s:s + k] = np.minimum(fade[s:s + k], np.linspace(0.2, 1, k))
            audio *= fade
        parts.append(("pad", f"pad {label}", audio, rng.uniform(-12, -4), 0.0, rng.uniform(0.01, 0.03)))
    return parts


# --- mixing -------------------------------------------------------------------------------------------------------

def rms(x):
    return float(np.sqrt(np.mean(np.square(x, dtype=np.float64)))) + 1e-12


def stereo(mono, pan, width, rng):
    """A mono part placed: panned (constant power), or spread (the other side a few ms later, as a part played
    twice)."""
    angle = (pan + 1) * np.pi / 4
    left, right = mono * np.cos(angle), mono * np.sin(angle)
    if width:
        d = int(width * RATE)
        late = np.concatenate([np.zeros(d, np.float32), mono[:-d]]) * rng.uniform(0.7, 1.0)
        left, right = (mono, late) if rng.random() < 0.5 else (late, mono)
        left, right = left * 0.75, right * 0.75
    return np.stack([left, right]).astype(np.float32)


def sections_mask(song, length, chance, rng):
    """A part playing in some of the song's 4-bar sections, not all: 0..1 over the samples, with short fades."""
    grid = song.grid
    mask = np.ones(length, np.float32)
    first, last = song.section(song.first_pos), song.section(song.last_pos)
    for s in range(first, last + 1):
        if rng.random() > chance:
            a = int(grid.time(grid.first + s * song.section_beats) * RATE)
            b = int(grid.time(grid.first + (s + 1) * song.section_beats) * RATE)
            mask[max(0, a):max(0, min(length, b))] = 0
    return uniform_filter1d(mask, int(0.03 * RATE))


def reverb(x, rng, seconds, damping):
    """A room: noise dying away in seconds (two channels, not alike), darker as it goes, after a predelay."""
    n = int(seconds * RATE)
    t = np.arange(n) / RATE
    ir = rng.standard_normal((2, n)) * np.exp(-6.9 * t / seconds)
    ir = np.stack([lowpass(c, damping) for c in ir])
    ir = np.concatenate([np.zeros((2, int(rng.uniform(0.0, 0.03) * RATE))), ir], axis=1)
    ir /= np.sqrt((ir ** 2).sum(axis=1, keepdims=True))
    return np.stack([signal.oaconvolve(x[c], ir[c])[:x.shape[1]] for c in range(2)]).astype(np.float32)


def compress(x, threshold_db, ratio, attack, release, block=64):
    """The gain of a bus compressor over a mix: the louder channel's peaks, block by block, followed with an
    attack and a release."""
    n = x.shape[1] // block * block
    peaks = np.abs(x[:, :n]).max(axis=0).reshape(-1, block).max(axis=1)
    db = 20 * np.log10(peaks + 1e-9)
    a, r = math.exp(-block / (attack * RATE)), math.exp(-block / (release * RATE))
    env, level = np.empty_like(db), -120.0
    for k, v in enumerate(db):
        c = a if v > level else r
        level = c * level + (1 - c) * v
        env[k] = level
    gain_db = np.minimum(0.0, (threshold_db - env) * (1 - 1 / ratio))
    centres = np.arange(len(gain_db)) * block + block / 2
    return 10 ** (np.interp(np.arange(x.shape[1]), centres, gain_db) / 20)


def limit(x, ceiling=0.98, lookahead=0.003):
    """The gain of a brickwall limiter over a mix: under the peaks to come, smoothed."""
    k = 2 * int(lookahead * RATE) + 1
    peak = maximum_filter1d(np.abs(x).max(axis=0), k)
    gain = np.minimum(1.0, ceiling / (peak + 1e-9))
    return uniform_filter1d(minimum_filter1d(gain, k), k)


def mix(drums, parts, song, rng, nrng):
    """Drums and band as a song: the band's balance, the drums' level against it, a room, a master bus.
    -> (stereo mix, the drums as they are in it (stereo), what was done)."""
    n = len(drums)
    band = np.zeros((2, n), np.float32)
    for name, label, audio, gain_db, pan, width in parts:
        if not audio.any():
            continue
        audio = audio / rms(audio) * 10 ** (gain_db / 20)
        if name != "bass" or rng.random() < 0.3:
            audio = audio * sections_mask(song, n, {"lead": 0.6, "pad": 0.7}.get(name, 0.9), rng)
        band += stereo(audio, pan, width, rng)
    drum_mix = np.stack([drums, drums]).astype(np.float32)
    notes = []
    drums_db = rng.uniform(-12, 6)
    if band.any():
        band *= rms(drums) / rms(band) * 10 ** (-drums_db / 20)
    else:
        drums_db = float("inf")
    if rng.random() < 0.5:
        seconds, damping = rng.uniform(0.3, 2.5), rng.uniform(2000, 9000)
        room = reverb(band + drum_mix * rng.uniform(0.0, 0.6), nrng, seconds, damping)
        wet = rng.uniform(0.05, 0.35)
        room *= rms(band + drum_mix) / rms(room) * wet
        band += room
        notes.append(f"room {seconds:.1f}s {wet:.2f}")
    out = band + drum_mix
    if rng.random() < 0.3:                          # (the low end cleaned, the top softened)
        sos = signal.butter(2, rng.uniform(25, 80), "highpass", fs=RATE, output="sos")
        out = signal.sosfilt(sos, out, axis=1).astype(np.float32)
        drum_mix = signal.sosfilt(sos, drum_mix, axis=1).astype(np.float32)
        notes.append("eq")
    # the master bus: a compressor, a limiter driven into; its gains on the drums too, to know them in the mix
    gain = 1 / (np.abs(out).max() + 1e-9)
    if rng.random() < 0.5:
        threshold, ratio = rng.uniform(-24, -8), rng.uniform(1.5, 6)
        gain = gain * compress(out * gain, threshold, ratio, rng.uniform(0.003, 0.03), rng.uniform(0.05, 0.4))
        notes.append(f"comp {threshold:.0f}dB {ratio:.1f}:1")
    if rng.random() < 0.6:
        drive = rng.uniform(2, 10)
        gain = gain / (np.abs(out * gain).max() + 1e-9) * 10 ** (drive / 20)
        gain = gain * limit(out * gain)
        notes.append(f"limit +{drive:.0f}dB")
    out, drum_mix = out * gain, drum_mix * gain
    level = rng.uniform(0.5, 0.98) / (np.abs(out).max() + 1e-9)
    return (out * level).astype(np.float32), (drum_mix * level).astype(np.float32), drums_db, notes


# --- separating ---------------------------------------------------------------------------------------------------

class Separator:
    """The drums of a mix by Demucs, as share/ai/kitnote_audio.py separate_drums does - the mix normalized,
    apply_model with split=True and overlap 0.25 - but on the CPU, on a number of threads."""

    def __init__(self, threads, model_name="htdemucs"):
        if "TORCH_HOME" not in os.environ:
            engine = os.path.join(os.environ.get("LOCALAPPDATA", ""), "KitNote", "ai", "torch")
            if os.path.isdir(engine):
                os.environ["TORCH_HOME"] = engine
        import torch
        from demucs.apply import apply_model
        from demucs.pretrained import get_model
        torch.set_num_threads(threads)
        torch.set_num_interop_threads(1)
        self.torch, self.apply_model = torch, apply_model
        self.model = get_model(model_name)
        self.model.eval()
        self.drums = list(self.model.sources).index("drums")

    def __call__(self, audio):
        torch = self.torch
        wav = torch.from_numpy(np.ascontiguousarray(audio, dtype=np.float32))
        ref = wav.mean(0)
        mean, std = ref.mean(), ref.std() + 1e-8
        wav = (wav - mean) / std
        with torch.no_grad():
            sources = self.apply_model(self.model, wav[None], device="cpu", split=True, overlap=0.25, progress=False)[0]
        sources = sources * std + mean
        return sources[self.drums].numpy()


def alignment(stem, clean, max_lag=0.05):
    """(lag of the stem behind the clean drums in ms, their correlation there), above 150 Hz, over at most
    30 s of the loudest part."""
    sos = signal.butter(2, 150, "highpass", fs=RATE, output="sos")
    a, b = signal.sosfilt(sos, stem), signal.sosfilt(sos, clean)
    span = min(len(a), 30 * RATE)
    k = max(0, int(np.argmax(uniform_filter1d(np.abs(b), RATE))) - span // 2)
    k = min(k, len(a) - span)
    a, b = a[k:k + span], b[k:k + span]
    size = 1 << (2 * span).bit_length()
    xc = np.fft.irfft(np.fft.rfft(a, size) * np.conj(np.fft.rfft(b, size)), size)
    m = int(max_lag * RATE)
    lags = np.concatenate([xc[-m:], xc[:m + 1]])
    best = int(np.argmax(lags))
    corr = lags[best] / (np.linalg.norm(a) * np.linalg.norm(b) + 1e-12)
    return (best - m) / RATE * 1000, float(corr)


# --- the batches --------------------------------------------------------------------------------------------------

def source_rows(data, sources, min_seconds, max_seconds=1e9):
    rows = []
    for source in sources:
        with open(os.path.join(data, source, source + ".csv"), newline="", encoding="utf-8") as f:
            for row in csv.DictReader(f):
                if row.get("audio_filename") and row.get("split") and min_seconds <= float(row.get("duration") or 0) <= max_seconds:
                    rows.append((source, row))
    return rows


def used_sources(data, prefix):
    """The performances the batches written already used (their source column)."""
    used = set()
    for folder in os.listdir(data):
        table = os.path.join(data, folder, folder + ".csv")
        if folder.startswith(prefix) and os.path.exists(table):
            with open(table, newline="", encoding="utf-8") as f:
                used.update(r["source"] for r in csv.DictReader(f))
    return used


def claim_batch(data, prefix):
    """A new batch folder of one's own: the first <prefix>NN that does not exist (made atomically)."""
    n = 0
    while True:
        folder = f"{prefix}{n:02d}"
        try:
            os.mkdir(os.path.join(data, folder))
            return folder
        except FileExistsError:
            n += 1


def make_example(source_dir, row, synth, separator, rng, nrng, debug=None):
    """(the drum stem, mono float32 peak 0.9, its row's extra columns) of one performance, or None."""
    drums, rate = sf.read(os.path.join(source_dir, row["audio_filename"]), dtype="float32", always_2d=True)
    drums = drums.mean(axis=1)
    if rate != RATE:
        raise ValueError(f"{row['audio_filename']}: {rate} Hz")
    notes = read_notes(os.path.join(source_dir, row["midi_filename"]))
    if len(notes) < 4:
        return None
    duration = len(drums) / RATE
    grid = find_grid(notes, duration, beats_per_bar(row))
    song = Song(rng, grid, max(0.0, notes[0][0] - rng.uniform(0, 0.3)), min(duration, notes[-1][0] + 0.3))
    parts = make_band(song, notes, synth, rng, len(drums))
    synth.forget()
    mixed, drums_mixed, drums_db, processing = mix(drums, parts, song, rng, nrng)
    stem = separator(mixed).mean(axis=0)
    clean = drums_mixed.mean(axis=0)
    lag, corr = alignment(stem, clean)
    if debug:
        base = os.path.join(debug, os.path.splitext(row["audio_filename"].replace("/", "_"))[0])
        sf.write(base + "_mix.wav", mixed.T, RATE, subtype="FLOAT")
        sf.write(base + "_drums.wav", clean, RATE, subtype="FLOAT")
        sf.write(base + "_stem.wav", stem, RATE, subtype="FLOAT")
        sf.write(base + "_band.wav", (mixed - drums_mixed).T, RATE, subtype="FLOAT")
    peak = float(np.abs(stem).max()) or 1.0
    band = "; ".join([p[1] for p in parts] + processing)
    return stem / peak * 0.9, {"drums_db": f"{drums_db:.1f}", "band": band, "lag_ms": f"{lag:.2f}",
                               "stem_corr": f"{corr:.3f}", "beats": f"{60 / np.median(np.diff(grid.beats)):.0f}bpm"}


def deadline(text):
    if not text:
        return None
    hour, minute = (int(x) for x in text.split(":"))
    now = datetime.datetime.now()
    when = now.replace(hour=hour, minute=minute, second=0, microsecond=0)
    if when <= now:
        when += datetime.timedelta(days=1)
    return when.timestamp()


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("--data", required=True, help="the data folder with the rendered* sources; batches go there")
    parser.add_argument("--sources", nargs="+", default=list(SOURCES), help="folders of <data> with render_kits.py tables")
    parser.add_argument("--prefix", default="rendered_sep", help="the batch folders: <prefix>NN")
    parser.add_argument("--batch", type=int, default=150, help="performances a batch")
    parser.add_argument("--batches", type=int, default=0, help="stop after this many batches (0: no limit)")
    parser.add_argument("--workers", type=int, default=1)
    parser.add_argument("--worker", type=int, default=0)
    parser.add_argument("--threads", type=int, default=4, help="CPU threads for Demucs")
    parser.add_argument("--until", help="HH:MM: stop at a batch's end before this time (the last batch then smaller)")
    parser.add_argument("--stop-file", help="stop the same way when this file appears")
    parser.add_argument("--min-seconds", type=float, default=4.0, help="leave out shorter performances")
    parser.add_argument("--max-seconds", type=float, default=150.0, help="leave out longer performances (memory)")
    parser.add_argument("--max-lag-ms", type=float, default=5.0, help="leave out stems further from their labels")
    parser.add_argument("--min-corr", type=float, default=0.2, help="leave out stems this unlike the drums")
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--debug-dir", help="also write each example's mix, drums, band and stem here")
    parser.add_argument("--soundfont", default=SOUNDFONT)
    args = parser.parse_args()

    stop_at = deadline(args.until)
    rows = source_rows(args.data, args.sources, args.min_seconds, args.max_seconds)
    random.Random(args.seed).shuffle(rows)
    used = used_sources(args.data, args.prefix)
    rows = [r for r in rows[args.worker::args.workers] if f"{r[0]}/{r[1]['audio_filename']}" not in used]
    print(f"worker {args.worker}/{args.workers}: {len(rows)} performances to use", flush=True)
    synth = Synth(args.soundfont)
    separator = Separator(args.threads)
    if args.debug_dir:
        os.makedirs(args.debug_dir, exist_ok=True)

    per_example, batches = None, 0
    queue = iter(rows)
    finished = False
    while not finished:
        folder = claim_batch(args.data, args.prefix)
        out = os.path.join(args.data, folder)
        table, started, audio_seconds = [], time.time(), 0.0
        print(f"{folder}: started", flush=True)
        while len(table) < args.batch:
            margin = 1.5 * (per_example or 60)
            if (stop_at and time.time() + margin > stop_at) or (args.stop_file and os.path.exists(args.stop_file)):
                finished = True
                break
            item = next(queue, None)
            if item is None:
                finished = True
                break
            source, row = item
            key = zlib.crc32(f"{args.seed}/{source}/{row['audio_filename']}".encode())
            rng, nrng = random.Random(key), np.random.default_rng(key)
            random.seed(key)
            begun = time.time()
            try:
                result = make_example(os.path.join(args.data, source), row, synth, separator, rng, nrng, args.debug_dir)
            except Exception as error:                  # (one bad performance does not stop a batch)
                print(f"  {source}/{row['audio_filename']}: {type(error).__name__}: {error}", flush=True)
                continue
            gc.collect()
            if result is None:
                continue
            stem, extra = result
            if abs(float(extra["lag_ms"])) > args.max_lag_ms or float(extra["stem_corr"]) < args.min_corr:
                print(f"  {source}/{row['audio_filename']}: left out (lag {extra['lag_ms']} ms, corr {extra['stem_corr']})", flush=True)
                continue
            n = len(table)
            sf.write(os.path.join(out, f"{n}.wav"), (stem * 32767).astype(np.int16), RATE, subtype="PCM_16")
            shutil.copyfile(os.path.join(args.data, source, row["midi_filename"]), os.path.join(out, f"{n}.mid"))
            seconds = len(stem) / RATE
            audio_seconds += seconds
            table.append({"midi_filename": f"{n}.mid", "audio_filename": f"{n}.wav", "split": row["split"],
                          "duration": f"{seconds:.3f}", "style": row.get("style", ""), "kit_name": row.get("kit_name", ""),
                          "source": f"{source}/{row['audio_filename']}", "drums_db": extra["drums_db"],
                          "band": extra["band"], "lag_ms": extra["lag_ms"], "stem_corr": extra["stem_corr"]})
            took = time.time() - begun
            per_example = took if per_example is None else 0.9 * per_example + 0.1 * took
            print(f"  {folder}/{n}: {seconds:5.1f} s in {took:5.1f} s, drums {extra['drums_db']} dB, "
                  f"lag {extra['lag_ms']} ms, corr {extra['stem_corr']}, {extra['beats']} | {extra['band']}", flush=True)
        if not table:
            os.rmdir(out) if not os.listdir(out) else None
            break
        # the table last, whole, under its name only when complete
        part = os.path.join(out, folder + ".csv.part")
        with open(part, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=FIELDS)
            writer.writeheader()
            writer.writerows(table)
        os.replace(part, os.path.join(out, folder + ".csv"))
        took = time.time() - started
        print(f"{folder}: {len(table)} performances, {audio_seconds / 3600:.2f} h of audio in {took / 60:.1f} min "
              f"({audio_seconds / took:.2f}x realtime)", flush=True)
        batches += 1
        if args.batches and batches >= args.batches:
            break
    print("done", flush=True)


if __name__ == "__main__":
    main()
