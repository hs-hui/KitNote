#!/usr/bin/env python3
"""Checks how the audio engine (share/ai/kitnote_audio.py) writes strokes as a chart: one groove per passage,
weakly heard strokes off the groove left out, fills kept, the time kept on one thing (an open hi-hat the drum
model calls a cymbal is a hi-hat; a ride stays a ride), ghost notes measured against the backbeats.

The strokes are made up here, as engine 2's drum model would give them, so no model or recording is needed:

    "%LOCALAPPDATA%\\KitNote\\ai\\venv\\Scripts\\python.exe" tools/kitnote/test_audio_chart.py
"""
import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "share" / "ai"))
import kitnote_audio as ka  # noqa: E402

MEASURES = 8
STEP = 0.125                    # a sixteenth at 120 a minute
BEATS = np.arange(0, 4 * MEASURES + 1) * 4 * STEP


class Song:
    """Strokes, and what engine 2 measures of them, measure by measure (m from 0, steps of sixteenths)."""

    def __init__(self):
        self.strokes = []
        for table in (ka.HAT_RINGS, ka.METAL, ka.CYMBAL_RINGS, ka.CONFIDENCE):
            table.clear()
        ka.MODEL_ACTIVATIONS[:] = []

    def add(self, m, step, drum, sure=1.0, strength=0.0, hat_ring=None, bright=None, cymbal_ring=None):
        t = (m * 16 + step) * STEP + 1e-4 * len(self.strokes)      # (a time of its own for every stroke)
        self.strokes.append((t, drum, strength))
        ka.CONFIDENCE[(t, ka.SIGNS[drum][0])] = sure
        if hat_ring is not None:
            ka.HAT_RINGS[t] = hat_ring
        if bright is not None:
            ka.METAL[t] = (bright, cymbal_ring or 0.0)
        if cymbal_ring is not None:
            ka.CYMBAL_RINGS[t] = cymbal_ring

    def groove(self, m, metal=("HH",), hat_ring=0.01, bright=9500.0, cymbal_ring=None):
        """bass drum on 1 and 3, snare on 2 and 4, eighths on the metal given (each as the model heard it)"""
        for step in (0, 8):
            self.add(m, step, "BD")
        for step in (4, 12):
            self.add(m, step, "SD")
        for step in range(0, 16, 2):
            alone = step not in (0, 4, 8, 12)
            for drum in metal:
                self.add(m, step, drum, strength=-5.0, hat_ring=hat_ring,
                         bright=bright if alone else None, cymbal_ring=cymbal_ring if drum in ("CC", "RD") else None)

    def tab(self, ghost_notes=False):
        text, measures, _ = ka.to_tab(sorted(self.strokes), BEATS, MEASURES * 16 * STEP, "4/4", ghost_notes,
                                      first_downbeat=0.0)
        lines = {}
        for block in text.strip().split("\n\n"):
            for line in block.splitlines():
                label, body = line.split("|", 1)
                lines.setdefault(label, []).extend(cell for cell in body.split("|") if cell)
        return lines


def at(lines, line, m, step):
    return lines.get(line, ["-" * 16] * MEASURES)[m][step]


def test_weak_stray_stroke_is_left_out():
    for sure, written in ((0.1, False), (0.95, True)):
        song = Song()
        for m in range(MEASURES):
            song.groove(m)
        song.add(4, 6, "BD", sure=sure)
        assert (at(song.tab(), "BD", 4, 6) != "-") == written, sure


def test_fill_is_kept():
    song = Song()
    for m in range(MEASURES - 1):
        song.groove(m)
    for step in range(16):
        song.add(MEASURES - 1, step, "SD", sure=0.3)
    lines = song.tab()
    assert all(c == "o" for c in lines["SD"][MEASURES - 1]), lines["SD"][MEASURES - 1]


def test_figure_off_the_groove_is_kept():
    song = Song()
    for m in range(MEASURES):
        song.groove(m)
    for step in (13, 14, 15):
        song.add(5, step, "SD", sure=0.2)
    lines = song.tab()
    assert lines["SD"][5][13:] == "ooo", lines["SD"][5]


