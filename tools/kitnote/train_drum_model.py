# SPDX-License-Identifier: GPL-3.0-only
"""Trains KitNote's drum model (share/ai/kitnote_drum_model.py) and writes its weights, kitnote_drums.pt.

Data: the Groove MIDI Dataset v1.0.0 (Gillick, Roberts, Engel, Eck, Bamman: "Learning to Groove with Inverse
Sequence Transformations", ICML 2019; Google Magenta; CC BY 4.0) - drummers playing an electronic kit, its audio
recorded with the MIDI of every stroke:
    https://storage.googleapis.com/magentadata/datasets/groove/groove-v1.0.0.zip
unpacked so that <data>/groove/info.csv is there. The audio is changed while training - level, colour, room,
noise, a little of other instruments - so that the model hears a drum stem separated from a song as well.

    python train_drum_model.py --data <folder> [<folder> ...] --out kitnote_drums.pt [--steps 20000]

Each data folder may also hold a part of the Expanded Groove MIDI Dataset (fetch_egmd_subset.py) and
performances played on sampled kits (render_kits.py, in folders named rendered*).

Needs PyTorch (a GPU helps much), NumPy, SciPy, soundfile and mido.
"""
import argparse
import csv
import math
import os
import random
import sys
import time

import mido
import numpy as np
import soundfile as sf
import torch
import torch.nn.functional as F

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "share", "ai"))
import kitnote_drum_model as km  # noqa: E402

# the pitches of the Roland TD-11 as the dataset has them, by KitNote's classes
PITCHES = {
    "BD": (35, 36),
    "SD": (37, 38, 40),                                  # head, rim, cross-stick
    "TT": (41, 43, 45, 47, 48, 50, 58),                  # toms, heads and rims
    "HH": (22, 26, 42, 44, 46),                          # closed, open (bow and edge), pedal
    "CY": (49, 51, 52, 53, 55, 57, 59),                  # crashes, ride (bow, edge, bell)
}
CLASS_OF = {p: i for i, name in enumerate(km.CLASSES) for p in PITCHES[name]}
CROP = 4 * km.RATE                                       # 4 seconds a training example


