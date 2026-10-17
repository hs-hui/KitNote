# SPDX-License-Identifier: GPL-3.0-only
"""A sampled drum kit in the SFZ format, as render_kits.py plays it: the regions of the kit read (with #define,
#include, <control> default_path and the opcodes of <global>, <master> and <group> handed down), each key named
by the files of its samples (kick, snare, hi-hat closed / open / pedal, toms, crash, ride, bell) or by the kit's own
key map (kit_maps.py), and a stroke
played from the layer of its strength, a random one of its alternatives, every microphone the kit records with
mixed in at a gain of its own."""
import os
import random
import re

import numpy as np
import soundfile as sf

RATE = 44100
NOTE_NAMES = {"c": 0, "d": 2, "e": 4, "f": 5, "g": 7, "a": 9, "b": 11}

# what Groove MIDI's pitches are, as kinds of stroke
KIND_OF_PITCH = {36: "kick", 35: "kick", 38: "snare", 40: "rim", 37: "stick", 42: "hh_closed", 22: "hh_closed",
                 46: "hh_open", 26: "hh_open", 44: "hh_pedal", 48: "tom_high", 50: "tom_high", 45: "tom_mid", 47: "tom_mid",
                 43: "tom_low", 58: "tom_low", 41: "tom_low", 49: "crash", 55: "crash", 57: "crash", 52: "crash", 51: "ride", 59: "ride",
                 53: "bell"}
# a kind a kit lacks: the next best
FALLBACK = {"rim": ["snare"], "stick": ["rim", "snare"], "hh_pedal": ["hh_closed"], "hh_open": ["hh_closed"],
            "tom_mid": ["tom_high", "tom_low"], "tom_high": ["tom_mid", "tom_low"], "tom_low": ["tom_mid", "tom_high"],
            "bell": ["ride"], "ride": ["crash"], "crash": ["ride"]}


def num(value, default=0.0):
    """The number a value begins with (kits have typos as "0.66s")."""
    m = re.match(r"\s*(-?\d+(\.\d*)?|-?\.\d+)", str(value))
    return float(m.group(1)) if m else default


def note_number(value):
    value = value.strip().lower()
    if value.lstrip("-").isdigit():
        return int(value)
    m = re.match(r"([a-g])(#|b)?(-?\d+)", value)
    if not m:
        return None
    n = NOTE_NAMES[m.group(1)] + (1 if m.group(2) == "#" else -1 if m.group(2) == "b" else 0)
    return n + 12 * (int(m.group(3)) + 1)


def kind_of(path):
    """The kind of stroke a sample is, by its file name and folders."""
    p = path.lower().replace("\\", "/")
    name = os.path.basename(p)
    if re.search(r"cowbell|tamb|clap|shaker|jingle|tree|triangle|chime|agogo|conga|bongo|timbal|clave|block|guiro|"
                 r"cabasa|vibra|whistle|cuica|maraca|perc/", p):
        return None
    if "china" in p or "splash" in p:
        return "crash"
    if "bell" in p and ("ride" in p or "bell" in name):
        return "bell"
    if "ride" in p:
        return "ride"
    if "crash" in p or "cym" in name:
        return "crash"
    if re.search(r"hi_?hat|hh", name) or re.search(r"hi_?hat", p):
        if re.search(r"foot|pedal|chick", name):
            return "hh_pedal"
        if re.search(r"open|loose|half", name):
            return "hh_open"
        return "hh_closed"
    if "tom" in name or "/tom" in p:
        if re.search(r"floor|low|lo_?tom|ltom|tom_?3|tom_?4|ftom", name):
            return "tom_low"
        if re.search(r"high|hi_?tom|htom|tom_?1|rack", name):
            return "tom_high"
        return "tom_mid"
    if "snare" in p or re.search(r"(^|[_/])sn[_\d]", name):
        if re.search(r"side|cross|stick|click", name):
            return "stick"
        if "rim" in name:
            return "rim"
        return "snare"
    if "kick" in p or re.search(r"(^|[_/])(bd|kd)[_\d]", name) or "bassdrum" in p:
        return "kick"
    return None