def test_open_hihat_called_a_cymbal_is_a_hihat():
    song = Song()
    for m in range(MEASURES):
        # the model hears every open hi-hat stroke as a hi-hat and as a cymbal; it rings on as a hi-hat does
        song.groove(m, metal=("HH", "RD"), hat_ring=0.8, cymbal_ring=0.1)
    lines = song.tab()
    assert "RD" not in lines and "CC" not in lines, lines
    assert all(cell == "o-o-o-o-o-o-o-o-" for cell in lines["HH"]), lines["HH"]


def test_ride_stays_a_ride():
    song = Song()
    for m in range(MEASURES):
        song.groove(m, metal=("RD",), hat_ring=0.12, bright=6500.0, cymbal_ring=0.02)
    # in one measure the model calls some of the ride's strokes a hi-hat, and tells little else
    song.strokes = [s for s in song.strokes if not (3 * 16 * STEP <= s[0] < 4 * 16 * STEP and s[1] == "RD")]
    for step in range(0, 16, 2):
        song.add(3, step, "HH" if step < 8 else "RD", cymbal_ring=None if step < 8 else 0.02)
    lines = song.tab()
    assert "HH" not in lines, lines.get("HH")
    assert all(cell == "x-x-x-x-x-x-x-x-" for cell in lines["RD"]), lines["RD"]


def test_ghost_notes_are_measured_against_the_backbeats():
    song = Song()
    for m in range(MEASURES):
        song.groove(m)
    song.add(2, 7, "SD", strength=-3.0)        # a little under the backbeats: a snare stroke
    song.add(5, 7, "SD", strength=-12.0)       # far under them: a ghost note
    lines = song.tab()
    assert at(lines, "SD", 2, 7) == "o" and at(lines, "SD", 5, 7) == "-"
    assert at(song.tab(ghost_notes=True), "SD", 5, 7) == "g"


def test_loud_fill_does_not_make_the_backbeats_ghost_notes():
    song = Song()
    for m in range(MEASURES - 1):
        song.groove(m)
    for step in range(16):
        song.add(MEASURES - 1, step, "SD", strength=7.0)     # a fill of accents, far louder than the backbeats
    lines = song.tab()
    assert all(cell[4] == "o" and cell[12] == "o" for cell in lines["SD"][:MEASURES - 1]), lines["SD"]


def test_closed_hihat_in_a_noisy_recording_stays_closed():
    """A drums-only recording made in a room, or with a phone, hisses: a closed hi-hat dies away into the hiss,
    not into silence, and is not to be measured as one that rings on. An open one still rings. (The hi-hat
    strokes as the drum model would hear them; the ring measured on the sound.)"""
    rng = np.random.default_rng(7)
    n = int(4.0 * ka.RATE)
    hit = np.arange(int(0.6 * ka.RATE))
    highpass = ka.signal.butter(4, 8000, "highpass", fs=ka.RATE, output="sos")
    for decay, every, kind in ((0.012, 0.25, "HH"), (0.5, 0.75, "HO")):
        audio = rng.normal(0.0, 0.01, n)                  # the hiss, about 40 dB under full scale
        # (a hi-hat played softly, its highs a few dB over the hiss)
        burst = ka.signal.sosfilt(highpass, rng.normal(0.0, 0.015, len(hit))) * np.exp(-hit / (decay * ka.RATE))
        times = np.arange(0.5, 4.0 - 0.6, every)
        for t in times:
            k = int(t * ka.RATE)
            audio[k:k + len(hit)] += burst
        activations = np.zeros((int(4.0 * ka.MODEL_FPS), len(ka.DRUM_CLASSES)), dtype=np.float32)
        for t in times:
            activations[int(round(t * ka.MODEL_FPS)), ka.DRUM_CLASSES.index("HH")] = 1.0
        strokes = ka.model_strokes(activations, np.stack([audio, audio]).astype(np.float32))
        hats = [drum for _, drum, _ in strokes if drum in ("HH", "HO")]
        assert len(hats) == len(times) and all(drum == kind for drum in hats), (kind, hats, list(ka.HAT_RINGS.values())[:4])


