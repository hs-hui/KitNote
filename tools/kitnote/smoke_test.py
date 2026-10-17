#!/usr/bin/env python3
"""Checks an installed KitNote build from the command line.

Covers what KitNote changes relative to MuseScore Studio: its version, its own
file extension, the refusal of scores with pitched instruments, the import and
export formats that are still built, the bundled templates and user guide, the
drum commands (patterns, rudiments, sticking, counting, repeat signs, drum key,
swing), the softer playback
of ghost notes, the hi-hat sound chosen by the open sign, and the recording a
score plays along with (attached, found where measure 1 begins, played in time,
following the timing of a recording whose tempo moves).

    python tools/kitnote/smoke_test.py [path\\to\\KitNote.exe]
"""
import json
import os
import re
import struct
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXE = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "build.install" / "bin" / "KitNote.exe"

HEADER = ('<?xml version="1.0" encoding="UTF-8"?>\n<score-partwise version="4.0"><part-list><score-part id="P1">'
          '<part-name>{name}</part-name>{instruments}</score-part></part-list><part id="P1"><measure number="1">'
          '<attributes><divisions>1</divisions><time><beats>4</beats><beat-type>4</beat-type></time>{clef}</attributes>')


def drums(notehead: str = "") -> str:
    """One measure of four snare quarter notes."""
    header = HEADER.format(
        name="Drum Kit",
        instruments='<score-instrument id="P1-I39"><instrument-name>Snare</instrument-name></score-instrument>'
                    '<midi-instrument id="P1-I39"><midi-channel>10</midi-channel><midi-unpitched>39</midi-unpitched></midi-instrument>',
        clef="<clef><sign>percussion</sign></clef>",
    )
    note = ('<note><unpitched><display-step>C</display-step><display-octave>5</display-octave></unpitched><duration>1</duration>'
            f'<instrument id="P1-I39"/><voice>1</voice><type>quarter</type>{notehead}</note>')
    return header + note * 4 + "</measure></part></score-partwise>"


DRUMS = drums()
GHOSTS = drums('<notehead parentheses="yes">normal</notehead>')


def hi_hat(open_instrument: bool, sign: str = "") -> str:
    """One measure of four hi-hat quarter notes, closed or open, optionally with a technical sign."""
    header = HEADER.format(
        name="Drum Kit",
        instruments='<score-instrument id="P1-I43"><instrument-name>Closed Hi-Hat</instrument-name></score-instrument>'
                    '<score-instrument id="P1-I47"><instrument-name>Open Hi-Hat</instrument-name></score-instrument>'
                    '<midi-instrument id="P1-I43"><midi-channel>10</midi-channel><midi-unpitched>43</midi-unpitched></midi-instrument>'
                    '<midi-instrument id="P1-I47"><midi-channel>10</midi-channel><midi-unpitched>47</midi-unpitched></midi-instrument>',
        clef="<clef><sign>percussion</sign></clef>",
    )
    notations = f"<notations><technical><{sign}/></technical></notations>" if sign else ""
    note = ('<note><unpitched><display-step>G</display-step><display-octave>5</display-octave></unpitched><duration>1</duration>'
            f'<instrument id="{"P1-I47" if open_instrument else "P1-I43"}"/><voice>1</voice><type>quarter</type>'
            f'<notehead>x</notehead>{notations}</note>')
    return header + note * 4 + "</measure></part></score-partwise>"
PIANO = HEADER.format(name="Piano", instruments="", clef="<clef><sign>G</sign><line>2</line></clef>") + (
    "<note><pitch><step>C</step><octave>4</octave></pitch><duration>4</duration><type>whole</type></note></measure></part></score-partwise>")

failures = []


def run(*args: str, apply: str = "") -> subprocess.CompletedProcess:
    """Runs KitNote. `apply` names drum commands for the converter's test hook (KITNOTE_TEST_APPLY)."""
    env = dict(os.environ, KITNOTE_TEST_APPLY=apply) if apply else None
    return subprocess.run([str(EXE), *args], capture_output=True, text=True, timeout=300, env=env)


def tab_after(commands: str, source: Path, folder: Path) -> list:
    """Applies drum commands to a score and returns the rows of the first four measures of its drum tab."""
    tab = folder / "check_tab.txt"
    tab.unlink(missing_ok=True)
    run("-o", str(folder / "check_tab.mscx"), str(source), apply=f"{commands};tab:{tab}")
    if not tab.exists():
        return []
    return tab.read_text(encoding="utf-8").split("\n\n")[0].splitlines()


def counts_after(commands: str, source: Path, folder: Path) -> dict:
    """Applies drum commands to a score and counts the elements of the .mscx that comes out."""
    out = folder / "applied.mscx"
    out.unlink(missing_ok=True)
    run("-o", str(out), str(source), apply=commands)
    text = out.read_text(encoding="utf-8") if out.exists() else ""
    tags = ("Measure", "Note", "Chord", "MeasureRepeat", "Sticking", "Tuplet", "Lyrics", "LayoutBreak", "irregular", "HBox", "acciaccatura", "Articulation", "parentheses", "SystemText")
    return {tag: len(re.findall(f"<{tag}[ >/]", text)) for tag in tags}


