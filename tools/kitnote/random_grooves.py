# SPDX-License-Identifier: GPL-3.0-only
"""Makes drum performances of KitNote's own, as MIDI: grooves of many styles - rock, pop, funk, disco and four
on the floor, half time, ballads, hip hop, reggae, drum and bass, train beats, latin, metal and blast beats, tom
grooves, shuffles, 6/8 and 12/8, jazz, waltzes, 5/4 and 7/8 - with fills, flams, crashes and open hi-hats, at
many tempos and strengths. They give KitNote's drum model what the Groove MIDI Dataset has little of: drums
struck together in every combination (a snare on a bass drum, a crash on both, a tom with the hi-hat).

    python random_grooves.py --out <folder> [--count 3000] [--seed 1]

Writes <out>/<n>.mid (General MIDI drums, the pitches of the Groove MIDI Dataset) and <out>/grooves.csv with
the columns render_kits.py reads (midi_filename, split, duration).
"""
import argparse
import csv
import os
import random

import mido

KICK, SNARE, RIM, STICK = 36, 38, 40, 37
HH, HH_OPEN, HH_PEDAL = 42, 46, 44
TOM_HIGH, TOM_HIGH2, TOM_MID, TOM_LOW, TOM_FLOOR = 50, 48, 45, 43, 41
CRASH, CRASH2, RIDE, BELL, CHINA, SPLASH = 49, 57, 51, 53, 52, 55

# styles: for each step of a bar (16ths, or triplet eighths where a beat has 3 steps), the drums struck:
# x always, g a ghost note, ? at random, o an open hi-hat. "beat": steps in a beat; "bpm": the range of tempos;
# "keepers": the drums that may keep time; "snare": the strokes the snare part is played with; "pedal": a
# hi-hat foot pattern; "dense": how often a ? is struck.
STYLES = {
    "rock":      {KICK: "x...x...x.x.....", SNARE: "....x.......x...", "time": "x.x.x.x.x.x.x.x."},
    "rock16":    {KICK: "x.....x.x.x.....", SNARE: "....x.......x...", "time": "xxxxxxxxxxxxxxxx"},
    "pop":       {KICK: "x..x..x.x.......", SNARE: "....x.......x...", "time": "x.x.x.x.x.x.x.x."},
    "funk":      {KICK: "x..x..x...x..x..", SNARE: "....x..g.g..x..g", "time": "xxxxxxxxxxxxxxxx"},
    "disco":     {KICK: "x...x...x...x...", SNARE: "....x.......x...", "time": "x.o.x.o.x.o.x.o."},
    "four":      {KICK: "x...x...x...x...", SNARE: "....x.......x...", "time": "x.x.x.x.x.x.x.x."},
    "motown":    {KICK: "x.....x...x.....", SNARE: "x...x...x...x...", "time": "x.x.x.x.x.x.x.x."},
    "halftime":  {KICK: "x.....x...x.....", SNARE: "........x.......", "time": "x.x.x.x.x.x.x.x.", "bpm": (60, 120)},
    "ballad":    {KICK: "x.......x.x.....", SNARE: "....x.......x...", "time": "xxxxxxxxxxxxxxxx", "bpm": (55, 85),
                  "snare": [SNARE, STICK]},
    "hiphop":    {KICK: "x.....x..x......", SNARE: "....x.......x...", "time": "x.x.x.x.x.x.x.x.", "bpm": (75, 100)},
    "reggae":    {KICK: "........x.......", SNARE: "........x.......", "time": "x.x.x.x.x.x.x.x.", "bpm": (60, 90),
                  "snare": [STICK, RIM]},
    "dnb":       {KICK: "x.........x.....", SNARE: "....x.......x...", "time": "x.x.x.x.x.x.x.x.", "bpm": (160, 180)},
    "train":     {KICK: "x.x.x.x.x.x.x.x.", SNARE: "gggxgggxgggxgggx", "time": "................", "bpm": (100, 160)},
    "latin":     {KICK: "x..x..x.x..x..x.", SNARE: "..x..x....x..x..", "time": "x.xxx.xxx.xxx.xx"},
    "metal":     {KICK: "xxxxxxxxxxxxxxxx", SNARE: "....x.......x...", "time": "x.x.x.x.x.x.x.x.",
                  "keepers": [HH, RIDE, CRASH, CHINA]},
    "blast":     {KICK: "x.x.x.x.x.x.x.x.", SNARE: "x.x.x.x.x.x.x.x.", "time": "x.x.x.x.x.x.x.x.", "bpm": (150, 230),
                  "keepers": [RIDE, CRASH, CHINA, HH]},
    "punk":      {KICK: "x.x.x.x.x.x.x.x.", SNARE: "..x...x...x...x.", "time": "x.x.x.x.x.x.x.x."},
    "toms":      {KICK: "x...x...x...x...", SNARE: "....x.......x...", "time": "x.xxx.xxx.xxx.xx",
                  "keepers": [TOM_LOW, TOM_FLOOR, TOM_MID]},
    "dense":     {KICK: "?.?.?.?.?.?.?.?.", SNARE: "?.?.?.?.?.?.?.?.", "time": "????????????????"},
    "shuffle":   {"beat": 3, KICK: "x.....x.....", SNARE: "...x.....x..", "time": "x.xx.xx.xx.x", "bpm": (60, 140)},
    "twelve8":   {"beat": 3, KICK: "x.....x..x..", SNARE: "...x.....x..", "time": "xxxxxxxxxxxx", "bpm": (50, 90)},
    "six8":      {"beat": 3, KICK: "x.....", SNARE: "...x..", "time": "xxxxxx", "bpm": (40, 80)},
    "jazz":      {"beat": 3, KICK: "?...........", SNARE: "..?..?..?..?", "time": "x..x.xx..x.x", "bpm": (90, 220),
                  "keepers": [RIDE], "pedal": "...x.....x..", "dense": 0.15},
    "waltz":     {KICK: "x...........", SNARE: "....x...x...", "time": "x.x.x.x.x.x."},
    "five4":     {KICK: "x.......x...x.......", SNARE: "....x.......x.....x.", "time": "x.x.x.x.x.x.x.x.x.x."},
    "seven8":    {KICK: "x...x...x.....", SNARE: "....x.....x...", "time": "x.x.x.x.x.x.x."},
}