def grooved(beat_times, pattern):
    """Strokes as engine 2 gives them: for every beat (index k from 0), pattern(k) -> [(place in the beat, drum)]"""
    strokes = []
    for k, (a, b) in enumerate(zip(beat_times, beat_times[1:])):
        for place, drum in pattern(k):
            strokes.append((a + place * (b - a), drum, 0.0))
    return sorted(strokes)


def test_slow_six_eight_counted_in_eighths_is_six_eight():
    """A beat model counts the eighths of a slow 6/8 (138 a minute) with a downbeat every three or six of them:
    the bass drum and the snare on every third eighth make it a 6/8 of dotted quarters, not a 4/4."""
    eighths = np.arange(0, 6 * 24 + 1) * 0.435
    downbeats = np.concatenate([eighths[0:36:3], eighths[36::6]])

    def six_eight(k):
        return [(0.0, "HH")] + ([(0.0, "BD")] if k % 6 == 0 else [(0.0, "SD")] if k % 6 == 3 else [])
    strokes = grooved(eighths, six_eight)
    meter, beats, _ = ka.find_meter(eighths, downbeats, strokes)
    assert meter == "6/8", meter
    assert abs(float(np.median(np.diff(beats))) - 3 * 0.435) < 0.01, np.diff(beats)[:4]
    assert round(beats[0] / 0.435) % 3 == 0, beats[:3]


def test_dotted_quarters_with_every_eighth_are_compound():
    """Dotted quarters, the hi-hat on all three of their eighths: 6/8 (two to the measure) or 12/8 (four)."""
    for per_bar, wanted in ((2, "6/8"), (4, "12/8")):
        beats = np.arange(0, 4 * 16 + 1) * 1.0
        downbeats = beats[::per_bar]

        def compound(k):
            return [(0.0, "HH"), (1 / 3, "HH"), (2 / 3, "HH"), (0.0, "BD" if k % 2 == 0 else "SD")]
        meter, _, _ = ka.find_meter(beats, downbeats, grooved(beats, compound))
        assert meter == wanted, (per_bar, meter)


def test_shuffle_stays_in_four_four():
    """A shuffle divides the beat in three as well, but long-short: the middle third is not struck. 4/4."""
    for beat_len in (0.6, 0.83, 0.43):
        beats = np.arange(0, 4 * 16 + 1) * beat_len
        downbeats = beats[::4]

        def shuffle(k):
            return [(0.0, "HH"), (2 / 3, "HH"), (0.0, "BD" if k % 2 == 0 else "SD")] + ([(2 / 3, "BD")] if k % 4 == 1 else [])
        meter, _, _ = ka.find_meter(beats, downbeats, grooved(beats, shuffle))
        assert meter == "4/4", (beat_len, meter)


def test_waltz_stays_in_three_four():
    """Quarter notes three to the measure, the snare on two and three: 3/4, also when they are fast."""
    for beat_len in (0.625, 0.42, 0.375):
        beats = np.arange(0, 3 * 24 + 1) * beat_len
        downbeats = beats[::3]

        def waltz(k):
            return [(0.0, "HH"), (0.0, "BD" if k % 3 == 0 else "SD")]
        meter, _, _ = ka.find_meter(beats, downbeats, grooved(beats, waltz))
        assert meter == "3/4", (beat_len, meter)


def test_straight_four_four_stays():
    beats = np.arange(0, 4 * 16 + 1) * 0.5
    downbeats = beats[::4]

    def rock(k):
        return [(0.0, "HH"), (0.5, "HH"), (0.0, "BD" if k % 2 == 0 else "SD")]
    meter, _, _ = ka.find_meter(beats, downbeats, grooved(beats, rock))
    assert meter == "4/4", meter


