# SPDX-License-Identifier: GPL-3.0-only
"""Plays the performances of the Groove MIDI Dataset (CC BY 4.0) on acoustic drum kits sampled for DrumGizmo
(DRSKit, MuldjordKit: CC BY 4.0, drumgizmo.org) - many microphones, every stroke at many strengths - and writes
them as audio with their MIDI: training data for KitNote's drum model in the sound of real drums in a room.

    python render_kits.py --data <folder> --kits <DRSKit folder> [<MuldjordKit folder> ...] [--per-kit 400]

Writes <data>/rendered/<kit>/<n>.wav and .mid, and <data>/rendered/rendered.csv (as the Groove table).
A kit is a DrumGizmo kit folder, an .sfz file (sfz_kit.py) or a folder of one-shots; kit_maps.py says what the
pieces of a kit are where their names do not, and convert_kits.py makes SFZ kits of Hydrogen, Drumlabooh and
SoundFont kits.
"""
import argparse
import csv
import os
import random
import re
import shutil
import xml.etree.ElementTree as ET

import mido
import numpy as np
import soundfile as sf

RATE = 44100

# the Groove MIDI pitches (Roland TD-11), and what to look for among a kit's instruments, in order of preference
WANTED = {
    36: [r"kdrum_with", r"^kick_?r", r"kickdrum_?r", r"^kick", r"kdrum", r"kick"],
    35: [r"kdrum_without", r"kdrum", r"kick"],
    38: [r"^snare$", r"^snare_?(center|mid|hit)", r"^snare(?!.*(rim|side|whisk|rest|bottom|flam|roll))", r"snare"],
    40: [r"snare_?rim_?shot", r"snare_rim(?!.*click)", r"^snare$", r"snare"],
    37: [r"snare_?(side|rim_?click|cross|stick)", r"snare_?rim(?!_?shot)", r"^snare$", r"snare"],
    48: [r"^tom_?1$", r"tom_?1(?!.*whisk)", r"hi_?tom", r"tom"],
    50: [r"^tom_?1$", r"tom_?1", r"tom"],
    45: [r"^tom_?2$", r"tom_?2(?!.*whisk)", r"mid_?tom", r"tom"],
    47: [r"^tom_?2$", r"tom_?2", r"tom"],
    43: [r"^tom_?4$", r"^tom_?3$", r"f_?tom_?1", r"floor", r"tom_?3", r"tom"],
    58: [r"^tom_?4$", r"^tom_?3$", r"f_?tom", r"floor", r"tom_?3", r"tom"],
    41: [r"^tom_?4$", r"f_?tom_?2", r"^tom_?3$", r"f_?tom", r"floor", r"tom_?3", r"tom"],
    42: [r"hihat_closed_shank", r"hihat_?closed(?!.*whisk)", r"hh_?closed", r"hihat"],
    22: [r"hihat_?closed(?!.*whisk)", r"hh_?closed", r"hihat"],
    46: [r"hihat_?open(?!.*(whisk|tip))", r"hihat_semi", r"hh_?open", r"hihat"],
    26: [r"hihat_open_tip", r"hihat_?open", r"hh_?open", r"hihat"],
    44: [r"hihat_?foot", r"hihat_?pedal_?hit", r"hihat_?pedal", r"hh_?foot", r"pedal", r"hihat"],
    49: [r"crash_left_shank", r"crash_?1", r"crash_?l", r"crash(?!.*(whisk|choke))", r"crash"],
    55: [r"crash_left_tip", r"crash_?1", r"crash_?l", r"crash"],
    57: [r"crash_right_shank", r"crash_?2", r"crash_?r", r"crash(?!.*(whisk|choke))", r"crash"],
    52: [r"crash_right_tip", r"crash_?2", r"china", r"crash"],
    51: [r"^ride_tip$", r"ride_?bow", r"^ride_?1$", r"^ride(?!.*(bell|whisk|chain|rest|shank))", r"ride"],
    59: [r"^ride_shank$", r"ride_?edge", r"^ride_?2$", r"^ride(?!.*(bell|whisk|chain|rest))", r"ride"],
    53: [r"^ride_tip_bell$", r"ride_shank_bell", r"ride_?bell", r"bell", r"ride"],
}


