# SPDX-License-Identifier: GPL-3.0-only
"""Turns drum kits render_kits.py cannot read into SFZ kits it can: a folder with <name>.sfz and its samples, each
file named by the kind of stroke it is (kick, snare_rim, hihat_open, tom_low, ride_bell ...), as sfz_kit.kind_of
reads them.

    python convert_kits.py hydrogen <kit folder with drumkit.xml> <out folder>
    python convert_kits.py drumlabooh <kit folder with drumkit.txt or drumkit.labooh> <out folder>
    python convert_kits.py soundfont <.sf2/.sf3> <out folder> <name>=<preset>[,<preset>...] ...

Hydrogen and Drumlabooh kits: the instruments kit_maps.HYDROGEN / DRUMLABOOH names, each velocity layer (or
alternative, as a random one) a region. SoundFonts: the GM keys of
the presets of the percussion bank (128) given, played as a SoundFont synthesizer would - pitch, filter and
volume envelopes, loops - into one sample a velocity layer; presets of a family (Standard 1 .. 7) go into one kit,
its kick and snare variants side by side as keys of the same kind.
"""
import argparse
import io
import os
import re
import struct
import xml.etree.ElementTree as ET

import numpy as np
import soundfile as sf

from kit_maps import DRUMLABOOH, GM_KINDS, HYDROGEN

RATE = 44100
# file names of the kinds, as sfz_kit.kind_of reads them
FILE_NAME = {"kick": "kick", "snare": "snare", "rim": "snare_rim", "stick": "snare_sidestick", "hh_closed": "hihat_closed",
             "hh_open": "hihat_open", "hh_pedal": "hihat_pedal", "tom_high": "tom_high", "tom_mid": "tom_mid",
             "tom_low": "tom_low", "crash": "crash", "ride": "ride", "bell": "ride_bell"}