def test_a_beat_heard_twice_or_missed_is_mended():
    beats = list(np.arange(0, 40) * 0.5)
    extra = sorted(beats + [5.17])                  # heard twice: a third of a beat after one
    assert len(ka.clean_beats(extra)) == 40
    missed = beats[:20] + beats[21:]                # one missed
    mended = ka.clean_beats(missed)
    assert len(mended) == 40 and abs(mended[20] - 10.0) < 1e-6
    # a sudden change of tempo is no beat too many or too few
    change = list(np.arange(0, 20) * 0.6) + list(19 * 0.6 + np.arange(1, 21) * 0.42)
    assert len(ka.clean_beats(change)) == 40


def tempo_change_song(meter, beat_lengths):
    """Beats (quarters, or dotted quarters) whose length changes suddenly, strokes on them, and where each measure
    begins: (strokes, beats, measure starts, duration)"""
    per_measure, compound = ka.METERS[meter]
    beats = np.concatenate([[0.0], np.cumsum(beat_lengths)])
    if compound:
        def pattern(k):
            return [(0.0, "HH"), (1 / 3, "HH"), (2 / 3, "HH"), (0.0, "BD" if k % 2 == 0 else "SD")]
    else:
        def pattern(k):
            return [(0.0, "HH"), (0.5, "HH"), (0.0, "BD" if k % 2 == 0 else "SD")]
    strokes = grooved(beats, pattern)
    for table in (ka.HAT_RINGS, ka.METAL, ka.CYMBAL_RINGS, ka.CONFIDENCE):
        table.clear()
    ka.MODEL_ACTIVATIONS[:] = []
    return strokes, beats, beats[::per_measure], beats[-1]


def test_measure_times_follow_sudden_changes_of_tempo():
    """Where the measures begin (KitNote's playback follows them) stays right where the tempo changes suddenly:
    a line through the strokes on both sides of the change fits a few of them only, and is not taken."""
    for meter, lengths in (("6/8", [1.2] * 16 + [0.833] * 16 + [1.07] * 16),
                           ("4/4", [0.667] * 32 + [0.476] * 32 + [0.6] * 32)):
        strokes, beats, starts, duration = tempo_change_song(meter, lengths)
        times = []
        ka.to_tab(strokes, beats, duration + 0.5, meter, first_downbeat=0.0, measure_times=times)
        error = max(abs(t - starts[np.argmin(np.abs(starts - t))]) for t in times[:len(starts)])
        assert error < 0.01, (meter, error, times[:6])


def tom_song(pitches, seconds=24.0, step=0.15, kick_every=0, snare_with=()):
    """Drums alone: fills down the toms (each tom of pitches - Hz, high to low - struck twice in sixteenths,
    the fill every two seconds), every tom stroke a ringing drum with its overtones, its pitch gliding down a
    little; the bass drum with every kick_every-th tom stroke (0: never), a snare with the tom strokes whose
    index is in snare_with. (strokes as engine 2 gives them, the truth: [(time, tom index)])"""
    rng = np.random.default_rng(len(pitches))
    n = int(seconds * ka.RATE)
    audio = rng.normal(0.0, 0.0005, n)
    hit = np.arange(int(0.5 * ka.RATE)) / ka.RATE
    activations = np.zeros((int(seconds * ka.MODEL_FPS) + 1, len(ka.DRUM_CLASSES)), dtype=np.float32)
    truth = []
    k = 0
    for start in np.arange(0.5, seconds - 2.0, 2.0):
        for i, pitch in enumerate(pitches):
            for again in range(2):
                at = start + (2 * i + again) * step
                glide = pitch * (1.0 + 0.04 * np.exp(-hit / 0.05))
                phase = 2 * np.pi * np.cumsum(glide) / ka.RATE
                sound = (np.sin(phase) + 0.45 * np.sin(1.59 * phase + 1.0) + 0.3 * np.sin(2.14 * phase + 2.0)) \
                    * np.exp(-hit / 0.25) * rng.uniform(0.6, 1.0)
                sound[:int(0.004 * ka.RATE)] += rng.normal(0.0, 0.3, int(0.004 * ka.RATE))
                drums = [("TT", at)]
                if kick_every and k % kick_every == 0:
                    sound += 0.8 * np.sin(2 * np.pi * 55 * hit * (1 + 0.5 * np.exp(-hit / 0.02))) * np.exp(-hit / 0.12)
                    drums.append(("BD", at))
                if k in snare_with:
                    sound += 0.5 * np.sin(2 * np.pi * 200 * hit) * np.exp(-hit / 0.06) \
                        + rng.normal(0.0, 0.2, len(hit)) * np.exp(-hit / 0.08)
                    drums.append(("SD", at))
                j = int(at * ka.RATE)
                audio[j:j + len(hit)] += 0.3 * sound[:n - j]
                for name, when in drums:
                    activations[int(round(when * ka.MODEL_FPS)), ka.DRUM_CLASSES.index(name)] = 0.9
                truth.append((round(at, 2), i))
                k += 1
    audio = np.stack([audio, audio]).astype(np.float32)
    for table in (ka.HAT_RINGS, ka.METAL, ka.CYMBAL_RINGS, ka.CONFIDENCE):
        table.clear()
    return ka.model_strokes(activations, audio), truth