def make(seed):
    """One performance: a style, a tempo, a time keeper, 8 to 24 bars with variations, fills and crashes."""
    rnd = random.Random(seed)
    name = rnd.choice(list(STYLES))
    style = STYLES[name]
    steps = len(style["time"])
    beat = style.get("beat", 4)
    bpm = rnd.uniform(*style.get("bpm", (70, 200)))
    step_len = 60.0 / bpm / beat
    keeper = rnd.choice(style.get("keepers", [HH, HH, HH, RIDE, HH_OPEN, CRASH]))
    snare = rnd.choice(style.get("snare", [SNARE, SNARE, SNARE, RIM]))
    swing = rnd.uniform(0.0, 0.25) if beat == 4 and rnd.random() < 0.2 else 0.0
    notes = []
    bars = rnd.randint(8, 24)
    dense = {d: style.get("dense", rnd.uniform(0.2, 0.7)) for d in (KICK, SNARE, "time")}
    toms = rnd.sample([TOM_HIGH, TOM_HIGH2, TOM_MID, TOM_LOW, TOM_FLOOR], rnd.randint(2, 4))

    def hit(t, pitch, velocity):
        notes.append((t + rnd.gauss(0, 0.004), pitch, int(min(127, max(10, velocity + rnd.gauss(0, 8))))))

    for bar in range(bars):
        fill = (bar % 4 == 3 and rnd.random() < 0.6) or bar == bars - 1
        fill_from = steps - rnd.choice([steps // 4, steps // 2, steps]) if fill else steps
        for s in range(steps):
            t = (bar * steps + s) * step_len
            if beat == 4 and s % 2 == 1:
                t += swing * step_len
            accent = 1.0 if s % beat == 0 else 0.85
            if s >= fill_from:
                # a fill: snare, toms, sometimes with the bass drum, sometimes a flam-like pair
                for _ in range(rnd.choice([1, 1, 1, 2])):
                    drum = rnd.choice([SNARE, SNARE] + toms)
                    if rnd.random() < 0.1:
                        hit(t - 0.025, drum, rnd.randint(30, 60))   # a flam
                    hit(t, drum, rnd.randint(70, 120))
                if rnd.random() < 0.3:
                    hit(t, KICK, rnd.randint(70, 115))
                continue
            for drum in (KICK, SNARE):
                c = style[drum][s]
                pitch = snare if drum == SNARE else KICK
                if c == "x" or (c == "?" and rnd.random() < dense[drum]) or (c == "." and rnd.random() < 0.06):
                    hit(t, pitch, 110 * accent)
                elif c == "g" or (drum == SNARE and c == "." and rnd.random() < 0.08):
                    hit(t, SNARE, rnd.randint(20, 50))                # a ghost note
            c = style["time"][s]
            if c in "xo" or (c == "?" and rnd.random() < dense["time"]):
                pitch = keeper
                if keeper == HH and (c == "o" or rnd.random() < 0.06):
                    pitch = HH_OPEN
                if keeper == RIDE and rnd.random() < 0.08:
                    pitch = BELL
                hit(t, pitch, rnd.randint(55, 105) * accent)
            if "pedal" in style and style["pedal"][s] == "x":
                hit(t, HH_PEDAL, rnd.randint(50, 90))
            if rnd.random() < 0.03:
                hit(t, rnd.choice([RIM, STICK, HH_PEDAL, rnd.choice(toms), SPLASH, BELL]), rnd.randint(40, 100))
        # a crash on the next downbeat after a fill, with the bass drum; sometimes on any bar
        if fill or rnd.random() < 0.15:
            t = (bar + 1) * steps * step_len
            if bar < bars - 1 or rnd.random() < 0.7:
                hit(t, rnd.choice([CRASH, CRASH, CRASH2, CRASH2, CHINA, SPLASH]), rnd.randint(90, 127))
                hit(t, KICK, rnd.randint(90, 120))
    if "pedal" not in style and rnd.random() < 0.3:   # a hi-hat foot keeping time under a ride
        for q in range(bars * steps // beat):
            if keeper != HH and q % 2 == 1:
                hit(q * beat * step_len, HH_PEDAL, rnd.randint(50, 90))
    notes.sort()
    return name, notes, bars * steps * step_len + 2.0


def write(notes, path):
    midi = mido.MidiFile(ticks_per_beat=480)
    track = mido.MidiTrack()
    midi.tracks.append(track)
    track.append(mido.MetaMessage("set_tempo", tempo=500000, time=0))
    events = []
    for t, p, v in notes:
        t = max(0.0, t)
        events.append((int(round(t * 960)), 1, p, v))
        events.append((int(round((t + 0.05) * 960)), 0, p, 0))
    last = 0
    for tick, on, p, v in sorted(events):
        track.append(mido.Message("note_on", note=p, velocity=v if on else 0, channel=9, time=tick - last))
        last = tick
    midi.save(path)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("--out", required=True)
    parser.add_argument("--count", type=int, default=3000)
    parser.add_argument("--seed", type=int, default=1)
    args = parser.parse_args()
    os.makedirs(args.out, exist_ok=True)
    rows = []
    for n in range(args.count):
        name, notes, duration = make(args.seed * 1000003 + n)
        midi = f"{n}.mid"
        write(notes, os.path.join(args.out, midi))
        rows.append({"midi_filename": midi, "audio_filename": "", "split": "validation" if n % 20 == 0 else "train",
                     "duration": round(duration, 2), "style": name})
    with open(os.path.join(args.out, "grooves.csv"), "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)
    print(f"{len(rows)} performances, {sum(r['duration'] for r in rows) / 3600:.1f} h")


if __name__ == "__main__":
    main()