def load_song(audio_path, midi_path):
    """(mono float32 at RATE, targets [frames, classes] float32): 1 on the frame of a stroke, 0.5 on its neighbours."""
    audio, rate = sf.read(audio_path, dtype="float32", always_2d=True)
    audio = audio.mean(axis=1)
    audio = audio / max(1e-6, float(np.abs(audio).max()))
    if rate != km.RATE:
        from scipy.signal import resample_poly
        g = math.gcd(rate, km.RATE)
        audio = resample_poly(audio, km.RATE // g, rate // g).astype(np.float32)
    frames = len(audio) // km.HOP + 1
    targets = np.zeros((frames, len(km.CLASSES)), np.float32)
    now = 0.0
    for message in mido.MidiFile(midi_path):            # (iterating a file gives times in seconds)
        now += message.time
        if message.type == "note_on" and message.velocity > 0 and message.note in CLASS_OF:
            f, c = int(round(now * km.FPS)), CLASS_OF[message.note]
            if 0 <= f < frames:
                targets[f, c] = 1.0
                for g in (f - 1, f + 1):
                    if 0 <= g < frames:
                        targets[g, c] = max(targets[g, c], 0.5)
    return (audio * 32767).astype(np.int16), targets


def cached_song(audio_path, midi_path, cache):
    """load_song, through a cache of .npy files: the audio memory-mapped from disk (a hundred hours of it do
    not fit in memory), the targets in memory."""
    import hashlib
    key = hashlib.sha1(os.path.abspath(audio_path).encode("utf-8")).hexdigest()[:20]
    wav_npy, tgt_npy = os.path.join(cache, key + ".audio.npy"), os.path.join(cache, key + ".targets.npy")
    if not (os.path.exists(wav_npy) and os.path.exists(tgt_npy)):
        audio, targets = load_song(audio_path, midi_path)
        np.save(wav_npy, audio)
        np.save(tgt_npy, targets)
    return np.load(wav_npy, mmap_mode="r"), np.load(tgt_npy)


# the sources: (table, folder its file names are in) - the Groove MIDI Dataset; the part of E-GMD that
# fetch_egmd_subset.py fetched; the performances render_kits.py played on acoustic sample kits: those of the
# Groove MIDI Dataset, and those random_grooves.py made
SOURCES = (("groove/info.csv", "groove"), ("e-gmd-subset.csv", "e-gmd-v1.0.0"), ("rendered/rendered.csv", "rendered"),
           ("rendered_grooves/rendered_grooves.csv", "rendered_grooves"))


def tables(data):
    """The tables of a data folder: those of SOURCES, and every rendered*/<folder>.csv render_kits.py wrote."""
    found = [(t, f) for t, f in SOURCES if os.path.exists(os.path.join(data, t))]
    for folder in sorted(os.listdir(data)):
        table = f"{folder}/{folder}.csv"
        if folder.startswith("rendered") and (table, folder) not in found and os.path.exists(os.path.join(data, table)):
            found.append((table, folder))
    return found


def load_split(data, split, cache=None, seen=None, boost=None):
    """The performances of a split; with seen (a set of audio files), only those not in it, which it gets.
    boost {folder prefix: n}: the performances of those folders n times, so that they are chosen n times as often."""
    songs = []
    cache = cache or os.path.join(data, "cache")
    os.makedirs(cache, exist_ok=True)
    for table, folder in tables(data):
        path = os.path.join(data, table)
        if not os.path.exists(path):
            continue
        count = 0
        with open(path, newline="", encoding="utf-8") as f:
            for row in csv.DictReader(f):
                if row["split"] != split or not row["audio_filename"]:
                    continue
                audio = os.path.join(data, folder, row["audio_filename"])
                if seen is not None and audio in seen:
                    continue
                if os.path.exists(audio):
                    if seen is not None:
                        seen.add(audio)
                    song = cached_song(audio, os.path.join(data, folder, row["midi_filename"]), cache)
                    times = max([n for prefix, n in (boost or {}).items() if folder.startswith(prefix)] or [1])
                    songs.extend([song] * times)
                    count += 1
        print(f"  {table}: {count} {split}", flush=True)
    return songs


# --- features and changes of the audio, on the GPU --------------------------------------------------------

class Frontend:
    def __init__(self, device):
        self.device = device
        self.window = torch.hann_window(km.N_FFT, periodic=False, device=device)
        self.bank = torch.from_numpy(km.BANK).to(device)
        self.freqs = torch.fft.rfftfreq(km.N_FFT, 1.0 / km.RATE).to(device)

    def magnitude(self, wave):                          # [batch, samples] -> [batch, bins, frames]
        return torch.stft(wave, km.N_FFT, km.HOP, window=self.window, center=True, pad_mode="constant",
                          return_complex=True).abs()

    def features(self, magnitude):                      # -> [batch, frames, bands]
        return torch.log1p(torch.einsum("kf,bft->btk", self.bank, magnitude))


def augment(wave, frontend):
    """The audio of a batch [batch, samples] changed so that it sounds as drums of other kits, rooms and songs,
    and as a drum stem a separation gives; returns the changed magnitude spectrogram."""
    b, n = wave.shape
    dev = wave.device
    # a room: noise dying away, 0.1 to 1.2 seconds, mixed in up to 40 %
    if random.random() < 0.6:
        length = int(km.RATE * random.uniform(0.1, 1.2))
        t = torch.arange(length, device=dev) / km.RATE
        ir = torch.randn(b, length, device=dev) * torch.exp(-6.9 * t / random.uniform(0.1, 1.2))
        ir = ir / ir.norm(dim=1, keepdim=True)
        # (one size of FFT for every room: a new size each time makes the GPU plan a new FFT each time)
        size = 1 << (n + int(km.RATE * 1.2)).bit_length()
        wet = torch.fft.irfft(torch.fft.rfft(wave, size) * torch.fft.rfft(ir, size), size)[:, :n]
        mix = torch.rand(b, 1, device=dev) * 0.4
        wave = (1 - mix) * wave + mix * wet * (wave.norm(dim=1, keepdim=True) / (wet.norm(dim=1, keepdim=True) + 1e-8))
    # drive: a little saturation, as a mix bus or a loud recording
    if random.random() < 0.3:
        drive = torch.empty(b, 1, device=dev).uniform_(1, 6)
        peak = wave.abs().amax(dim=1, keepdim=True) + 1e-8
        wave = torch.tanh(wave / peak * drive) * peak / torch.tanh(drive)
    # what a separation leaves of other instruments: soft tones with slow swells
    if random.random() < 0.5:
        t = torch.arange(n, device=dev) / km.RATE
        tones = torch.zeros_like(wave)
        for _ in range(random.randint(1, 4)):
            f0 = torch.empty(b, 1, device=dev).uniform_(60, 1200)
            swell = 0.5 + 0.5 * torch.sin(2 * math.pi * torch.empty(b, 1, device=dev).uniform_(0.2, 2) * t)
            tones = tones + torch.sin(2 * math.pi * f0 * t) * swell
        level = 10 ** (torch.empty(b, 1, device=dev).uniform_(-45, -20) / 20)
        wave = wave + tones / (tones.std(dim=1, keepdim=True) + 1e-8) * wave.std(dim=1, keepdim=True) * level
    # hiss
    if random.random() < 0.4:
        level = 10 ** (torch.empty(b, 1, device=dev).uniform_(-60, -30) / 20)
        wave = wave + torch.randn_like(wave) * wave.std(dim=1, keepdim=True) * level
    # level
    wave = wave * 10 ** (torch.empty(b, 1, device=dev).uniform_(-15, 6) / 20)
    magnitude = frontend.magnitude(wave)
    # colour: a tilt and a few bumps and dips, -12 to +12 dB, smooth over log frequency
    if random.random() < 0.8:
        logf = torch.log2(frontend.freqs.clamp(min=20) / 1000)          # -5.6 .. 4.5
        curve = torch.empty(b, 1, device=dev).uniform_(-1.5, 1.5) * logf[None]
        for _ in range(3):
            centre = torch.empty(b, 1, device=dev).uniform_(-5, 4)
            width = torch.empty(b, 1, device=dev).uniform_(0.3, 1.5)
            curve = curve + torch.empty(b, 1, device=dev).uniform_(-9, 9) * torch.exp(-((logf[None] - centre) / width) ** 2)
        magnitude = magnitude * (10 ** (curve.clamp(-15, 15) / 20))[:, :, None]
    # what a separation does to the quiet parts of drums: they flicker, half of them cut away
    if random.random() < 0.4:
        loud = magnitude.amax(dim=(1, 2), keepdim=True)
        quiet = magnitude < loud * 10 ** (random.uniform(-50, -25) / 20)
        cut = torch.rand_like(magnitude) < random.uniform(0.3, 0.7)
        magnitude = torch.where(quiet & cut, magnitude * random.uniform(0.0, 0.3), magnitude)
    return magnitude


# --- training -------------------------------------------------------------------------------------------

def batches(songs, size, device):
    while True:
        waves, targets = [], []
        for _ in range(size):
            audio, target = random.choice(songs)
            if len(audio) <= CROP:
                start = 0
            else:
                start = random.randrange(0, len(audio) - CROP) // km.HOP * km.HOP
            piece = audio[start:start + CROP].astype(np.float32) / 32767
            piece = np.pad(piece, (0, CROP - len(piece)))
            f0 = start // km.HOP
            t = target[f0:f0 + CROP // km.HOP + 1]
            t = np.pad(t, ((0, CROP // km.HOP + 1 - len(t)), (0, 0)))
            waves.append(piece)
            targets.append(t)
        yield (torch.from_numpy(np.stack(waves)).to(device), torch.from_numpy(np.stack(targets)).to(device))


def peaks(curve, threshold):
    """Frames of the peaks of an activation (as kitnote_audio.pick_peaks: 30 ms apart)."""
    from scipy.signal import find_peaks
    found, _ = find_peaks(np.concatenate([[0.0], curve, [0.0]]), height=threshold, distance=3)
    return found - 1


def evaluate(model, songs, frontend, thresholds=None):
    """F-measure per class (50 ms), at the given thresholds or at the best of 0.2 .. 0.7 for each class."""
    model.eval()
    acts, truths = [], []
    with torch.no_grad():
        for audio, target in songs:
            # (a minute at a time: a long performance whole would fill the GPU's memory)
            parts = []
            for start in range(0, len(audio), 60 * km.RATE):
                part = torch.from_numpy(audio[start:start + 60 * km.RATE].astype(np.float32) / 32767)
                x = frontend.features(frontend.magnitude(part[None].to(frontend.device)))
                parts.append(torch.sigmoid(model(x))[0, :(len(part) + km.HOP - 1) // km.HOP].cpu().numpy())
            acts.append(np.concatenate(parts))
            truths.append(target)
    model.train()
    if frontend.device == "cuda":
        torch.cuda.empty_cache()
    result, chosen = {}, []
    for c, name in enumerate(km.CLASSES):
        best = (-1.0, 0.5)
        for th in ([thresholds[c]] if thresholds else np.arange(0.2, 0.75, 0.05)):
            tp = fp = fn = 0
            for act, truth in zip(acts, truths):
                found = peaks(act[:len(truth), c], th)
                real = np.flatnonzero(truth[:, c] == 1.0)
                used = np.zeros(len(real), bool)
                for f in found:
                    if len(real):
                        k = np.argmin(np.abs(real - f))
                        if abs(real[k] - f) <= 5 and not used[k]:
                            used[k] = True
                            tp += 1
                            continue
                    fp += 1
                fn += int((~used).sum())
            f1 = 2 * tp / max(1, 2 * tp + fp + fn)
            best = max(best, (f1, float(th)))
        result[name] = round(best[0], 3)
        chosen.append(round(best[1], 2))
    return result, chosen


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("--data", required=True, nargs="+", help="data folders (each with its own cache)")
    parser.add_argument("--cache", help="one folder for the cache of every data folder (best on a fast disk); "
                        "default: <data>/cache")
    parser.add_argument("--refresh", type=int, default=0,
                        help="every so many steps, add the performances written to the data folders since the start")
    parser.add_argument("--init", help="weights to start from (their width and hidden are kept)")
    parser.add_argument("--lr", type=float, default=1e-3, help="the highest learning rate")
    parser.add_argument("--boost", nargs="*", default=[], metavar="FOLDER:N",
                        help="choose the performances of folders starting with FOLDER N times as often")
    parser.add_argument("--out", required=True)
    parser.add_argument("--steps", type=int, default=20000)
    parser.add_argument("--batch", type=int, default=24)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--width", type=int, default=32, help="channels of the first convolutions")
    parser.add_argument("--hidden", type=int, default=96, help="units of the recurrent layer, each way")
    args = parser.parse_args()
    random.seed(args.seed)
    torch.manual_seed(args.seed)
    device = "cuda" if torch.cuda.is_available() else "cpu"

    start = time.time()
    boost = {b.split(":")[0]: int(b.split(":")[1]) for b in args.boost}
    seen = set()
    train = [song for data in args.data for song in load_split(data, "train", args.cache, seen, boost)]
    valid = [song for data in args.data for song in load_split(data, "validation", args.cache)]
    # (a fixed sample of the validation performances: the checks stay quick as the data grow)
    valid = random.Random(0).sample(valid, min(300, len(valid)))
    hours = sum(len(a) for a, _ in train) / km.RATE / 3600
    print(f"train: {len(train)} performances, {hours:.1f} h; validation: {len(valid)}; loaded in {time.time() - start:.0f} s", flush=True)

    frontend = Frontend(device)
    config = {"width": args.width, "hidden": args.hidden}
    if args.init:
        start_from = torch.load(args.init, map_location="cpu", weights_only=True)
        config = start_from.get("config") or config
    model = km.DrumNet(**config).to(device)
    if args.init:
        model.load_state_dict(start_from["state"])
    optimiser = torch.optim.AdamW(model.parameters(), lr=1e-3, weight_decay=1e-4)
    schedule = torch.optim.lr_scheduler.OneCycleLR(optimiser, max_lr=args.lr, total_steps=args.steps, pct_start=0.05)
    # strokes are rare among frames: they count more
    pos_weight = torch.tensor([4.0, 4.0, 6.0, 3.0, 5.0], device=device)
    best = -1.0
    stream = batches(train, args.batch, device)
    for step in range(1, args.steps + 1):
        wave, target = next(stream)
        x = frontend.features(augment(wave, frontend))
        logits = model(x)[:, :target.shape[1]]
        loss = F.binary_cross_entropy_with_logits(logits, target, pos_weight=pos_weight)
        optimiser.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(model.parameters(), 5.0)
        optimiser.step()
        schedule.step()
        if step % 50 == 0 and step <= 200:
            print(f"step {step}  loss {loss.item():.4f}  {time.time() - start:.0f} s", flush=True)
        if args.refresh and step % args.refresh == 0:
            # (renders still running when training began: their performances join as their tables appear)
            more = [song for data in args.data for song in load_split(data, "train", args.cache, seen, boost)]
            train.extend(more)
            if more:
                hours = sum(len(a) for a, _ in train) / km.RATE / 3600
                print(f"step {step}: {len(more)} more performances, {len(train)} in all, {hours:.1f} h", flush=True)
        if step % 1000 == 0 or step == args.steps:
            scores, thresholds = evaluate(model, valid, frontend)
            mean = float(np.mean(list(scores.values())))
            print(f"step {step}  loss {loss.item():.4f}  validation F {scores}  mean {mean:.3f}  thresholds {thresholds}  "
                  f"{time.time() - start:.0f} s", flush=True)
            if mean > best:
                best = mean
                torch.save({"state": {k: v.cpu() for k, v in model.state_dict().items()}, "config": config,
                            "thresholds": thresholds, "validation": scores,
                            "data": "Groove MIDI Dataset v1.0.0 (Google Magenta, CC BY 4.0)"}, args.out)
    print(f"best mean F {best:.3f}; weights in {args.out}")


if __name__ == "__main__":
    main()