def signs_after(commands: str, source: Path, folder: Path) -> list:
    """Applies drum commands to a score and lists its measure-repeat signs as (first measure, measures it repeats)."""
    out = folder / "signs.mscx"
    out.unlink(missing_ok=True)
    run("-o", str(out), str(source), apply=commands)
    text = out.read_text(encoding="utf-8") if out.exists() else ""
    signs = []
    for number, measure in enumerate(re.split("<Measure[ >]", text)[1:], start=1):
        for block in re.findall(r"<MeasureRepeat>(.*?)</MeasureRepeat>", measure, re.S):
            subtype = re.search(r"<subtype>(\d)</subtype>", block)
            count = int(subtype.group(1)) if subtype else 1
            # a sign of 2 or 4 measures stands in the last measure of the first half of its group
            signs.append((number - max(0, count // 2 - 1), count))
    return signs


def midi_notes(path: Path) -> list:
    """The note-ons of a MIDI file as (tick, key), in order."""
    raw = path.read_bytes() if path.exists() else b""
    notes, pos = [], 14
    while pos + 8 <= len(raw) and raw[pos:pos + 4] == b"MTrk":
        end = pos + 8 + struct.unpack(">I", raw[pos + 4:pos + 8])[0]
        pos, tick, status = pos + 8, 0, 0
        while pos < end:
            delta = 0
            while True:
                byte = raw[pos]
                pos += 1
                delta = (delta << 7) | (byte & 0x7F)
                if byte < 0x80:
                    break
            tick += delta
            if raw[pos] >= 0x80:
                status = raw[pos]
                pos += 1
            if status == 0xFF:
                length = 0
                pos += 1
                while True:
                    byte = raw[pos]
                    pos += 1
                    length = (length << 7) | (byte & 0x7F)
                    if byte < 0x80:
                        break
                pos += length
            elif status in (0xF0, 0xF7):
                length = 0
                while True:
                    byte = raw[pos]
                    pos += 1
                    length = (length << 7) | (byte & 0x7F)
                    if byte < 0x80:
                        break
                pos += length
            else:
                size = 1 if status & 0xF0 in (0xC0, 0xD0) else 2
                if status & 0xF0 == 0x90 and raw[pos + 1] > 0:
                    notes.append((tick, raw[pos]))
                pos += size
        pos = end
    return sorted(notes)


def check(name: str, ok: bool, detail: str = "") -> None:
    print(("ok    " if ok else "FAIL  ") + name + (f"  ({detail})" if detail and not ok else ""))
    if not ok:
        failures.append(name)


def convert(src: Path, dst: Path) -> bool:
    dst.unlink(missing_ok=True)
    result = run("-o", str(dst), str(src))
    return result.returncode == 0 and dst.exists() and dst.stat().st_size > 0


def peak(path: Path) -> float:
    """Largest sample of a 32-bit float or 16-bit WAV file."""
    raw = path.read_bytes()
    pos, bits, data = 12, 0, b""
    while pos + 8 <= len(raw):
        size = struct.unpack("<I", raw[pos + 4:pos + 8])[0]
        body = raw[pos + 8:pos + 8 + size]
        if raw[pos:pos + 4] == b"fmt ":
            bits = struct.unpack("<H", body[14:16])[0]
        elif raw[pos:pos + 4] == b"data":
            data = body
        pos += 8 + size + (size & 1)
    if not data or bits not in (16, 32):
        return 0.0
    width = bits // 8
    samples = struct.unpack("<%d%s" % (len(data) // width, "f" if bits == 32 else "h"), data[:len(data) // width * width])
    return max(abs(v) for v in samples) / (1.0 if bits == 32 else 32768.0)


def wav_frames(path: Path) -> tuple:
    """The sample rate and the samples of a 16-bit or 32-bit float WAV file, the channels mixed."""
    raw = path.read_bytes() if path.exists() else b""
    pos, rate, channels, bits, data = 12, 0, 1, 0, b""
    while pos + 8 <= len(raw):
        size = struct.unpack("<I", raw[pos + 4:pos + 8])[0]
        body = raw[pos + 8:pos + 8 + size]
        if raw[pos:pos + 4] == b"fmt ":
            channels, rate = struct.unpack("<HI", body[2:8])
            bits = struct.unpack("<H", body[14:16])[0]
        elif raw[pos:pos + 4] == b"data":
            data = body
        pos += 8 + size + (size & 1)
    if not data or bits not in (16, 32):
        return 0, []
    width = bits // 8
    count = len(data) // (width * channels)
    values = struct.unpack("<%d%s" % (count * channels, "f" if bits == 32 else "h"), data[:count * channels * width])
    scale = 1.0 if bits == 32 else 32768.0
    return rate, [sum(values[i * channels:(i + 1) * channels]) / (channels * scale) for i in range(count)]


def write_wav(path: Path, rate: int, samples: list) -> None:
    """A 16-bit mono WAV file."""
    data = struct.pack("<%dh" % len(samples), *(max(-32768, min(32767, int(v * 32767))) for v in samples))
    path.write_bytes(b"RIFF" + struct.pack("<I", 36 + len(data)) + b"WAVEfmt " + struct.pack("<IHHIIHH", 16, 1, 1, rate, rate * 2, 2, 16)
                     + b"data" + struct.pack("<I", len(data)) + data)


def lag_of(a: list, b: list, rate: int, at: float, reach: int = 200) -> int:
    """Samples by which b comes after a, around 'at' seconds (cross-correlation of a tenth of a second)."""
    start, length = int(at * rate), rate // 10
    piece = a[start:start + length]
    def dot(k: int) -> float:
        return sum(x * y for x, y in zip(piece, b[start + k:start + k + length]))
    return max(range(-reach, reach + 1), key=dot)


def check_recording(kit: Path, d: Path) -> None:
    """A recording made of the score's own sound, 0.75 s of silence before it: attached, measure 1 found
    in it, and played in time with the score (the score and the recording exported one without the other)."""
    score = d / "rec_score.kitnote"
    run("-o", str(score), str(kit), apply="measures:8;tempo:120;@1-2 pattern:rock-8;@3 pattern:snare-fill;"
        "@4-5 pattern:disco;@6 pattern:tom-fill;@7 pattern:half-time;@8 pattern:crash-rock-8")
    render = d / "rec_score.wav"
    if not convert(score, render):
        check("recording: the score is rendered", False)
        return
    rate, samples = wav_frames(render)
    song = d / "song.wav"
    write_wav(song, rate, [0.0] * int(0.75 * rate) + [v * 0.8 for v in samples])

    info = d / "rec_info.json"
    attached = d / "rec_attached.kitnote"
    run("-o", str(attached), str(score), apply=f"recording:{song};info:{info}")
    recording = json.loads(info.read_text(encoding="utf-8")).get("recording", {}) if info.exists() else {}
    check("recording: attached, kept next to the score by its name", recording.get("path") == "song.wav", str(recording))
    check("recording: measure 1 found 0.75 s into it (within 5 ms)",
          abs(recording.get("offset_seconds", 0) - 0.75) <= 0.005, str(recording))

    # in time: the recording alone (the score's notes cleared) against the score alone, at two places
    alone = d / "rec_alone.wav"
    score_alone = d / "rec_score_alone.wav"
    exact = d / "rec_exact.kitnote"
    run("-o", str(exact), str(attached), apply="recordingoffset:0.75")
    subprocess.run([str(EXE), "-o", str(alone), str(exact)], capture_output=True, timeout=300,
                   env=dict(os.environ, KITNOTE_TEST_APPLY="clear", KITNOTE_EXPORT_RECORDING="1"))
    subprocess.run([str(EXE), "-o", str(score_alone), str(exact)], capture_output=True, timeout=300,
                   env=dict(os.environ, KITNOTE_EXPORT_RECORDING="0"))
    _, played = wav_frames(alone)
    _, written = wav_frames(score_alone)
    lags = [lag_of(written, played, rate, at) for at in (2.0, 13.0)] if played and written else []
    check("recording: plays with the score to the sample", lags == [0, 0], str(lags))
    difference = max((abs(x - y) for x, y in zip(written, samples)), default=1.0)
    check("recording: exported audio leaves it out unless asked (the score alone sounds as before)",
          len(written) > rate and difference < 0.01, f"{difference:.4f}")

    run("-o", str(exact), str(attached), apply=f"recordingoffset:1.5;info:{info}")
    recording = json.loads(info.read_text(encoding="utf-8")).get("recording", {}) if info.exists() else {}
    check("recording: its offset given", recording.get("offset_seconds") == 1.5, str(recording))
    # saved in another folder (Save as): the path kept still finds the recording - by name next to the score,
    # relative to it, or else absolute
    def stored_path(score: Path) -> str:
        run("-o", str(score.with_suffix(".mscx")), str(score), apply=f"info:{info}")
        return json.loads(info.read_text(encoding="utf-8")).get("recording", {}).get("path", "") if info.exists() else ""
    (d / "elsewhere").mkdir(exist_ok=True)
    moved = d / "elsewhere" / "moved.kitnote"
    run("-o", str(moved), str(attached))
    path = stored_path(moved)
    check("recording: a score saved in another folder still finds it", Path(path).is_absolute() and Path(path).resolve() == song.resolve(), path)
    back = d / "back.kitnote"
    run("-o", str(back), str(moved))
    check("recording: saved next to it again, the score keeps it by its name", stored_path(back) == "song.wav", stored_path(back))
    # not found (the score file copied alone): the score still plays and exports, the recording silent
    (d / "lonely").mkdir(exist_ok=True)
    (d / "lonely" / "back.kitnote").write_bytes(back.read_bytes())
    lonely = d / "lonely" / "back.wav"
    subprocess.run([str(EXE), "-o", str(lonely), str(d / "lonely" / "back.kitnote")], capture_output=True, timeout=300,
                   env=dict(os.environ, KITNOTE_EXPORT_RECORDING="1"))
    check("recording: a score whose recording is gone still exports its audio", lonely.exists() and peak(lonely) > 0.01)

    run("-o", str(exact), str(attached), apply=f"recording-remove;info:{info}")
    check("recording: taken away", info.exists() and "recording" not in json.loads(info.read_text(encoding="utf-8")))


def check_recording_timing(kit: Path, d: Path) -> None:
    """A recording whose tempo moves (120, then 123, then a slower passage at 108), 0.5 s of silence before it,
    and a score of it at one tempo given the times its measures begin (recordingtimes:, as a transcription
    gives them): hidden tempo marks keep the score with the recording, the chart shows 120 and a mark of its
    own for the slower passage; the first tempo changed scales them, and they can be taken away."""
    played = d / "timing_played.kitnote"
    run("-o", str(played), str(kit), apply="measures:12;tempo:120;@1-12 pattern:rock-8;@5 tempo:123;@9 tempo:108")
    render = d / "timing_played.wav"
    if not convert(played, render):
        check("recording timing: the recording is rendered", False)
        return
    rate, samples = wav_frames(render)
    song = d / "timing_song.wav"
    write_wav(song, rate, [0.0] * int(0.5 * rate) + [v * 0.8 for v in samples])
    starts = [0.5]
    for bpm in [120] * 4 + [123] * 4 + [108] * 4:
        starts.append(starts[-1] + 4 * 60.0 / bpm)
    times = d / "timing_times.json"
    times.write_text(json.dumps([round(t, 4) for t in starts]), encoding="utf-8")

    score = d / "timing_score.kitnote"
    info = d / "timing_info.json"
    run("-o", str(score), str(kit), apply=f"measures:12;tempo:120;@1-12 pattern:rock-8;recording:{song};recordingtimes:{times};info:{info}")
    summary = json.loads(info.read_text(encoding="utf-8")) if info.exists() else {}
    check("recording timing: the score follows it with hidden tempo marks",
          summary.get("tempo_follows_recording") is True and summary.get("hidden_tempo_changes", 0) >= 1, str(summary)[:300])
    check("recording timing: the chart shows 120, and 108 where the passage is clearly slower",
          summary.get("tempo_bpm") == 120 and [(m["measure"], m["bpm"]) for m in summary.get("tempo_marks", [])] == [(1, 120), (9, 108)],
          str(summary.get("tempo_marks")))
    check("recording timing: measure 1 begins where the recording's first measure does",
          summary.get("recording", {}).get("offset_seconds") == 0.5, str(summary.get("recording")))

    # in time: the score alone against the recording alone, late in each passage (at one tempo: 0.2 s off at 8)
    alone = d / "timing_rec_alone.wav"
    score_alone = d / "timing_score_alone.wav"
    subprocess.run([str(EXE), "-o", str(alone), str(score)], capture_output=True, timeout=300,
                   env=dict(os.environ, KITNOTE_TEST_APPLY="clear", KITNOTE_EXPORT_RECORDING="1"))
    subprocess.run([str(EXE), "-o", str(score_alone), str(score)], capture_output=True, timeout=300,
                   env=dict(os.environ, KITNOTE_EXPORT_RECORDING="0"))
    rate, recorded = wav_frames(alone)
    _, written = wav_frames(score_alone)
    measure = [t - 0.5 for t in starts]
    lags = [lag_of(written, recorded, rate, measure[m] + 1.0, reach=rate // 50) for m in (3, 7, 11)] if recorded and written else []
    check("recording timing: the score keeps with the recording (within 10 ms)",
          len(lags) == 3 and all(abs(lag) <= rate // 100 for lag in lags), str(lags))

    # the first tempo changed: everything scaled with it, in the same step; the timing taken away
    run("-o", str(score), str(score), apply=f"tempo:132;info:{info}")
    scaled = json.loads(info.read_text(encoding="utf-8")) if info.exists() else {}
    check("recording timing: changing the first tempo scales the later marks",
          [(m["measure"], m["bpm"]) for m in scaled.get("tempo_marks", [])] == [(1, 132), (9, 119)]
          and scaled.get("hidden_tempo_changes") == summary.get("hidden_tempo_changes"), str(scaled.get("tempo_marks")))
    run("-o", str(score), str(score), apply=f"recording-remove;info:{info}")
    kept = json.loads(info.read_text(encoding="utf-8")) if info.exists() else {}
    check("recording timing: kept when the recording goes", kept.get("tempo_follows_recording") is True and "recording" not in kept)
    run("-o", str(score), str(score), apply=f"repeats;info:{info}")
    signs = json.loads(info.read_text(encoding="utf-8")).get("repeat_signs", []) if info.exists() else []
    check("recording timing: repeat signs reach over its hidden marks",
          any(s["first_measure"] <= 5 < s["first_measure"] + s["measure_count"] for s in signs), str(signs))
    run("-o", str(score), str(score), apply=f"recordingtempo-remove;info:{info}")
    removed = json.loads(info.read_text(encoding="utf-8")) if info.exists() else {}
    check("recording timing: taken away", "tempo_follows_recording" not in removed and removed.get("tempo_bpm") == 132, str(removed)[:200])


def main() -> int:
    if not EXE.exists():
        print(f"{EXE} not found")
        return 2
    version = run("--version").stdout.strip()
    check("reports its own name and version", version.startswith("KitNote "), version)

    with tempfile.TemporaryDirectory() as tmp:
        d = Path(tmp)
        (d / "drums.musicxml").write_text(DRUMS, encoding="utf-8")
        (d / "piano.musicxml").write_text(PIANO, encoding="utf-8")

        check("imports a percussion MusicXML file and saves .kitnote", convert(d / "drums.musicxml", d / "a.kitnote"))
        if (d / "a.kitnote").exists():
            with zipfile.ZipFile(d / "a.kitnote") as z:
                names = z.namelist()
                score = next((n for n in names if n.endswith(".mscx")), "")
                xml = z.read(score).decode("utf-8") if score else ""
            check(".kitnote is a MuseScore archive", bool(score) and "META-INF/container.xml" in names, str(names))
            check("the file records the MuseScore engine version, not KitNote's", "<programVersion>4." in xml)
            check(".kitnote converts to .mscz", convert(d / "a.kitnote", d / "b.mscz"))
            check(".mscz converts back to .kitnote", convert(d / "b.mscz", d / "c.kitnote"))
            for suffix in ("pdf", "png", "svg", "musicxml", "mid", "wav", "mp3", "ogg", "flac", "aac"):
                out = d / f"out.{suffix}"
                ok = convert(d / "a.kitnote", out) or any(d.glob(f"out*.{suffix}"))   # images are written per page
                check(f"exports .{suffix}", ok)
            if (d / "out.mid").exists():
                check("imports the MIDI file it exported", convert(d / "out.mid", d / "from_midi.kitnote"))
            if (d / "out.musicxml").exists():
                check("imports the MusicXML file it exported", convert(d / "out.musicxml", d / "from_xml.kitnote"))

        (d / "ghosts.musicxml").write_text(GHOSTS, encoding="utf-8")
        if convert(d / "drums.musicxml", d / "plain.wav") and convert(d / "ghosts.musicxml", d / "ghosts.wav"):
            plain, ghost = peak(d / "plain.wav"), peak(d / "ghosts.wav")
            check("ghost notes play softer than plain notes", 0 < ghost < plain * 0.6, f"plain {plain:.4f}, ghost {ghost:.4f}")
        else:
            check("ghost notes play softer than plain notes", False, "audio export failed")

        peaks = {}
        for name, xml in (("closed", hi_hat(False)), ("open", hi_hat(True)), ("closed_o", hi_hat(False, "open"))):
            (d / f"{name}.musicxml").write_text(xml, encoding="utf-8")
            peaks[name] = peak(d / f"{name}.wav") if convert(d / f"{name}.musicxml", d / f"{name}.wav") else 0.0
        check('the open sign over a closed hi-hat plays the open hi-hat',
              peaks["open"] > 0 and abs(peaks["closed_o"] - peaks["open"]) < 1e-4 < abs(peaks["closed"] - peaks["open"]), str(peaks))

        templates = sorted((EXE.parent.parent / "templates").rglob("*.mscx"))
        check("templates are installed", len(templates) >= 5, f"{len(templates)} found")
        for template in templates:
            check(f"template {template.stem} opens and exports", convert(template, d / f"{template.stem}.pdf"))

        # The drum commands, applied to the drum-kit template through the converter's test hook.
        kit = next((t for t in templates if t.stem == "01-Drum_Kit"), None)
        if kit:
            n = counts_after("", kit, d)["Measure"]                     # measures in the template
            rock = counts_after("pattern:rock-8", kit, d)
            check("drum pattern: 8 hi-hats, 2 snares and 2 bass drums in every measure",
                  n > 0 and rock["Note"] == 12 * n and rock["Chord"] == 10 * n, str(rock))
            signs = counts_after("pattern:rock-8;repeats", kit, d)
            check("repeat signs: every measure after the first becomes one",
                  signs["MeasureRepeat"] == n - 1 and signs["Note"] == 12, str(signs))
            back = counts_after("pattern:rock-8;repeats;write-out", kit, d)
            check("repeat signs written out give the pattern back", back == rock, f"{back} != {rock}")

            # Phrases of two and four measures played again get the 2- and 4-measure signs; a phrase that is
            # one measure played twice keeps the % signs.
            def phrase(*patterns: str, first: int = 1) -> str:
                return ";".join(f"@{first + i} pattern:{name}" for i, name in enumerate(patterns))
            abab = phrase("rock-8", "disco", "rock-8", "disco")
            abcd = phrase("rock-8", "disco", "rock-16", "motown", "rock-8", "disco", "rock-16", "motown")
            check("2-measure repeat sign: A B A B", signs_after(abab + ";@1-4 repeats", kit, d) == [(3, 2)],
                  str(signs_after(abab + ";@1-4 repeats", kit, d)))
            check("4-measure repeat sign: A B C D A B C D", signs_after(abcd + ";@1-8 repeats", kit, d) == [(5, 4)],
                  str(signs_after(abcd + ";@1-8 repeats", kit, d)))
            alternating = phrase("rock-8", "disco", "rock-8", "disco", "rock-8", "disco", "rock-8", "disco")
            check("A B A B A B A B: three 2-measure signs, not a 4-measure one",
                  signs_after(alternating + ";@1-8 repeats", kit, d) == [(3, 2), (5, 2), (7, 2)],
                  str(signs_after(alternating + ";@1-8 repeats", kit, d)))
            pairs = phrase("rock-8", "rock-8", "disco", "disco", "rock-8", "rock-8", "disco", "disco")
            check("A A B B A A B B: % signs inside the phrase, then the 4-measure sign",
                  signs_after(pairs + ";@1-8 repeats", kit, d) == [(2, 1), (4, 1), (5, 4)],
                  str(signs_after(pairs + ";@1-8 repeats", kit, d)))
            check("a repeat sign does not reach over the start of a section",
                  signs_after(abcd + ";@5 section:Chorus;@1-8 repeats", kit, d) == [],
                  str(signs_after(abcd + ";@5 section:Chorus;@1-8 repeats", kit, d)))
            check("a repeat sign does not reach over a tempo change",
                  signs_after(abcd + ";@3 tempo:140;@1-8 repeats", kit, d) == [],
                  str(signs_after(abcd + ";@3 tempo:140;@1-8 repeats", kit, d)))
            check("a repeat sign does not reach over a repeat barline",
                  signs_after(abcd + ";@5-8 repeat;@1-8 repeats", kit, d) == [],
                  str(signs_after(abcd + ";@5-8 repeat;@1-8 repeats", kit, d)))
            check("repeats run twice change nothing more",
                  signs_after(abcd + ";@1-8 repeats;@1-8 repeats", kit, d) == [(5, 4)])
            written = counts_after(abcd, kit, d)
            check("a 4-measure sign written out gives the phrase back",
                  counts_after(abcd + ";@1-8 repeats;write-out", kit, d) == written,
                  str(counts_after(abcd + ";@1-8 repeats;write-out", kit, d)))
            check("write-out of a range inside a 4-measure sign writes out the whole sign",
                  counts_after(abcd + ";@1-8 repeats;@6 write-out", kit, d) == written)
            check("a pattern over part of a 4-measure sign keeps the rest of it",
                  signs_after(abcd + ";@1-8 repeats;@6 pattern:rock-8", kit, d) == []
                  and counts_after(abcd + ";@1-8 repeats;@6 pattern:rock-8", kit, d)["Note"]
                  == counts_after(abcd + ";@6 pattern:rock-8", kit, d)["Note"])
            for name, commands in (("1-measure", phrase(*["rock-8"] * 4)), ("2-measure", abab), ("4-measure", abcd)):
                plain, signed = d / f"plain_{name}.mid", d / f"signed_{name}.mid"
                plain.unlink(missing_ok=True)
                signed.unlink(missing_ok=True)
                run("-o", str(plain), str(kit), apply=commands)
                run("-o", str(signed), str(kit), apply=commands + ";repeats")
                check(f"{name} repeat signs play what they stand for",
                      midi_notes(plain) and midi_notes(plain) == midi_notes(signed),
                      f"{len(midi_notes(plain))} notes written out, {len(midi_notes(signed))} with signs")
            sticking = counts_after("pattern:rock-8;sticking-r", kit, d)
            check("sticking: one letter for every hand stroke", sticking["Sticking"] == 8 * n, str(sticking))
            steps = counts_after("step:36:0;step:42:0;step:42:2;step:38:4;step:42:4", kit, d)
            check("step grid: five steps make four chords (hands and feet apart)",
                  steps["Note"] == 5 and steps["Chord"] == 4, str(steps))
            check("step grid: a step switched on and off again leaves the measure empty",
                  counts_after("step:38:4;step:38:4", kit, d)["Note"] == 0)
            kinds = counts_after("step:42:0;stepkind:42:0;step:38:4;stepkind:38:4;stepkind:38:4;step:36:8", kit, d)
            check("step grid: an accent and a ghost note survive later edits of the measure",
                  kinds["Note"] == 3 and kinds["Articulation"] == counts_after("", kit, d)["Articulation"] + 1
                  and kinds["parentheses"] == 1, str(kinds))
            marked = counts_after("section:Intro;section:Verse", kit, d)
            check("song structure: a section name replaces the one already at the measure",
                  marked["SystemText"] == 1, str(marked))
            filled = counts_after("section:Verse;fillsection:rock-8", kit, d)
            check("song structure: a section is filled with a groove and ends with a fill",
                  filled["Note"] == 12 * (n - 1) + 14, str(filled))
            tab_file = d / "tab.txt"
            run("-o", str(d / "tab.mscx"), str(kit), apply=f"pattern:rock-8;tab:{tab_file}")
            tab = tab_file.read_text(encoding="utf-8").splitlines() if tab_file.exists() else []
            check("drum tab: hi-hat, snare and bass drum lines of the rock beat",
                  tab[:3] == ["HH|" + "x-x-x-x-x-x-x-x-|" * 4, "SD|" + "----o-------o---|" * 4, "BD|" + "o-------o-------|" * 4],
                  "\n".join(tab[:4]))
            run("-o", str(d / "tab2.mscx"), str(kit), apply=f"tabpaste:{tab_file}")
            pasted = (d / "tab2.mscx").read_text(encoding="utf-8") if (d / "tab2.mscx").exists() else ""
            check("drum tab: pasted back into an empty score it is the rock beat again",
                  len(re.findall("<Note[ >/]", pasted)) == 12 * n, str(len(re.findall("<Note[ >/]", pasted))))
            swung = counts_after("steptriplets;step:42:0;step:42:2;step:36:0;step:38:3", kit, d)
            check("step grid: an empty measure as triplets - one triplet for the hands, quarter notes otherwise",
                  swung["Note"] == 4 and swung["Tuplet"] == 1, str(swung))
            shuffle_tab = d / "shuffle.txt"
            run("-o", str(d / "tab3.mscx"), str(kit), apply=f"pattern:shuffle;tab:{shuffle_tab}")
            back = counts_after(f"tabpaste:{shuffle_tab}", kit, d)
            check("drum tab: a shuffle goes out with twelve columns to the measure and comes back as triplets",
                  shuffle_tab.exists() and "HH|x-xx-xx-xx-x|" in shuffle_tab.read_text(encoding="utf-8")
                  and back["Note"] == 12 * n and back["Tuplet"] >= 4 * n, str(back))
            ride = tab_after("pattern:rock-8;change:hihat-to-ride", kit, d)
            check("change: the hi-hat of a groove moves to the ride",
                  ride[:1] == ["RD|" + "x-x-x-x-x-x-x-x-|" * 4] and len(ride) == 3, "\n".join(ride))
            check("change: and back to the hi-hat",
                  tab_after("pattern:rock-8;change:hihat-to-ride;change:ride-to-hihat", kit, d)[:1]
                  == ["HH|" + "x-x-x-x-x-x-x-x-|" * 4])
            accents = tab_after("pattern:rock-16;change:accent-offbeats", kit, d)
            check("change: accents halfway between the beats", accents[:1] == ["HH|" + "xxXxxxXxxxXxxxXx|" * 4], "\n".join(accents))
            check("change: accents removed again",
                  tab_after("pattern:rock-16;change:accent-beats;change:accent-clear", kit, d)[:1] == ["HH|" + "x" * 16 + "|" + ("x" * 16 + "|") * 3])
            messy = d / "messy.txt"
            messy.write_text("C: Am - F - G\n\nCC|X---------------| repeat 2x\nCb|----o-------o---|\n"
                             "HH|:x-x-x-x-x-x-x-x-:|\nSD| ---- o--- ---- o--- |\nBD|o-------o-------|\n\nSD|oooo|\n", encoding="utf-8")
            pasted_messy = tab_after(f"tabpaste:{messy}", kit, d)
            check("drum tab: text after the tab, unknown lines, repeat dots and a pickup do not become notes",
                  pasted_messy == ["CC|X---------------|" + "----------------|" * 3, "HH|X-x-x-x-x-x-x-x-|" + "----------------|" * 3,
                                   "SD|----o-------o---|" + "----------------|" * 3, "BD|o-------o-------|" + "----------------|" * 3],
                  "\n".join(pasted_messy))
            exported_tab = d / "export.txt"
            run("-o", str(exported_tab), str(kit), apply="pattern:rock-8")
            check("drum tab: written as a .txt file from the command line",
                  exported_tab.exists() and exported_tab.read_text(encoding="utf-8").startswith("HH|x-x-x-x-x-x-x-x-|"))
            own = d / "own_pattern.txt"
            run("-o", str(d / "own.mscx"), str(kit), apply=f"pattern:rock-8-push;measuretab:{own}")
            own_text = own.read_text(encoding="utf-8") if own.exists() else ""
            check("my patterns: the selected measure is kept as a one-measure tab",
                  own_text.splitlines() == ["@4/4", "HH|x-x-x-x-x-x-x-x-|", "SD|----o-------o---|", "BD|o-----o-o-------|"], own_text)
            filled = tab_after(f"tabfill:{own}", kit, d)
            check("my patterns: and written into every selected measure",
                  filled == ["HH|" + "x-x-x-x-x-x-x-x-|" * 4, "SD|" + "----o-------o---|" * 4, "BD|" + "o-----o-o-------|" * 4],
                  " / ".join(filled))
            headed = d / "headed.txt"
            headed.write_text("Intro\nCC|X---------------|\nBD|o---------------|\nVerse\nHH|x-x-x-x-x-x-x-x-|\n"
                              "Note: open on the last\nBD|o-------o-------|\n", encoding="utf-8")
            rows = tab_after(f"tabpaste:{headed}", kit, d)
            check("drum tab: a heading line between rows (no blank line) starts the next row",
                  rows[:1] == ["CC|X---------------|" + "----------------|" * 3]
                  and any(r.startswith("HH|----------------|x-x-x-x-x-x-x-x-|") for r in rows), " / ".join(rows))
            unknown = d / "unknown.txt"
            unknown.write_text("HH|x-x-x-x-x-x-x-x-|????????|x-x-x-x-x-x-x-x-|\n", encoding="utf-8")
            rows = tab_after(f"tabpaste:{unknown}", kit, d)
            check("drum tab: a '????' measure (one drumTabText could not read) is passed over, not shifted",
                  rows[:1] == ["HH|x-x-x-x-x-x-x-x-|----------------|x-x-x-x-x-x-x-x-|----------------|"], " / ".join(rows))
            bell = d / "bell.txt"
            bell.write_text("RD|b-x-b-x-b-x-b-x-|" + chr(10) + "BD|o-------o-------|" + chr(10), encoding="utf-8")
            check("drum tab: the ride bell (b) comes back as the bell",
                  tab_after(f"tabpaste:{bell}", kit, d)[:1] == ["RD|b-x-b-x-b-x-b-x-|" + "----------------|" * 3])
            large = d / "large.kitnote"
            run("-o", str(large), str(kit), apply="spatium:2.1")
            large_style = zipfile.ZipFile(large).read("score_style.mss").decode("utf-8") if large.exists() else ""
            check("staff size: Format > Staff size changes the space between the lines", "<spatium>2.1</spatium>" in large_style)
            tempo_out = d / "tempo.mscx"
            run("-o", str(tempo_out), str(kit), apply="tempo:132")
            tempo_text = tempo_out.read_text(encoding="utf-8") if tempo_out.exists() else ""
            check("tap tempo: the tempo of the music is written into the score",
                  tempo_text.count("<Tempo>") == 1 and "<tempo>2.2</tempo>" in tempo_text and "= 132" in tempo_text)
            run("-o", str(tempo_out), str(kit), apply="tempo:100;@9 tempo:140")
            tempo_text = tempo_out.read_text(encoding="utf-8") if tempo_out.exists() else ""
            bars = re.split("<Measure[ >]", tempo_text)
            check("tempo change: '@9 tempo:140' puts a mark at measure 9, and measure 1 keeps its own",
                  tempo_text.count("<Tempo>") == 2 and len(bars) > 9 and "= 100" in bars[1] and "= 140" in bars[9],
                  str(tempo_text.count("<Tempo>")))
            repeated = d / "repeated.mscx"
            run("-o", str(repeated), str(kit), apply="pattern:rock-8;@5-8 repeat:3;@8 volta:1;@9-10 volta:2")
            repeated_text = repeated.read_text(encoding="utf-8") if repeated.exists() else ""
            bars = re.split("<Measure[ >]", repeated_text)
            check("repeat barlines: a start repeat at 5, an end repeat played three times at 8, two endings",
                  repeated_text.count("<startRepeat/>") == 1 and len(bars) > 8 and "<startRepeat/>" in bars[5]
                  and "<endRepeat>3</endRepeat>" in bars[8] and len(re.findall("<Volta[ >]", repeated_text)) == 2,
                  str(repeated_text.count("<startRepeat/>")))
            removed = counts_after("pattern:rock-8;@5-8 repeat;@8 volta:1;@5-8 repeat-remove", kit, d)
            removed_text = (d / "applied.mscx").read_text(encoding="utf-8") if (d / "applied.mscx").exists() else ""
            check("repeat barlines: taken away again with repeat-remove", removed == rock and "<startRepeat" not in removed_text
                  and "<endRepeat" not in removed_text
                  and "<Volta" not in removed_text, str(removed))
            refused = d / "bad_row.txt"
            refused.write_text("HH|x-x-x-x-x-x-x-x-|x-x-x-x-x-x-x-x-|\nSD|----o-------o---|----o-------o---|\n"
                               "T1|----------oo----|\nBD|o-------o-------|o-------o-------|\n", encoding="utf-8")
            report = d / "report.txt"
            report.unlink(missing_ok=True)
            refused_out = d / "refused.mscx"
            refused_out.unlink(missing_ok=True)
            subprocess.run([str(EXE), "-o", str(refused_out), str(kit)], capture_output=True, text=True, timeout=300,
                           env=dict(os.environ, KITNOTE_TEST_APPLY=f"tabpaste:{refused}", KITNOTE_APPLY_REPORT=str(report)))
            refused_text = refused_out.read_text(encoding="utf-8") if refused_out.exists() else ""
            check("drum tab: a row whose lines cover different measures is refused, naming the line, and not written",
                  report.exists() and "line 3" in report.read_text(encoding="utf-8")
                  and len(re.findall("<Note[ >/]", refused_text)) == 0, report.read_text(encoding="utf-8") if report.exists() else "")
            full = d / "full_line.txt"
            full.write_text("@4/4\nHH|xxxxxxxxxxxxxxxx|\nSD|----o-------o---|\n", encoding="utf-8")
            full_rows = tab_after(f"tabfill:{full}", kit, d)
            check("my patterns: a line with a stroke on every step is written, and nothing past the selection",
                  full_rows == ["HH|" + "xxxxxxxxxxxxxxxx|" * 4, "SD|" + "----o-------o---|" * 4], " / ".join(full_rows))
            waltz = d / "waltz.txt"
            waltz.write_text("@3/4\nHH|x-x-x-x-x-x-|\nBD|o-----------|\n", encoding="utf-8")
            check("my patterns: a pattern of 3/4 is not squeezed into 4/4", tab_after(f"tabfill:{waltz}", kit, d) == [])
            rudiment = counts_after("pattern:paradiddle", kit, d)
            check("rudiment: sixteen notes in every measure, each with its sticking",
                  rudiment["Note"] == 16 * n and rudiment["Sticking"] == 16 * n, str(rudiment))
            signs = counts_after("pattern:paradiddle;repeats", kit, d)
            check("rudiment: equal measures with equal sticking become repeat signs",
                  signs["MeasureRepeat"] == n - 1 and signs["Sticking"] == 16, str(signs))
            jazz = counts_after("pattern:jazz-ride", kit, d)
            check("jazz ride: six ride notes, two triplets and two pedal hi-hats in every measure",
                  jazz["Note"] == 8 * n and jazz["Tuplet"] == 2 * n, str(jazz))
            flams = counts_after("pattern:flam-tap", kit, d)
            check("flam tap: a grace note before every second note",
                  flams["Note"] == 24 * n and flams["Sticking"] == 16 * n and flams["acciaccatura"] == 8 * n, str(flams))
            counted = counts_after("pattern:rock-8;counting", kit, d)
            check("counting: one count for every eighth note", counted["Lyrics"] == 8 * n, str(counted))
            check("counting: run again, it is replaced", counts_after("pattern:rock-8;counting;counting", kit, d) == counted)
            shuffle = counts_after("pattern:shuffle", kit, d)
            check("shuffle: four triplets in every measure", shuffle["Tuplet"] == 4 * n and shuffle["Note"] == 12 * n, str(shuffle))
            key = counts_after("pattern:rock-8;key", kit, d)
            check("drum key: one extra measure naming the three drums used",
                  key["Measure"] == n + 1 and key["Lyrics"] == 3 and key["irregular"] == 1 and key["Note"] == 12 * n + 3, str(key))
            check("drum key: run again, it is replaced, not doubled", counts_after("pattern:rock-8;key;key", kit, d) == key)
            check("drum key: removed, the score is as before", counts_after("pattern:rock-8;key;key-remove", kit, d) == rock)
            renewed = counts_after("pattern:rock-8;key;pattern:tom-fill;key", kit, d)
            check("drum key: renewed for the drums in use", renewed["Measure"] == n + 1 and renewed["HBox"] == 1
                  and renewed["Lyrics"] == 4 and renewed["irregular"] == 1, str(renewed))
            check("drum key: an empty frame takes the rest of its line", key["HBox"] == 1, str(key))
            swung = d / "swung.kitnote"
            run("-o", str(swung), str(kit), apply="swing-8")
            style = zipfile.ZipFile(swung).read("score_style.mss").decode("utf-8") if swung.exists() else ""
            check("swing: stored in the style of the score", "<swingUnit>eighth</swingUnit>" in style)
            empty_key = counts_after("key", kit, d)
            check("drum key of a score without notes names the main drums",
                  empty_key["Lyrics"] >= 6 and empty_key["Lyrics"] == empty_key["Note"]
                  and empty_key["irregular"] == empty_key["Measure"] - n, str(empty_key))

            check_recording(kit, d)
            check_recording_timing(kit, d)

        for guide in ("user-guide_ko.html", "user-guide_en.html"):
            check(f"{guide} is installed", (EXE.parent.parent / "manual" / "kitnote" / guide).is_file())

        check("refuses a score with a pitched instrument", not convert(d / "piano.musicxml", d / "piano.pdf"))
        check("does not write formats it does not support (.gp)", not convert(d / "drums.musicxml", d / "x.gp"))

    print(f"\n{len(failures)} failure(s)" if failures else "\nall checks passed")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