def tom_lines(strokes):
    return {round(time, 2): drum for time, drum, _ in strokes if drum in ("T1", "T2", "FT")}


def test_toms_are_named_by_their_pitch():
    """Each tom stroke goes to the drum of its pitch, the drums named from the highest: three toms are T1 T2 FT,
    two the rack tom and the floor tom (T1 FT), four T1 T2 FT FT - whatever their tuning - and a fill down the
    toms never goes up the lines (the old engine called most of them mid toms)."""
    for pitches, lines in (((150.0, 112.0, 82.0), ("T1", "T2", "FT")),
                           ((210.0, 160.0, 120.0), ("T1", "T2", "FT")),
                           ((128.0, 74.0), ("T1", "FT")),
                           ((180.0, 150.0, 115.0, 88.0), ("T1", "T2", "FT", "FT"))):
        strokes, truth = tom_song(pitches)
        found = tom_lines(strokes)
        wrong = [(at, lines[i], found.get(at)) for at, i in truth if found.get(at) != lines[i]]
        # (a stroke struck while the tom before rings on as loudly may go to that one now and then)
        assert len(wrong) <= len(truth) // 20, (pitches, wrong[:6])
        down = sum(1 for (a, i), (b, j) in zip(truth, truth[1:]) if j > i and b - a < 0.2
                   and ("T1", "T2", "FT").index(found.get(b, "T1")) < ("T1", "T2", "FT").index(found.get(a, "T1")))
        assert down == 0, (pitches, "a fill going up the toms", down)


def test_tom_struck_with_the_bass_drum_or_the_snare_keeps_its_drum():
    """A tom struck with the bass drum (whose boom covers a floor tom's pitch) or with the snare (whose own
    pitch may be the strongest then) is named by what sounds at the drums' pitches."""
    strokes, truth = tom_song((150.0, 112.0, 82.0), kick_every=3, snare_with=set(range(4, 200, 7)))
    found = tom_lines(strokes)
    lines = ("T1", "T2", "FT")
    wrong = [(at, lines[i], found.get(at)) for at, i in truth if found.get(at) != lines[i]]
    assert len(wrong) <= len(truth) // 20, wrong[:6]