def microphone(path):
    """The microphone family of a sample, by its folders and name: close, overhead, room or vintage."""
    p = path.lower().replace("\\", "/")
    if re.search(r"room|amb|(^|/)wet/", p):                 # (a "wet" sample: the room of it)
        return "room"
    if re.search(r"lofi|vintage", p):
        return "vintage"
    if re.search(r"(^|/|_)oh(/|_|\b)|overhead", p):
        return "overhead"
    if re.search(r"/mid/|(^|/)mid_", p):
        return "mid"
    return "close"


class SfzKit:
    def __init__(self, sfz_path, keymap=None, cc=None, offsets=False, skip=None):
        """keymap: the keys of each kind as the kit's own key map says, {kind: [key, ...]}, instead of guessing them
        by file names; cc: controllers held at a value, {number: value} (an open hi-hat pedal ...); offsets: start
        the samples where the kit's offset opcodes say (kits whose samples have a count-in before the stroke);
        skip: a pattern of the sample paths to leave out (a layer that is not the stroke itself)."""
        self.path = sfz_path
        self.name = os.path.splitext(os.path.basename(sfz_path))[0]
        self.base = os.path.dirname(sfz_path)
        self.regions = []
        self.cc = {}
        self.cc_held = dict(cc or {})
        self.offsets = offsets
        self.skip = skip
        self.rates = {}
        self._parse(self._expand(sfz_path, {}))
        self.audio = {}
        # the keys by kind: the kind most of a key's samples are
        by_key = {}
        for r in self.regions:
            by_key.setdefault(r["key"], []).append(r)
        self.kinds = {}
        for key, regions in by_key.items():
            kinds = [kind_of(r["sample"]) for r in regions]
            kinds = [k for k in kinds if k]
            if kinds:
                kind = max(set(kinds), key=kinds.count)
                self.kinds.setdefault(kind, []).append(key)
        if keymap:
            self.kinds = {kind: [k for k in keys if k in by_key] for kind, keys in keymap.items()}
            self.kinds = {kind: keys for kind, keys in self.kinds.items() if keys}
        self.by_key = by_key
        self.channels = ["close", "overhead", "room", "mid", "vintage"]
        self.instruments = {}
        print(f"{self.name}: {len(self.regions)} regions; " + ", ".join(f"{k}: {v}" for k, v in sorted(self.kinds.items())), flush=True)

    def _expand(self, path, defines):
        """The text of a file with its #include files put in and its #define names replaced."""
        text = open(path, encoding="utf-8", errors="ignore").read()
        text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
        out = []
        for line in text.splitlines():
            line = re.sub(r"//.*", "", line)
            m = re.match(r'\s*#define\s+(\$\w+)\s+(.+)', line)
            if m:
                defines[m.group(1)] = m.group(2).strip()
                continue
            m = re.match(r'\s*#include\s+"([^"]+)"', line)
            if m:
                inc = os.path.join(self.base, m.group(1).replace("\\", "/"))
                if os.path.exists(inc):
                    out.append(self._expand(inc, defines))
                continue
            for name in sorted(defines, key=len, reverse=True):
                line = line.replace(name, defines[name])
            out.append(line)
        return "\n".join(out)

    def _parse(self, text):
        tokens = list(re.finditer(r"<(\w+)>|([\w]+)=", text))
        levels = {"global": {}, "master": {}, "group": {}, "control": {}}
        current, header = None, None
        for i, m in enumerate(tokens):
            if m.group(1):
                if header == "region" and current is not None:
                    self._add_region(levels, current)
                header = m.group(1)
                if header in ("global", "master", "group", "control"):
                    levels[header] = {}
                    if header == "global":
                        levels["master"], levels["group"] = {}, {}
                    if header == "master":
                        levels["group"] = {}
                    current = levels[header]
                elif header == "region":
                    current = {}
                else:
                    current = {}
                continue
            end = tokens[i + 1].start() if i + 1 < len(tokens) else len(text)
            value = text[m.end():end].strip()
            if m.group(2) != "sample":
                value = value.split()[0] if value.split() else ""
            if current is not None:
                current[m.group(2)] = value
        if header == "region" and current is not None:
            self._add_region(levels, current)
        for k, v in levels["control"].items():
            m = re.match(r"set_(hd)?cc(\d+)$", k)
            if m:
                self.cc[int(m.group(2))] = num(v) * (127 if m.group(1) else 1)

    def _add_region(self, levels, region):
        op = {**levels["global"], **levels["master"], **levels["group"], **region}
        if "sample" not in op or op.get("trigger", "attack") != "attack":
            return
        if any(k.startswith("sw_") for k in op):
            return
        cc = dict(self.cc)
        for k, v in levels["control"].items():
            m = re.match(r"set_(hd)?cc(\d+)$", k)
            if m:
                cc[int(m.group(2))] = num(v) * (127 if m.group(1) else 1)
        cc.update(self.cc_held)
        offset = 0.0                                    # (where a sample starts: its count-in left out)
        if self.offsets:
            offset = num(op.get("offset", 0)) + sum(num(v) * cc.get(int(m.group(1)), 0.0) / 127
                                                    for m, v in ((re.match(r"offset_oncc(\d+)$", k), v) for k, v in op.items()) if m)
        for k, v in op.items():                         # conditions on controllers, at their starting values
            m = re.match(r"(lo|hi)cc(\d+)$", k)
            if m:
                value = cc.get(int(m.group(2)), 0.0)
                if (m.group(1) == "lo" and value < num(v)) or (m.group(1) == "hi" and value > num(v)):
                    return
        lo = note_number(op.get("lokey", op.get("key", "-1")))
        hi = note_number(op.get("hikey", op.get("key", "-1")))
        if lo is None or hi is None or lo < 0:
            return
        sample = (levels["control"].get("default_path", "") + op["sample"]).replace("\\", "/")
        if self.skip and re.search(self.skip, sample, re.I):
            return
        for key in range(lo, hi + 1):
            self.regions.append({
                "key": key, "sample": os.path.join(self.base, sample),
                "lovel": int(num(op.get("lovel", 1), 1)), "hivel": int(num(op.get("hivel", 127), 127)),
                "lorand": num(op.get("lorand", 0), 0.0), "hirand": num(op.get("hirand", 1), 1.0),
                "volume": num(op.get("volume", 0), 0.0), "amp_veltrack": num(op.get("amp_veltrack", 100), 100.0),
                "mic": microphone(sample), "group": op.get("group"), "off_by": op.get("off_by"), "offset": offset,
            })

    def read(self, path):
        if path not in self.audio:
            if not os.path.exists(path):
                self.audio[path] = None
            else:
                data, rate = sf.read(path, dtype="float32", always_2d=True)
                if rate != RATE:
                    from scipy.signal import resample_poly
                    import math
                    g = math.gcd(rate, RATE)
                    data = resample_poly(data, RATE // g, rate // g, axis=0).astype(np.float32)
                self.audio[path] = data.mean(axis=1)
        return self.audio[path]

    def group_of(self, pitch):
        return "hihat" if KIND_OF_PITCH.get(pitch, "").startswith("hh") else None

    def key_for(self, pitch, chosen):
        kind = KIND_OF_PITCH.get(pitch)
        for k in [kind] + FALLBACK.get(kind, []):
            if k in self.kinds:
                if (k, pitch) not in chosen:
                    chosen[(k, pitch)] = random.choice(self.kinds[k])
                return chosen[(k, pitch)]
        return None

    def stroke(self, pitch, velocity, gains, chosen):
        key = self.key_for(pitch, chosen)
        if key is None:
            return None, None
        r_value = random.random()
        out = None
        group = None
        mics = {}
        for r in self.by_key[key]:
            if r["lovel"] <= velocity <= r["hivel"] and r["lorand"] <= r_value < r["hirand"] + 1e-9:
                mics.setdefault(r["mic"], []).append(r)
        for mic, regions in mics.items():
            gain = gains.get(mic, 1.0)
            if gain <= 0:
                continue
            r = random.choice(regions)
            data = self.read(r["sample"])
            if data is None:
                continue
            if r["offset"] > 0:
                if r["sample"] not in self.rates:
                    self.rates[r["sample"]] = sf.info(r["sample"]).samplerate
                data = data[int(r["offset"] * RATE / self.rates[r["sample"]]):]
            vel_gain = 1 - r["amp_veltrack"] / 100 * (1 - (velocity / 127) ** 2)
            sound = data * (10 ** (r["volume"] / 20) * vel_gain * gain)
            if out is None:
                out = sound.copy()
            else:
                n = max(len(out), len(sound))
                out = np.pad(out, (0, n - len(out))) + np.pad(sound, (0, n - len(sound)))
            group = group or ("hihat" if KIND_OF_PITCH.get(pitch, "").startswith("hh") else None)
        return out, group