def write_kit(out, name, regions):
    """regions: [(kind, key, lovel, hivel, volume dB, amp_veltrack, mono float32 at RATE[, (n, of n alternatives)])]
    -> out/<name>.sfz"""
    os.makedirs(os.path.join(out, "samples"), exist_ok=True)
    lines = [f"// {name}: converted by tools/kitnote/convert_kits.py for render_kits.py", "<control> default_path=samples/"]
    for kind, key, lo, hi, volume, veltrack, audio, *alt in regions:
        file, rand = f"{FILE_NAME[kind]}_{key}_{lo}", ""
        if alt:
            n, count = alt[0]
            file += f"_{n}"
            rand = f" lorand={n / count:.4f} hirand={(n + 1) / count:.4f}"
        sf.write(os.path.join(out, "samples", file + ".wav"), audio, RATE, subtype="FLOAT")
        lines.append(f"<region> key={key} lovel={lo} hivel={hi}{rand} volume={volume:.2f} amp_veltrack={veltrack} sample={file}.wav")
    with open(os.path.join(out, name + ".sfz"), "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(lines) + "\n")


def mono(path):
    data, rate = sf.read(path, dtype="float32", always_2d=True)
    data = data.mean(axis=1)
    if rate != RATE:
        from math import gcd
        from scipy.signal import resample_poly
        g = gcd(rate, RATE)
        data = resample_poly(data, RATE // g, rate // g).astype(np.float32)
    # the stroke at the start: what comes before it (silence, a hi-hat pedal moving) left out but 2 ms, so the
    # label of a stroke is where it sounds
    loud = np.abs(data) > 0.1 * max(1e-9, float(np.abs(data).max()))
    start = max(0, int(np.argmax(loud)) - int(0.002 * RATE))
    data = data[start:].copy()
    data[:int(0.001 * RATE)] *= np.linspace(0, 1, min(len(data), int(0.001 * RATE)))
    return data


# --- Hydrogen ---------------------------------------------------------------------------------------------------

def hydrogen(folder, out):
    text = open(os.path.join(folder, "drumkit.xml"), encoding="utf-8", errors="ignore").read()
    root = ET.fromstring(re.sub(r'xmlns="[^"]+"', "", text).encode())
    name = root.findtext("name").strip()
    patterns = HYDROGEN[name]
    regions = []
    key = 0
    for inst in root.iter("instrument"):
        label = (inst.findtext("name") or "").strip()
        kinds = [k for k, p in patterns.items() if re.fullmatch(p, label, re.I)]
        layers = [(float(l.findtext("min", "0")), float(l.findtext("max", "1")), float(l.findtext("gain", "1") or 1),
                   float(l.findtext("pitch", "0") or 0), l.findtext("filename")) for l in inst.iter("layer")]
        if not layers and inst.findtext("filename"):            # (the oldest format: one sample, no layers)
            layers = [(0.0, 1.0, 1.0, 0.0, inst.findtext("filename"))]
        if not kinds or not layers:
            print(f"  - {label}")
            continue
        if len(kinds) > 1:
            raise ValueError(f"{label}: more than one kind {kinds}")
        kind = kinds[0]
        gain = float(inst.findtext("volume", "1") or 1) * float(inst.findtext("gain", "1") or 1)
        layers.sort()
        veltrack = 50 if len(layers) > 1 else 100               # (the layers themselves get louder)
        low = 1
        for n, (lo, hi, layer_gain, pitch, file) in enumerate(layers):
            top = 127 if n == len(layers) - 1 else max(low, min(127, int(round(hi * 127))))
            audio = mono(os.path.join(folder, file))
            if pitch:
                from scipy.signal import resample
                audio = resample(audio, int(len(audio) / 2 ** (pitch / 12))).astype(np.float32)
            regions.append((kind, key, low, top, 20 * np.log10(max(gain * layer_gain, 1e-4)), veltrack, audio))
            low = top + 1
            if low > 127:
                break
        print(f"  {kind:9s} key {key:3d} <- {label} ({len(layers)} layers)")
        key += 1
    write_kit(out, os.path.basename(os.path.normpath(out)), regions)


# --- Drumlabooh -------------------------------------------------------------------------------------------------

def drumlabooh(folder, out):
    """drumkit.txt: "[note]name=file,file ..." (velocity layers, soft to loud); drumkit.labooh: <sample name=..>
    with a folder or a list file of alternatives (layer_index_mode="alt") or of layers."""
    name = os.path.basename(os.path.normpath(folder))
    patterns = DRUMLABOOH[name]
    instruments = []                                            # (name, [files], alternatives)
    if os.path.exists(os.path.join(folder, "drumkit.labooh")):
        root = ET.parse(os.path.join(folder, "drumkit.labooh")).getroot()
        for el in root.iter("sample"):
            where = os.path.join(folder, (el.text or "").strip())
            if os.path.isdir(where):
                files = sorted(os.path.join(where, f) for f in os.listdir(where) if re.search(r"\.(wav|flac|aiff?|ogg)$", f, re.I))
            else:
                files = [os.path.join(os.path.dirname(where), l.strip()) for l in open(where, encoding="utf-8") if l.strip()]
            instruments.append((el.get("name"), files, el.get("layer_index_mode") == "alt"))
    else:
        for line in open(os.path.join(folder, "drumkit.txt"), encoding="utf-8", errors="ignore"):
            m = re.match(r"\s*(\[\d+\])?\*?([^=]+)=(.+)", line)
            if m:
                files, base = [], folder
                for f in m.group(3).split(","):         # (a file in a folder: the next ones are in it too)
                    f = f.strip()
                    if f:
                        if "/" in f:
                            base = os.path.join(folder, os.path.dirname(f))
                        files.append(os.path.join(base, os.path.basename(f)))
                # (a name given twice: the line with the most files)
                instruments = [i for i in instruments if i[0] != m.group(2).strip() or len(i[1]) >= len(files)]
                if m.group(2).strip() not in [i[0] for i in instruments]:
                    instruments.append((m.group(2).strip(), files, False))
    regions, key = [], 0
    for label, files, alternatives in instruments:
        kinds = [k for k, p in patterns.items() if re.fullmatch(p, label, re.I)]
        if not kinds or not files:
            print(f"  - {label}")
            continue
        kind = kinds[0]
        if alternatives:
            regions += [(kind, key, 1, 127, 0.0, 100, mono(f), (n, len(files))) for n, f in enumerate(files)]
        else:
            edges = [1 + round(126 * n / len(files)) for n in range(len(files))] + [128]
            regions += [(kind, key, edges[n], edges[n + 1] - 1, 0.0, 50 if len(files) > 1 else 100, mono(f))
                        for n, f in enumerate(files)]
        print(f"  {kind:9s} key {key:3d} <- {label} ({len(files)} {'alternatives' if alternatives else 'layers'})")
        key += 1
    write_kit(out, os.path.basename(os.path.normpath(out)), regions)


# --- SoundFont --------------------------------------------------------------------------------------------------

def chunks(data, start, end):
    pos = start
    while pos + 8 <= end:
        tag, size = data[pos:pos + 4], struct.unpack_from("<I", data, pos + 4)[0]
        yield tag, pos + 8, size
        pos += 8 + size + (size & 1)


def records(body, fmt):
    size = struct.calcsize(fmt)
    return [struct.unpack_from(fmt, body, i) for i in range(0, len(body) - size + 1, size)]


SIGNED = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34,
          35, 36, 37, 38, 39, 40, 45, 46, 47, 48, 50, 51, 52, 56, 57, 58}
DEFAULTS = {8: 13500, 21: -12000, 23: -12000, 25: -12000, 26: -12000, 27: -12000, 28: -12000, 30: -12000, 33: -12000,
            34: -12000, 35: -12000, 36: -12000, 38: -12000, 56: 100, 58: -1}
# generators that a preset zone adds to the instrument's (all but ranges, sample and the sample-level ones)
NOT_ADDED = {0, 1, 2, 3, 4, 12, 43, 44, 45, 46, 47, 50, 53, 54, 57, 58}


class SoundFont:
    def __init__(self, path):
        src = open(path, "rb").read()
        self.smpl, pdta = b"", {}
        for tag, pos, size in chunks(src, 12, len(src)):
            if tag != b"LIST":
                continue
            for sub, spos, ssize in chunks(src, pos + 4, pos + size):
                if src[pos:pos + 4] == b"sdta" and sub == b"smpl":
                    self.smpl = src[spos:spos + ssize]
                elif src[pos:pos + 4] == b"pdta":
                    pdta[sub] = src[spos:spos + ssize]
        self.phdr = records(pdta[b"phdr"], "<20sHHHIII")
        self.pbag = records(pdta[b"pbag"], "<HH")
        self.pgen = records(pdta[b"pgen"], "<HH")
        self.inst = records(pdta[b"inst"], "<20sH")
        self.ibag = records(pdta[b"ibag"], "<HH")
        self.igen = records(pdta[b"igen"], "<HH")
        self.shdr = records(pdta[b"shdr"], "<20sIIIIIBbHH")
        self.cache = {}

    @staticmethod
    def gens(gen_list, first, last):
        out = {}
        for oper, amount in gen_list[first:last]:
            if oper in (43, 44):
                out[oper] = (amount & 255, amount >> 8)
            else:
                out[oper] = amount - 65536 if oper in SIGNED and amount >= 32768 else amount
        return out

    def zones(self, bag_list, gen_list, first, last, link):
        """[(generators)] of a preset's or instrument's zones, its global zone folded into the others."""
        zones = [self.gens(gen_list, bag_list[b][0], bag_list[b + 1][0]) for b in range(first, last)]
        if zones and link not in zones[0]:
            glob, zones = zones[0], zones[1:]
            zones = [{**glob, **z} for z in zones]
        return [z for z in zones if link in z]

    def preset(self, number, bank=128):
        for i, p in enumerate(self.phdr[:-1]):
            if p[1] == number and p[2] == bank:
                return i, p[0].split(b"\0")[0].decode("latin-1")
        return None, None

    def voices(self, preset_index, key):
        """[(generators, sample index)] of the zones a key plays, preset generators added to the instrument's."""
        out = []
        p = self.phdr
        for pz in self.zones(self.pbag, self.pgen, p[preset_index][3], p[preset_index + 1][3], 41):
            if not pz.get(43, (0, 127))[0] <= key <= pz.get(43, (0, 127))[1]:
                continue
            ii = pz[41]
            for iz in self.zones(self.ibag, self.igen, self.inst[ii][1], self.inst[ii + 1][1], 53):
                if not iz.get(43, (0, 127))[0] <= key <= iz.get(43, (0, 127))[1]:
                    continue
                g = {**DEFAULTS, **iz}
                for oper, amount in pz.items():
                    if oper not in NOT_ADDED and oper != 41:
                        g[oper] = g.get(oper, 0) + amount
                pv, iv = pz.get(44, (0, 127)), iz.get(44, (0, 127))
                g[44] = (max(pv[0], iv[0]), min(pv[1], iv[1]))
                if g[44][0] <= g[44][1]:
                    out.append((g, iz[53]))
        return out

    def sample(self, index):
        """(float32 audio, rate, original key, correction, loop start, loop end, kind) of a sample header."""
        if index not in self.cache:
            name, start, end, ls, le, rate, key, corr, link, kind = self.shdr[index]
            if kind & 0x10:                                     # .sf3: an Ogg Vorbis stream, loops relative to it
                audio, rate = sf.read(io.BytesIO(self.smpl[start:end]), dtype="float32", always_2d=True)
                audio = audio[:, 0]
            else:
                audio = np.frombuffer(self.smpl[start * 2:end * 2], "<i2").astype(np.float32) / 32768
                ls, le = ls - start, le - start
            self.cache[index] = (audio, rate, key if key <= 127 else 60, corr, ls, le, kind)
        return self.cache[index]


def timecents(tc):
    return 0.0 if tc <= -12000 else 2.0 ** (tc / 1200)


def envelope(g, base, key, t, release_at):
    """An SF2 envelope (base 25: modulation, 33: volume) as 0..1 over times t, released at release_at (None: held).
    Attack linear, decay and release linear in the 0..1 value (for the volume: 0..-96 dB)."""
    delay, attack = timecents(g.get(base, -12000)), timecents(g.get(base + 1, -12000))
    hold = timecents(g.get(base + 2, -12000) + (60 - key) * g.get(base + 6, 0))
    decay = timecents(g.get(base + 3, -12000) + (60 - key) * g.get(base + 7, 0))
    sustain = 1 - min(max(g.get(base + 4, 0), 0), 1000) / 1000
    release = timecents(g.get(base + 5, -12000))
    v = np.zeros_like(t)
    a = (t >= delay) & (t < delay + attack)
    v[a] = (t[a] - delay) / attack if attack else 1
    h = t >= delay + attack
    after = t[h] - (delay + attack + hold)
    v[h] = np.where(after <= 0, 1.0, np.maximum(sustain, 1 - after / decay) if decay else sustain)
    if release_at is not None:
        r = t >= release_at
        if r.any():
            level = v[np.argmax(r)]
            v[r] = np.maximum(0, level - (t[r] - release_at) / release) if release else 0
    return v


def render(sf2, g, index, key):
    """One zone of a key, as a SoundFont synthesizer plays it, held until it dies away (or 0.3 s, if it loops)."""
    audio, rate, root_key, corr, ls, le, kind = sf2.sample(index)
    start = g.get(0, 0) + 32768 * g.get(4, 0)
    end = len(audio) + g.get(1, 0) + 32768 * g.get(12, 0)
    ls, le = ls + g.get(2, 0) + 32768 * g.get(45, 0) - start, le + g.get(3, 0) + 32768 * g.get(50, 0) - start
    audio = audio[max(0, start):max(start + 1, end)]
    loops = g.get(54, 0) in (1, 3) and 0 <= ls < le <= len(audio) and le - ls > 8
    root = g[58] if g.get(58, -1) >= 0 else root_key
    cents = (key - root) * g.get(56, 100) + g.get(51, 0) * 100 + g.get(52, 0) + corr
    step = 2 ** (cents / 1200) * rate / RATE
    t = np.arange(int(6.0 * RATE)) / RATE
    release_at = 0.3 if loops else None
    vol = envelope(g, 33, key, t, release_at)
    mod = envelope(g, 25, key, t, release_at)
    # where in the sample each output sample is: the pitch bent by the modulation envelope
    rates = step * 2 ** (mod * g.get(7, 0) / 1200)
    pos = np.concatenate([[0.0], np.cumsum(rates[:-1])])
    if loops:
        over = pos >= le
        pos[over] = ls + (pos[over] - ls) % (le - ls)
    else:
        keep = pos < len(audio) - 1
        pos, vol, mod, t = pos[keep], vol[keep], mod[keep], t[keep]
    out = np.interp(pos, np.arange(len(audio)), audio).astype(np.float32)
    # the volume envelope (its 0..1 as 0..-96 dB after the attack) and the attenuation (x 0.4, as FluidSynth)
    amp = np.where(t < timecents(g.get(33, -12000)) + timecents(g.get(34, -12000)), vol, 10 ** (-96 * (1 - vol) / 20))
    amp[vol <= 0] = 0
    out *= amp * 10 ** (-0.4 * max(0, g.get(48, 0)) / 200)
    # the low-pass filter, its cutoff moved by the modulation envelope (block by block)
    cutoff = g.get(8, 13500) + mod * g.get(11, 0)
    if (cutoff < 13400).any():
        from scipy.signal import lfilter
        q = max(0.5, 10 ** ((g.get(9, 0) / 10 - 3.01) / 20))      # (its resonance in dB, as FluidSynth)
        filtered, state = np.zeros_like(out), np.zeros(2)
        for b in range(0, len(out), 64):
            fc = min(8.176 * 2 ** (cutoff[b] / 1200), RATE * 0.45)
            w = 2 * np.pi * fc / RATE
            alpha = np.sin(w) / (2 * q)
            bq = np.array([(1 - np.cos(w)) / 2, 1 - np.cos(w), (1 - np.cos(w)) / 2])
            aq = np.array([1 + alpha, -2 * np.cos(w), 1 - alpha])
            filtered[b:b + 64], state = lfilter(bq / aq[0], aq / aq[0], out[b:b + 64], zi=state)
        out = filtered.astype(np.float32)
    # cut the tail where it has died away
    loud = np.nonzero(np.abs(out) > 10 ** (-80 / 20) * max(1e-9, float(np.abs(out).max())))[0]
    out = out[:loud[-1] + 1] if len(loud) else out[:1]
    return out * (0.5 if kind & 0x6 else 1.0), kind


def soundfont(path, out_root, families):
    sf2 = SoundFont(path)
    for family, presets in families.items():
        regions, seen, key = [], set(), 0
        for number in presets:
            index, preset_name = sf2.preset(number)
            if index is None:
                raise ValueError(f"no preset 128:{number} in {path}")
            for gm, kind in sorted(GM_KINDS.items()):
                voices = sf2.voices(index, gm)
                if not voices:
                    continue
                # the velocity bands of the key's zones
                edges = sorted({v[0][44][0] for v in voices} | {v[0][44][1] + 1 for v in voices} | {1, 128})
                bands = [(lo, hi - 1) for lo, hi in zip(edges, edges[1:]) if lo >= 1 and hi - 1 <= 127 and lo <= hi - 1]
                layers = []
                for lo, hi in bands:
                    playing = [(g, s) for g, s in voices if g[44][0] <= hi and g[44][1] >= lo]
                    if not playing:
                        continue
                    sig = repr(sorted((s, sorted((k, v) for k, v in g.items() if k != 44)) for g, s in playing))
                    if layers and layers[-1][2] == sig:          # (the same sound as the band below: one band)
                        layers[-1][1] = hi
                        continue
                    layers.append([lo, hi, sig, playing])
                if not layers:
                    continue
                layers[0][0] = 1
                sig = repr([(l[0], l[1], l[2]) for l in layers]) + str(gm)
                if sig in seen:                                 # (the same as in a preset before)
                    continue
                seen.add(sig)
                rendered = []
                for lo, hi, _, playing in layers:
                    sounds = [render(sf2, g, s, gm)[0] for g, s in playing]
                    n = max(len(x) for x in sounds)
                    rendered.append((kind, key, lo, hi, 0.0, 100, sum(np.pad(x, (0, n - len(x))) for x in sounds)))
                names = sorted({sf2.shdr[s][0].split(b"\0")[0].decode("latin-1") for _, s in layers[-1][3]})
                # a stroke must hit at once, or its label would be early (a reverse cymbal, a swell, a brush swirl)
                top = np.abs(rendered[-1][-1])
                slow = np.argmax(top > 0.3 * top.max()) > 0.025 * RATE
                if not top.max() > 1e-4 or slow or re.search(r"swirl|reverse", str(names), re.I):
                    print(f"  {family}: left out {preset_name} {gm} {names}: silent or slow to sound")
                    continue
                regions += rendered
                print(f"  {family}: {kind:9s} key {key:3d} <- {preset_name} {gm} {names} ({len(layers)} layers)")
                key += 1
        write_kit(os.path.join(out_root, family), family, regions)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("format", choices=("hydrogen", "drumlabooh", "soundfont"))
    parser.add_argument("source")
    parser.add_argument("out")
    parser.add_argument("presets", nargs="*", help="soundfont: <kit name>=<preset>[,<preset>...]")
    args = parser.parse_args()
    if args.format == "hydrogen":
        hydrogen(args.source, args.out)
    elif args.format == "drumlabooh":
        drumlabooh(args.source, args.out)
    else:
        families = {}
        for item in args.presets:
            name, numbers = item.split("=")
            families[name] = [int(n) for n in numbers.split(",")]
        soundfont(args.source, args.out, families)


if __name__ == "__main__":
    main()