class Kit:
    def __init__(self, folder):
        self.folder = folder
        name = os.path.basename(os.path.normpath(folder))
        kit_files = [f for f in os.listdir(folder) if f.endswith(".xml") and not f.lower().startswith("midimap")]
        # the kit description with the most instruments
        best = max(kit_files, key=lambda f: open(os.path.join(folder, f), encoding="utf-8", errors="ignore").read().count("<instrument "))
        root = ET.parse(os.path.join(folder, best)).getroot()
        self.name = name
        self.channels = [c.get("name") for c in root.iter("channel")]
        self.instruments = {}
        self.channel_maps = {}      # instrument -> {its microphone: the kit's channel}, where a kit renames them
        for inst in root.iter("instrument"):
            self.instruments[inst.get("name")] = (os.path.join(folder, inst.get("file")), inst.get("group"))
            self.channel_maps[inst.get("name")] = {m.get("in"): m.get("out") for m in inst.iter("channelmap")}
        self.samples = {}           # instrument -> [(power, [(channel, file, filechannel)])]
        self.audio = {}             # file -> [frames, channels] int16 (with its scale)
        self.choose = {}
        lowered = {n.lower(): n for n in self.instruments}
        for pitch, patterns in WANTED.items():
            for pattern in patterns:
                hits = [lowered[n] for n in sorted(lowered) if re.search(pattern, n)]
                if hits:
                    self.choose[pitch] = hits[0]
                    break
        from kit_maps import DRUMGIZMO
        self.choose.update({p: i for p, i in DRUMGIZMO.get(name, {}).items() if i in self.instruments})
        print(f"{name}: {len(self.instruments)} instruments, {len(self.channels)} channels; "
              + ", ".join(f"{p}->{i}" for p, i in sorted(self.choose.items())), flush=True)

    def layers(self, instrument):
        if instrument not in self.samples:
            path, _ = self.instruments[instrument]
            root = ET.parse(path).getroot()
            base = os.path.dirname(path)
            layers = []
            rename = self.channel_maps.get(instrument, {})
            # (the old format: no power to a sample, but velocity groups naming it)
            groups = {ref.get("name"): (float(v.get("lower")) + float(v.get("upper"))) / 2
                      for v in root.iter("velocity") for ref in v.iter("sampleref")}
            for sample in root.iter("sample"):
                files = [(rename.get(a.get("channel"), a.get("channel")), os.path.join(base, a.get("file")), int(a.get("filechannel")))
                         for a in sample.iter("audiofile")]
                power = sample.get("power")
                if power is None and sample.get("name") in groups:
                    power = groups[sample.get("name")] ** 2
                layers.append((float(power if power is not None else 1.0), files))
            layers.sort(key=lambda layer: layer[0])
            self.samples[instrument] = layers
        return self.samples[instrument]

    def read(self, path):
        if path not in self.audio:
            data, rate = sf.read(path, dtype="float32", always_2d=True)
            if rate != RATE:
                from math import gcd
                from scipy.signal import resample_poly
                g = gcd(rate, RATE)
                data = resample_poly(data, RATE // g, rate // g, axis=0).astype(np.float32)
            scale = float(np.abs(data).max()) or 1.0
            self.audio[path] = ((data / scale * 32767).astype(np.int16), scale)
        data, scale = self.audio[path]
        return data, scale

    def group_of(self, pitch):
        """The choke group of the instrument a pitch plays (the hi-hat), or None."""
        instrument = self.choose.get(pitch)
        return self.instruments[instrument][1] if instrument else None

    def stroke(self, pitch, velocity, gains, chosen=None):
        """The mono sound of one stroke: the layer nearest the strength, its microphones mixed by gains."""
        instrument = self.choose.get(pitch)
        if instrument is None:
            return None, None
        layers = self.layers(instrument)
        if not layers:
            return None, None
        top = layers[-1][0]
        want = top * (velocity / 127.0) ** 2 * random.uniform(0.8, 1.25)
        k = int(np.argmin([abs(p - want) for p, _ in layers]))
        k = min(len(layers) - 1, max(0, k + random.choice((-1, 0, 0, 1))))
        out = None
        for channel, path, filechannel in layers[k][1]:
            gain = gains.get(channel, 0.0)
            if gain <= 0:
                continue
            data, scale = self.read(path)
            column = data[:, filechannel - 1].astype(np.float32) * (scale / 32767 * gain)
            if out is None:
                out = column
            else:
                n = min(len(out), len(column))
                out = out[:n] + column[:n]
        return out, self.instruments[instrument][1]


class OneShotKit:
    """A kit of single strokes, one file a strength: <piece>-<0..9>.wav (StemGMD's single hits: 10 acoustic kits,
    Mezza et al., CC BY 4.0)."""
    PIECES = {36: "kick", 35: "kick", 38: "snare", 40: "snare", 37: "snare", 42: "hihat_closed", 22: "hihat_closed",
              44: "hihat_closed", 46: "hihat_open", 26: "hihat_open", 48: "hi_tom", 50: "hi_tom", 45: "mid_tom",
              47: "mid_tom", 43: "low_tom", 58: "low_tom", 41: "low_tom", 49: "crash_left", 55: "crash_left", 57: "crash_left",
              52: "crash_left", 51: "ride", 59: "ride", 53: "ride"}

    def __init__(self, folder):
        self.folder = folder
        self.name = "stemgmd-" + os.path.basename(os.path.normpath(folder))
        self.channels = ["close"]
        self.instruments = {}
        self.audio = {}
        self.layers = {}
        for f in os.listdir(folder):
            m = re.match(r"(.+)-(\d+)\.wav$", f)
            if m:
                self.layers.setdefault(m.group(1), []).append((int(m.group(2)), os.path.join(folder, f)))
        for piece in self.layers:
            self.layers[piece].sort()
        print(f"{self.name}: " + ", ".join(sorted(self.layers)), flush=True)

    def group_of(self, pitch):
        return "hihat" if self.PIECES.get(pitch, "").startswith("hihat") else None

    def stroke(self, pitch, velocity, gains, chosen=None):
        piece = self.PIECES.get(pitch)
        layers = self.layers.get(piece)
        if not layers:
            return None, None
        k = int(round((velocity - 30) / 97 * (len(layers) - 1)))
        k = min(len(layers) - 1, max(0, k + random.choice((-1, 0, 0, 0, 1))))
        path = layers[k][1]
        if path not in self.audio:
            data, rate = sf.read(path, dtype="float32", always_2d=True)
            self.audio[path] = data.mean(axis=1)
        gain = 10 ** (random.uniform(-2, 2) / 20)
        if pitch == 44:
            gain *= 0.4             # (the pedal: a quiet closed hi-hat)
        return self.audio[path] * gain, self.group_of(pitch)


def mic_gains(kit):
    """A random balance of the microphones: close ones, overheads and the room, as engineers do differently."""
    gains = {}
    room = random.uniform(0.0, 0.8)
    over = random.uniform(0.4, 1.0)
    for ch in kit.channels:
        low = ch.lower()
        if "amb" in low or "room" in low:
            gains[ch] = room
        elif "oh" in low or "over" in low:
            gains[ch] = over
        elif "trig" in low:
            gains[ch] = 0.0
        elif "vintage" in low:
            gains[ch] = random.choice((0.0, 0.0, random.uniform(0.2, 0.8)))
        else:
            gains[ch] = random.uniform(0.3, 1.0)
    return gains


def render(kit, midi_path, out_wav, out_midi=None):
    """Plays a performance on a kit - a little faster or slower (up to 15 %), softer or harder - and writes the
    audio, and (out_midi) the performance as played, for its labels."""
    notes = []
    now = 0.0
    for message in mido.MidiFile(midi_path):
        now += message.time
        if message.type == "note_on" and message.velocity > 0:
            notes.append((now, message.note, message.velocity))
    if not notes:
        return False
    stretch = random.uniform(0.87, 1.15)
    push = random.uniform(-15, 15)
    notes = [(t * stretch, p, int(min(127, max(1, v + push)))) for t, p, v in notes]
    gains = mic_gains(kit)
    chosen = {}             # (an SFZ kit: which of its keys of a kind this performance plays)
    length = int((notes[-1][0] + 3.0) * RATE)
    out = np.zeros(length, np.float32)
    played = []             # (the labels: only the strokes the kit has a sound for)
    # the hi-hat chokes itself: a stroke on it cuts the one before
    for k, (t, pitch, velocity) in enumerate(notes):
        sound, group = kit.stroke(pitch, velocity, gains, chosen)
        if sound is None:
            continue
        played.append((t, pitch, velocity))
        start = int(t * RATE)
        if group:
            later = [u for u, p, _ in notes[k + 1:] if kit.group_of(p) == group]
            if later:
                cut = int((later[0] - t) * RATE)
                if cut < len(sound):
                    fade = min(len(sound) - cut, int(0.015 * RATE))
                    sound = sound[:cut + fade].copy()
                    sound[cut:] *= np.linspace(1, 0, len(sound) - cut)
        end = min(length, start + len(sound))
        out[start:end] += sound[:end - start]
    if not played:
        return False
    if out_midi:
        write_midi(played, out_midi)
    peak = float(np.abs(out).max()) or 1.0
    sf.write(out_wav, (out / peak * 0.9 * 32767).astype(np.int16), RATE, subtype="PCM_16")
    return True


def write_midi(notes, path):
    """Notes [(seconds, pitch, velocity)] as a MIDI file at 120 BPM (a beat = 0.5 s)."""
    midi = mido.MidiFile(ticks_per_beat=480)
    track = mido.MidiTrack()
    midi.tracks.append(track)
    track.append(mido.MetaMessage("set_tempo", tempo=500000, time=0))
    events = []
    for t, p, v in notes:
        events.append((int(round(t * 960)), 1, p, v))
        events.append((int(round((t + 0.05) * 960)), 0, p, 0))
    last = 0
    for tick, on, p, v in sorted(events):
        track.append(mido.Message("note_on", note=p, velocity=v if on else 0, channel=9, time=tick - last))
        last = tick
    midi.save(path)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("--data", required=True)
    parser.add_argument("--kits", nargs="+", required=True)
    parser.add_argument("--per-kit", type=int, default=400)
    parser.add_argument("--seed", type=int, default=1)
    parser.add_argument("--midi", default="groove",
                        help="the performances: a folder of <data> with a table (groove: info.csv; random_grooves.py: grooves.csv)")
    parser.add_argument("--out-name", default="rendered", help="the folder of <data> (and its table) to write into")
    parser.add_argument("--share", action="store_true",
                        help="give each kit its own part of the performances, instead of per-kit samples of all")
    args = parser.parse_args()
    random.seed(args.seed)
    midi_root = os.path.join(args.data, args.midi)
    table_name = next(f for f in ("info.csv", "grooves.csv") if os.path.exists(os.path.join(midi_root, f)))
    rows = [r for r in csv.DictReader(open(os.path.join(midi_root, table_name), newline="")) if r["split"] in ("train", "validation")]
    out_root = os.path.join(args.data, args.out_name)
    table = []
    shares = {}
    if args.share:
        shuffled = rows[:]
        random.shuffle(shuffled)
        for k, folder in enumerate(args.kits):
            shares[folder] = shuffled[k::len(args.kits)]
    for folder in args.kits:
        if folder.lower().endswith(".sfz"):
            from sfz_kit import SfzKit
            from kit_maps import sfz_map
            keymap, cc, offsets, skip = sfz_map(folder)
            kit = SfzKit(folder, keymap, cc, offsets, skip)
            kit.name = os.path.basename(os.path.dirname(os.path.abspath(folder)))
            if kit.name.lower() == "programs":
                kit.name = os.path.basename(os.path.dirname(os.path.dirname(os.path.abspath(folder))))
        elif any(re.match(r".+-\d+\.wav$", f) for f in os.listdir(folder)):
            kit = OneShotKit(folder)
        else:
            kit = Kit(folder)
        os.makedirs(os.path.join(out_root, kit.name), exist_ok=True)
        if args.share:
            chosen = shares[folder]
        else:
            chosen = random.sample([r for r in rows if r["split"] == "train"], min(args.per_kit, sum(r["split"] == "train" for r in rows)))
            chosen += random.sample([r for r in rows if r["split"] == "validation"], 20)
        for n, row in enumerate(chosen):
            midi = os.path.join(midi_root, row["midi_filename"])
            wav_rel = f"{kit.name}/{n}.wav"
            mid_rel = f"{kit.name}/{n}.mid"
            if render(kit, midi, os.path.join(out_root, wav_rel), os.path.join(out_root, mid_rel)):
                table.append({**row, "audio_filename": wav_rel, "midi_filename": mid_rel, "kit_name": kit.name})
            if n % 50 == 0:
                print(f"{kit.name}: {n + 1}/{len(chosen)}", flush=True)
        kit.audio.clear()
    # (the kits rendered before stay in the table)
    path = os.path.join(out_root, args.out_name + ".csv")
    names = {row["kit_name"] for row in table}
    if os.path.exists(path):
        table = [r for r in csv.DictReader(open(path, newline="", encoding="utf-8")) if r["kit_name"] not in names] + table
    with open(path, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=list(table[-1].keys()))
        writer.writeheader()
        writer.writerows(table)
    print("done", len(table))


if __name__ == "__main__":
    main()