def test_slow_sixteenths_counted_in_eighths_are_halved():
    """A ballad at 70 with the hi-hat in sixteenths, counted at 140 by the beat model: the snare comes every
    four of its beats - the backbeat at half the speed. 70, also when the beat model missed a beat here and
    there. Fast punk at 180 in eighths, a half-time chorus in a song on two and four, and sixteenths at 140
    (thirty-second notes at 70) keep the tempo found."""
    beat = 60.0 / 140.0
    beats = np.arange(0, 4 * 32 + 1) * beat

    def ballad(k):
        return [(0.0, "HH"), (0.5, "HH")] + ([(0.0, "SD")] if k % 4 == 2 else [(0.0, "BD")] if k % 4 == 0 else [])
    strokes = grooved(beats, ballad)
    assert abs(ka.half_tempo(beats, strokes) - 70.0) < 1.0
    missed = np.delete(beats, [13, 50, 51, 90])
    assert abs(ka.half_tempo(missed, strokes) - 70.0) < 1.0

    fast = np.arange(0, 4 * 48 + 1) * (60.0 / 180.0)

    def punk(k):
        return [(0.0, "HH"), (0.5, "HH"), (0.0, "SD" if k % 2 else "BD")]
    assert ka.half_tempo(fast, grooved(fast, punk)) is None

    def chorus(k):
        if (k // 16) % 2:
            return ballad(k)        # every other passage of four measures in half time
        return [(0.0, "HH"), (0.5, "HH"), (0.0, "SD" if k % 2 else "BD")]
    assert ka.half_tempo(beats, grooved(beats, chorus)) is None

    def sixteenths(k):
        return [(0.0, "HH"), (0.25, "HH"), (0.5, "HH"), (0.75, "HH")] + ballad(k)[2:]
    assert ka.half_tempo(beats, grooved(beats, sixteenths)) is None


def test_fast_punk_counted_in_halves_is_doubled():
    """Punk at 180 (bass drum on one and three, snare on two and four) counted at 90: the snare falls on the
    "and" of every beat - 180. A groove with the snare on every beat (Motown, at 100) stays as it is."""
    beats = np.arange(0, 4 * 24 + 1) * (60.0 / 90.0)

    def punk(k):
        return [(0.0, "HH"), (0.25, "HH"), (0.5, "HH"), (0.75, "HH"), (0.0, "BD"), (0.5, "SD")]
    assert abs(ka.double_tempo(beats, grooved(beats, punk)) - 180.0) < 1.0
    motown = np.arange(0, 4 * 24 + 1) * 0.6

    def backbeats(k):
        return [(0.0, "HH"), (0.5, "HH"), (0.0, "SD")] + ([(0.0, "BD")] if k % 2 == 0 else [])
    assert ka.double_tempo(motown, grooved(motown, backbeats)) is None


def test_measures_the_beat_model_did_not_hear_come_from_the_groove():
    """Downbeats every two beats (the beat model often marks them so in drums alone): 4/4, not 2/4; downbeats
    that do not agree: the groove says how many beats a measure has."""
    beats = np.arange(0, 4 * 16 + 1) * 0.5

    def rock(k):
        return [(0.0, "HH"), (0.5, "HH"), (0.0, "BD" if k % 2 == 0 else "SD")] + ([(0.5, "BD")] if k % 4 == 2 else [])
    assert ka.find_meter(beats, beats[::2], grooved(beats, rock))[0] == "4/4"
    waltz_beats = np.arange(0, 3 * 24 + 1) * 0.45
    scattered = waltz_beats[np.cumsum([0, 3, 1, 2, 3, 1, 3, 2, 1, 3, 3, 1, 2, 3, 1, 3, 2, 1, 3, 2])]

    def waltz(k):
        return [(0.0, "HH"), (0.5, "HH"), (0.0, "BD" if k % 3 == 0 else "SD")]
    assert ka.find_meter(waltz_beats, scattered, grooved(waltz_beats, waltz))[0] == "3/4"


def test_hum_is_no_floor_tom():
    """A drums-only recording with mains hum (60 Hz and its harmonics, all through it): a stroke the drum model
    takes for a tom where only the hum sounds low is left out - the hum would make it a floor tom - while a
    real floor tom, tuned to the hum's own pitch even, is kept."""
    rng = np.random.default_rng(3)
    n = int(6.0 * ka.RATE)
    t = np.arange(n) / ka.RATE
    audio = 0.02 * (np.sin(2 * np.pi * 60 * t) + 0.5 * np.sin(2 * np.pi * 120 * t + 1) + 0.3 * np.sin(2 * np.pi * 180 * t + 2))
    highpass = ka.signal.butter(4, 7000, "highpass", fs=ka.RATE, output="sos")
    hit = np.arange(int(0.3 * ka.RATE))
    hats = np.arange(0.5, 5.5, 0.25)
    for at in hats:
        k = int(at * ka.RATE)
        audio[k:k + len(hit)] += ka.signal.sosfilt(highpass, rng.normal(0.0, 0.2, len(hit))) * np.exp(-hit / (0.03 * ka.RATE))
    toms = (1.62, 3.62)
    for at in toms:
        k = int(at * ka.RATE)
        audio[k:k + len(hit)] += 0.1 * np.sin(2 * np.pi * 62 * hit / ka.RATE) * np.exp(-hit / (0.25 * ka.RATE))
    activations = np.zeros((int(6.0 * ka.MODEL_FPS), len(ka.DRUM_CLASSES)), dtype=np.float32)
    for at in list(hats[::3]) + list(toms):     # the model hears every third hat as a tom too
        activations[int(round(at * ka.MODEL_FPS)), ka.DRUM_CLASSES.index("TT")] = 0.9
    strokes = ka.model_strokes(activations, np.stack([audio, audio]).astype(np.float32))
    found = sorted(round(time, 2) for time, drum, _ in strokes if drum in ("T1", "T2", "FT"))
    assert found == list(toms), found


def test_mains_hum_is_taken_out():
    """The hum of the mains is found (50 or 60 Hz, and the harmonics that stand out) and notched out before the
    drum model listens; a recording without one - drums ringing at their own pitches - is left as it is."""
    rng = np.random.default_rng(5)
    n = int(12.0 * ka.RATE)
    t = np.arange(n) / ka.RATE
    drums = np.zeros(n)
    hit = np.arange(int(0.4 * ka.RATE))
    for at in [x for x in np.arange(0.3, 11.5, 0.5) if not 4.0 <= x < 6.5]:      # (a break: two seconds of hum)
        k = int(at * ka.RATE)
        pitch = (62.0, 98.0, 180.0)[int(at * 2) % 3]           # a floor tom tuned near the hum too
        drums[k:k + len(hit)] += 0.3 * np.sin(2 * np.pi * pitch * hit / ka.RATE) * np.exp(-hit / (0.15 * ka.RATE))
    drums += rng.normal(0.0, 0.001, n)
    for mains in (60.0, 50.0, 50.13):
        hum = 0.01 * (np.sin(2 * np.pi * mains * t) + 0.6 * np.sin(2 * np.pi * 2 * mains * t + 1.0))
        _, found = ka.remove_hum(np.stack([drums + hum, drums + hum]).astype(np.float32))
        # found under the drums, to a few hundredths of a hertz (the notch is half a hertz wide)
        assert [round(f / mains) for f in found][:2] == [1, 2], (mains, found)
        assert all(abs(f / round(f / mains) - mains) < 0.03 for f in found), (mains, found)
        # and taken out
        cleaned, _ = ka.remove_hum(np.stack([hum, hum]).astype(np.float32))
        middle = slice(int(2 * ka.RATE), int(10 * ka.RATE))
        assert np.sqrt(np.mean(cleaned[0, middle] ** 2)) < 0.01 * np.sqrt(np.mean(hum[middle] ** 2)), mains
    assert ka.remove_hum(np.stack([drums, drums]).astype(np.float32))[1] == []


if __name__ == "__main__":
    failed = 0
    for name, test in [(n, f) for n, f in sorted(globals().items()) if n.startswith("test_")]:
        try:
            test()
            print("ok  ", name)
        except AssertionError as error:
            failed += 1
            print("FAIL", name, error)
    sys.exit(1 if failed else 0)
