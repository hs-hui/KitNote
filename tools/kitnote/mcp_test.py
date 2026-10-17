"""Talks to kitnote-mcp as an AI tool would (JSON-RPC over stdio) and checks what it answers.
python tools/kitnote/mcp_test.py [folder for the files]   - prints "mcp: all checks passed" at the end."""
import json
import re
import struct
import subprocess
import sys
import tempfile
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SERVER = ROOT / "build.install" / "bin" / "kitnote-mcp.exe"


class Client:
    def __init__(self):
        self.process = subprocess.Popen([str(SERVER)], stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)
        self.next_id = 1
        self.notifications = []

    def request(self, method, params=None):
        message = {"jsonrpc": "2.0", "id": self.next_id, "method": method, "params": params or {}}
        self.next_id += 1
        self.process.stdin.write((json.dumps(message) + "\n").encode("utf-8"))
        self.process.stdin.flush()
        # notifications (progress) may come first: they are kept, and the answer is returned
        while True:
            answer = json.loads(self.process.stdout.readline().decode("utf-8"))
            if "method" in answer and "id" not in answer:
                self.notifications.append(answer)
                continue
            return answer

    def notify(self, method):
        self.process.stdin.write((json.dumps({"jsonrpc": "2.0", "method": method}) + "\n").encode("utf-8"))
        self.process.stdin.flush()

    def call(self, tool, **arguments):
        result = self.request("tools/call", {"name": tool, "arguments": arguments})["result"]
        return result.get("isError", False), result["content"][0]["text"]

    def close(self):
        self.process.stdin.close()
        self.process.wait(timeout=30)


failures = []


def check(what, ok, detail=""):
    print(("ok    " if ok else "FAIL  ") + what + ("" if ok else f"  ({detail[:400]})"))
    if not ok:
        failures.append(what)


def wav_mono(path):
    """The sample rate and the samples of a 16-bit or 32-bit float WAV file, the channels mixed."""
    raw = path.read_bytes() if path.exists() else b""
    pos, rate, channels, bits, data = 12, 0, 1, 0, b""
    while pos + 8 <= len(raw):
        size = struct.unpack("<I", raw[pos + 4:pos + 8])[0]
        if raw[pos:pos + 4] == b"fmt ":
            channels, rate = struct.unpack("<HI", raw[pos + 10:pos + 16])
            bits = struct.unpack("<H", raw[pos + 22:pos + 24])[0]
        elif raw[pos:pos + 4] == b"data":
            data = raw[pos + 8:pos + 8 + size]
        pos += 8 + size + (size & 1)
    if not data or bits not in (16, 32):
        return 0, []
    count = len(data) // (bits // 8 * channels)
    values = struct.unpack("<%d%s" % (count * channels, "f" if bits == 32 else "h"), data[:count * channels * bits // 8])
    scale = channels * (1.0 if bits == 32 else 32768.0)
    return rate, [sum(values[i * channels:(i + 1) * channels]) / scale for i in range(count)]


def lag_ms(score, recording, rate, at, offset, reach_ms=40):
    """Milliseconds by which the recording (measure 1 at "offset" seconds in it) comes after the score, around
    "at" seconds of the score (cross-correlation of a tenth of a second, every second sample)"""
    start, length, shift, reach = int(at * rate), rate // 10, int(round(offset * rate)), int(reach_ms * rate / 1000)
    piece = score[start:start + length:2]
    def dot(k):
        return sum(x * y for x, y in zip(piece, recording[start + shift + k:start + shift + k + length:2]))
    return 1000.0 * max(range(-reach, reach + 1), key=dot) / rate


def main():
    folder = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(tempfile.mkdtemp())
    folder.mkdir(parents=True, exist_ok=True)
    score = folder / "ai_song.kitnote"
    score.unlink(missing_ok=True)

    client = Client()
    init = client.request("initialize", {"protocolVersion": "2025-06-18", "capabilities": {}, "clientInfo": {"name": "test", "version": "1"}})
    check("initialize: server info", init["result"]["serverInfo"]["name"] == "kitnote", json.dumps(init))
    check("initialize: the instructions say to load the skill first",
          "Load the kitnote skill first" in init["result"].get("instructions", ""), json.dumps(init))
    client.notify("notifications/initialized")
    listed = client.request("tools/list")["result"]["tools"]
    tools = [tool["name"] for tool in listed]
    check("tools/list: the tools are there", {"create_score", "write_drum_tab", "read_score", "export_score", "transcribe_audio", "audio_engine"} <= set(tools), str(tools))
    edit_help = next((tool["description"] for tool in listed if tool["name"] == "edit_score"), "")
    check("edit_score describes tempo changes, repeat barlines and endings",
          "@9 tempo:140" in edit_help and "@5-8 repeat" in edit_help and "volta:1" in edit_help, edit_help)

    prompts = [prompt["name"] for prompt in client.request("prompts/list")["result"]["prompts"]]
    check("prompts/list: the ready requests are there", {"drum_chart", "transcribe_song"} <= set(prompts), str(prompts))
    got = client.request("prompts/get", {"name": "drum_chart", "arguments": {"style": "funk"}})["result"]
    check("prompts/get: the text with its arguments", "Style: funk" in got["messages"][0]["content"]["text"], json.dumps(got))

    error, text = client.call("create_score", path=str(score), title="AI Song", tempo=112, measures=8, time_signature="4/4")
    check("create_score", not error and '"tempo_bpm": 112' in text and '"measures": 8' in text, text)
    check("create_score says what comes next", "write_drum_tab" in text.split("\n")[0], text[:300])

    tab = ("CC|X---------------|----------------|\n"
           "HH|--x-x-x-x-x-x-x-|x-x-x-x-x-x-x-o-|\n"
           "SD|----o-------o---|----o--g----o---|\n"
           "BD|o-------o-o-----|o-----o-o-------|\n")
    error, text = client.call("write_drum_tab", path=str(score), tab=tab, start_measure=1)
    check("write_drum_tab", not error and "HH|--x-x-x-x-x-x-x-|x-x-x-x-x-x-x-o-|" in text.replace("\\n", "\n"), text)
    check("empty measures left at the end are pointed out", "the last 6 measure(s), 3-8, are empty" in text
          and '"empty_measures_at_end": 6' in text, text[:300])

    error, text = client.call("fill_pattern", path=str(score), pattern="tom-fill", first_measure=8, last_measure=8)
    check("fill_pattern into one measure", not error and "oooo" in text, text)
    check("no note when the music reaches the end", "are empty" not in text, text[:300])

    error, text = client.call("add_section", path=str(score), measure=5, name="Chorus")
    check("add_section", not error and '"Chorus"' in text, text)

    error, text = client.call("edit_score", path=str(score), commands=["@3-4 pattern:rock-8", "@5-7 pattern:disco"])
    check("edit_score with measure ranges", not error and "x-o-x-o-" in text, text)

    error, text = client.call("set_properties", path=str(score), title="AI Song 2", tempo=120)
    check("set_properties", not error and '"title": "AI Song 2"' in text and '"tempo_bpm": 120' in text, text)

    error, text = client.call("read_score", path=str(score))
    summary = json.loads(text) if not error else {}
    check("read_score: json with the tab", summary.get("measures") == 8 and summary.get("drum_tab", "").startswith("CC|X"), text)

    error, text = client.call("set_properties", path=str(score), title="x" + chr(10) + "info:" + str(folder / "leak.json"))
    check("a line break cannot smuggle in a second command", error and not (folder / "leak.json").exists(), text)
    error, text = client.call("edit_score", path=str(score), commands=["@1 info:" + str(folder / "leak.json")])
    check("edit_score refuses commands that write files", error and not (folder / "leak.json").exists(), text)
    error, text = client.call("set_properties", path=str(score), measures="10")
    check("a number may come as text", not error and '"measures": 10' in text, text)
    error, text = client.call("set_properties", path=str(score), measures="lots")
    check("nonsense instead of a number is an error, not a cut score", error, text)
    error, text = client.call("edit_score", path=str(score), commands=["@1 pattern:no-such-groove"])
    check("an unknown command is reported, not answered with Done", error and "unknown" in text, text)
    error, text = client.call("write_drum_tab", path=str(score), tab="HH|xxxxxxxx|" + chr(10) + "BD|o---o---|", start_measure=9)
    check("a line with a stroke on every step (straight eighths) is written",
          not error and "HH|" in text and "x-x-x-x-x-x-x-x-|" in json.loads(text[text.index("{"):]).get("drum_tab", ""), text)

    # what the QA of the server found
    error, text = client.call("write_drum_tab", path=str(score), tab="HH|x-x-x-x-x-x-x-x-|\nXX|o-------o-------|", start_measure=1)
    check("an unknown drum label is an error, not a line quietly left out", error and "XX" in text, text)
    error, text = client.call("write_drum_tab", path=str(score), tab="HH|x-x-x-x-x-x-x-x-|\nBD|o-------o-----|", start_measure=1)
    check("a measure with a miscounted line is an error, nothing is written", error and "could not be written" in text, text)
    error, text = client.call("write_drum_tab", path=str(score),
                              tab="HH|x-x-x-x-x-x-x-x-|\nSD|----o-------o---|\nnote: open on the last\nBD|o-------o-------|", start_measure=2)
    rows = json.loads(text[text.index("{"):]).get("drum_tab", "").splitlines() if not error else []
    check("a line of words between the lines of a row does not split it",
          not error and any(r.startswith("BD|") and r.split("|")[2] == "o-------o-------" for r in rows), text)
    error, text = client.call("fill_pattern", path=str(score), pattern="Rock-8", first_measure=11, last_measure=12)
    check("a pattern past the end adds the measures it needs (and its name is not case-sensitive)",
          not error and '"measures": 12' in text, text)
    error, text = client.call("fill_pattern", path=str(score), pattern="rock-8", first_measure=3, last_measure=2)
    check("a backwards range is an error", error, text)
    error, text = client.call("edit_score", path=str(score), commands=["@11-12 repeats"])
    bars = json.loads(text[text.index("{"):]).get("drum_tab", "").split("\n\n")[-1].splitlines() if not error else []
    check("measures shown as % signs read as what they repeat",
          not error and any(r.startswith("HH|") and r.count("x-x-x-x-x-x-x-x-") >= 2 for r in bars), text)

    # phrases of two and four measures played again become the 2- and 4-measure repeat signs
    phrases = folder / "ai_phrases.kitnote"
    phrases.unlink(missing_ok=True)
    client.call("create_score", path=str(phrases), title="Phrases", tempo=100, measures=16)
    eighths, sixteenths, quarters, offbeats = "x-x-x-x-x-x-x-x-", "xxxxxxxxxxxxxxxx", "x---x---x---x---", "--x---x---x---x-"
    snare, kick = "----o-------o---", "o-------o-------"
    # A % % % | B C %% | A B C D %%%%: one-measure signs, a 2-measure sign at 7 and a 4-measure sign at 13
    hats = [eighths] * 4 + [sixteenths, quarters] * 2 + [eighths, sixteenths, quarters, offbeats] * 2
    tab = "\n\n".join("HH|" + "|".join(hats[i:i + 4]) + "|\nSD|" + (snare + "|") * 4 + "\nBD|" + (kick + "|") * 4
                      for i in range(0, 16, 4))

    def hi_hat_measures(summary):
        return [m for row in summary.get("drum_tab", "").splitlines() if row.startswith("HH|") for m in row.split("|")[1:-1]]
    error, text = client.call("write_drum_tab", path=str(phrases), tab=tab, start_measure=1)
    check("a tab of sixteen measures is written", not error, text)
    error, text = client.call("edit_score", path=str(phrases), commands=["repeats"])
    info = json.loads(text[text.index("{"):]) if not error else {}
    check("repeats: % signs, a 2-measure sign and a 4-measure sign, listed as repeat_signs",
          info.get("repeat_signs") == [{"first_measure": 2, "measure_count": 1}, {"first_measure": 3, "measure_count": 1},
                                       {"first_measure": 4, "measure_count": 1}, {"first_measure": 7, "measure_count": 2},
                                       {"first_measure": 13, "measure_count": 4}], text[:1500])
    check("the drum tab shows what the 2- and 4-measure signs stand for", hi_hat_measures(info) == hats, text[:1500])
    error, text = client.call("write_drum_tab", path=str(phrases), tab="HH|" + quarters + "|\nSD|" + snare + "|\nBD|" + kick + "|",
                              start_measure=14)
    info = json.loads(text[text.index("{"):]) if not error else {}
    check("a tab over part of a 4-measure sign keeps the other measures of it as notes",
          not error and hi_hat_measures(info)[12:] == [eighths, quarters, quarters, offbeats]
          and not any(s.get("measure_count") == 4 for s in info.get("repeat_signs", [])), text[:1500])
    error, text = client.call("edit_score", path=str(phrases), commands=["write-out"])
    info = json.loads(text[text.index("{"):]) if not error else {}
    check("write-out turns every repeat sign back into notes", not error and "repeat_signs" not in info, text[:800])

    error, text = client.call("set_properties", path=str(score), time_signature="4/0")
    check("a time signature that is none is an error", error, text)
    error, text = client.call("add_section", path=str(score), measure=3, name="")
    check("a section without a name is an error", error, text)
    error, text = client.call("export_score", path=str(score), output_path=str(folder / "x.xyz"))
    check("an export format KitNote does not write is named in the error", error and "pdf" in text, text)

    # rows of a tab that would put strokes in the wrong measures are refused, and the score stays as it was
    form = folder / "ai_form.kitnote"
    form.unlink(missing_ok=True)
    client.call("create_score", path=str(form), title="Form", tempo=100, measures=12)
    before = form.read_bytes()
    bad_rows = {
        "a line of a row with fewer measures than the others":
            ("HH|x-x-x-x-x-x-x-x-|x-x-x-x-x-x-x-x-|\nSD|----o-------o---|----o-------o---|\nT1|----------oo----|\n"
             "BD|o-------o-------|o-------o-------|", "line 3"),
        "an empty line that splits a row":
            ("HH|x-x-x-x-x-x-x-x-|\nSD|----o-------o---|\n\nBD|o-------o-------|", "empty line"),
        "a row of cymbals alone among rows with drums":
            ("HH|x-x-x-x-x-x-x-x-|\n\nHH|x-x-x-x-x-x-x-x-|\nSD|----o-------o---|\nBD|o-------o-------|", "only cymbal"),
    }
    for what, (tab, said) in bad_rows.items():
        error, text = client.call("write_drum_tab", path=str(form), tab=tab, start_measure=1)
        check(f"write_drum_tab refuses {what}, and writes nothing", error and said in text and form.read_bytes() == before, text)
    error, text = client.call("write_drum_tab", path=str(form), tab="HH|x-x-x-x-x-x-x-x-|x-x-x-x-x-x-x-x-|", start_measure=1)
    check("a tab of the hi-hat alone is still written", not error, text)

    # a tempo change partway through: a mark at its measure; measure 1 keeps its tempo
    error, text = client.call("edit_score", path=str(form), commands=["@9 tempo:140"])
    info = json.loads(text[text.index("{"):]) if not error else {}
    marks = info.get("tempo_marks", [])
    check("@9 tempo:140 puts a tempo mark at measure 9 and leaves measure 1's",
          info.get("tempo_bpm") == 100 and {"measure": 9, "bpm": 140} in marks
          and any(m.get("measure") == 1 and m.get("bpm") == 100 for m in marks), text[:600])
    error, text = client.call("set_properties", path=str(form), tempo=90)
    info = json.loads(text[text.index("{"):]) if not error else {}
    check("set_properties tempo changes the start and keeps the change at measure 9",
          info.get("tempo_bpm") == 90 and {"measure": 9, "bpm": 140} in info.get("tempo_marks", []), text[:600])
    error, text = client.call("edit_score", path=str(form), commands=["@9-12 tempo:140"])
    check("a tempo over a range of measures is an error (it goes at one measure)", error, text)
    error, text = client.call("edit_score", path=str(form), commands=["@3 title:Part"])
    check("a title for some measures is an error: it applies to the whole score", error and "whole score" in text, text)

    # repeat barlines (도돌이표) and endings
    error, text = client.call("edit_score", path=str(form), commands=["@5-8 repeat", "@8 volta:1", "@9-10 volta:2"])
    info = json.loads(text[text.index("{"):]) if not error else {}
    check("@5-8 repeat gives a start repeat at 5 and an end repeat at 8",
          info.get("repeats") == [{"first_measure": 5, "last_measure": 8, "play_count": 2}], text[:800])
    check("@8 volta:1 and @9-10 volta:2 give the 1st and 2nd endings",
          info.get("voltas") == [{"first_measure": 8, "last_measure": 8, "endings": [1]},
                                 {"first_measure": 9, "last_measure": 10, "endings": [2]}], text[:800])
    with zipfile.ZipFile(form) as archive:
        xml = archive.read(next(n for n in archive.namelist() if n.endswith(".mscx"))).decode("utf-8")
    check("the saved score has the repeat barlines and the endings",
          xml.count("<startRepeat/>") == 1 and xml.count("<endRepeat>2</endRepeat>") == 1
          and len(re.findall("<Volta[ >]", xml)) == 2, str(xml.count("<startRepeat/>")))
    error, text = client.call("edit_score", path=str(form), commands=["@5-8 repeat:3"])
    info = json.loads(text[text.index("{"):]) if not error else {}
    check("repeat:3 plays the measures three times",
          info.get("repeats") == [{"first_measure": 5, "last_measure": 8, "play_count": 3}], text[:600])
    error, text = client.call("edit_score", path=str(form), commands=["repeat"])
    check("a repeat without its measures is an error", error, text)
    error, text = client.call("edit_score", path=str(form), commands=["@11-20 repeat"])
    check("a repeat past the end of the score is an error", error, text)
    error, text = client.call("edit_score", path=str(form), commands=["@5-10 repeat-remove"])
    info = json.loads(text[text.index("{"):]) if not error else {}
    check("repeat-remove takes the repeats and the endings away",
          not error and "repeats" not in info and "voltas" not in info, text[:600])

    # two calls on one score at once: both changes are kept
    import threading
    other = Client()
    other.request("initialize", {"protocolVersion": "2025-06-18"})
    results = {}

    def run(c, key, **arguments):
        results[key] = c.call("fill_pattern", path=str(score), **arguments)

    t1 = threading.Thread(target=run, args=(client, "a"), kwargs={"pattern": "disco", "first_measure": 1, "last_measure": 2})
    t2 = threading.Thread(target=run, args=(other, "b"), kwargs={"pattern": "tom-fill", "first_measure": 7, "last_measure": 8})
    t1.start(); t2.start(); t1.join(); t2.join()
    other.close()
    error, text = client.call("read_score", path=str(score))
    tab = json.loads(text).get("drum_tab", "") if not error else ""
    check("two calls on one score at the same time: neither change is lost",
          "x-o-x-o-x-o-x-o-" in tab and "oooo" in tab, tab)

    answer = client.request("tools/call", {"name": "view_score", "arguments": {"path": str(score)}})["result"]
    images = [c for c in answer["content"] if c["type"] == "image"]
    check("view_score: a page as a PNG picture", not answer.get("isError") and images
          and images[0]["mimeType"] == "image/png" and len(images[0]["data"]) > 1000, str(answer)[:300])

    # the files written come back as links a client can offer to open or download
    out = folder / "ai_song_link.pdf"
    out.unlink(missing_ok=True)
    answer = client.request("tools/call", {"name": "export_score", "arguments": {"path": str(score), "output_path": str(out)}})["result"]
    links = [c for c in answer["content"] if c["type"] == "resource_link"]
    check("export_score answers with a resource link to the file", len(links) == 1 and links[0]["name"] == out.name
          and links[0]["mimeType"] == "application/pdf" and links[0]["uri"].startswith("file:///"), json.dumps(answer)[:400])
    answer = client.request("tools/call", {"name": "read_score", "arguments": {"path": str(score)}})["result"]
    check("read_score links no file", not [c for c in answer["content"] if c["type"] == "resource_link"], json.dumps(answer)[:200])

    for suffix in ("pdf", "mid", "musicxml", "txt"):
        out = folder / f"ai_song.{suffix}"
        out.unlink(missing_ok=True)
        error, text = client.call("export_score", path=str(score), output_path=str(out))
        check(f"export_score .{suffix}", not error and out.exists() and out.stat().st_size > 0, text)
    error, text = client.call("export_score", path=str(score), output_path=str(folder / "ai_song.png"))
    check("export_score .png (page by page)", not error and (folder / "ai_song-1.png").exists(), text)

    # the recording a score plays along with: here its own sound, so measure 1 is at its very start
    take = folder / "ai_song_take.wav"
    take.unlink(missing_ok=True)
    client.call("export_score", path=str(score), output_path=str(take))
    error, text = client.call("set_properties", path=str(score), recording=str(take))
    recording = json.loads(text[text.index("{"):]).get("recording", {}) if not error else {}
    check("set_properties recording: attached next to the score, measure 1 found at its start",
          recording.get("path") == take.name and abs(recording.get("offset_seconds", 9)) <= 0.005, text[:600])
    error, text = client.call("set_properties", path=str(score), recording_offset=1.25)
    recording = json.loads(text[text.index("{"):]).get("recording", {}) if not error else {}
    check("set_properties recording_offset", recording.get("offset_seconds") == 1.25, text[:600])
    error, text = client.call("set_properties", path=str(score), recording=str(folder / "no_such_take.wav"))
    check("a recording that is not there is an error", error and "no such file" in text, text)
    error, text = client.call("set_properties", path=str(score), recording="")
    check("set_properties recording \"\" takes it away", not error and '"recording"' not in text, text[:600])

    # the audio engine: its state, and no installation without the user's consent
    error, text = client.call("audio_engine", action="status")
    check("audio_engine status says whether it is installed", not error and '"installed"' in text, text)
    error, text = client.call("audio_engine", action="install")
    check("audio_engine install without confirm asks for the user's consent first", error and "Ask the user first" in text, text)
    error, text = client.call("audio_engine", action="remove")
    check("audio_engine remove without confirm asks too", error and "Ask the user first" in text, text)

    # a recording of the score, back into a score (only where the audio engine is installed)
    engine = Path.home() / "AppData" / "Local" / "KitNote" / "ai" / "engine.json"
    if engine.exists():
        wav = folder / "ai_song.wav"
        client.call("export_score", path=str(score), output_path=str(wav))
        back = folder / "ai_song_back.kitnote"
        back.unlink(missing_ok=True)
        answer = client.request("tools/call", {"name": "transcribe_audio", "_meta": {"progressToken": "t1"},
                                               "arguments": {"audio_path": str(wav), "output_path": str(back), "separate": False}})["result"]
        error, text = answer.get("isError", False), answer["content"][0]["text"]
        check("transcribe_audio sends progress to a client that asks", any(n["method"] == "notifications/progress"
              and n["params"]["progressToken"] == "t1" for n in client.notifications), str(client.notifications[:2]))
        check("transcribe_audio: a recording becomes a score at its tempo",
              not error and back.exists() and '"tempo_bpm": 120' in text and "BD|o" in json.loads(text[text.index("{"):]).get("drum_tab", ""), text)
        check("transcribe_audio: the score plays its recording along with it",
              not error and json.loads(text[text.index("{"):]).get("recording", {}).get("path") == wav.name, text[:800])

        # a waltz: its time signature is found from the groove
        waltz = folder / "ai_waltz.kitnote"
        waltz.unlink(missing_ok=True)
        client.call("create_score", path=str(waltz), title="Waltz", time_signature="3/4", tempo=120, measures=8)
        client.call("write_drum_tab", path=str(waltz), tab=("HH|" + "x-x-x-x-x-x-|" * 8 + "\nSD|" + "----o---o---|" * 8
                                                            + "\nBD|" + "o-----------|" * 8 + "\n"))
        waltz_wav = folder / "ai_waltz.wav"
        client.call("export_score", path=str(waltz), output_path=str(waltz_wav))
        waltz_back = folder / "ai_waltz_back.kitnote"
        waltz_back.unlink(missing_ok=True)
        error, text = client.call("transcribe_audio", audio_path=str(waltz_wav), output_path=str(waltz_back), separate=False)
        check("transcribe_audio: a waltz is found to be in 3/4",
              not error and json.loads(text[text.index("{"):]).get("time_signature") == "3/4", text)

        # a slow 6/8 (dotted quarter = 70) whose beat the beat model may hear wrong: the tempo given wins
        slow = folder / "ai_slow68.kitnote"
        slow.unlink(missing_ok=True)
        client.call("create_score", path=str(slow), title="Slow", time_signature="6/8", tempo=105, measures=8)
        client.call("fill_pattern", path=str(slow), pattern="six-eight", first_measure=1, last_measure=8)
        slow_wav = folder / "ai_slow68.wav"
        client.call("export_score", path=str(slow), output_path=str(slow_wav))
        slow_back = folder / "ai_slow68_back.kitnote"
        slow_back.unlink(missing_ok=True)
        error, text = client.call("transcribe_audio", audio_path=str(slow_wav), output_path=str(slow_back), separate=False,
                                  time_signature="6/8", tempo=70)
        info = json.loads(text[text.index("{"):]) if not error else {}
        check("transcribe_audio: the tempo given is the tempo of the score (dotted quarter = 70 in 6/8)",
              info.get("tempo_bpm") == 105 and info.get("measures") == 8 and "BD|o-----------|o-----------|o-----------|o-----------|" in info.get("drum_tab", ""),
              text[:800])

        # a drummer who drifts (120, 124, 117, 121): the score keeps with the recording, its chart shows one tempo
        drifting = folder / "ai_drift.kitnote"
        drifting.unlink(missing_ok=True)
        client.call("create_score", path=str(drifting), title="Drift", tempo=120, measures=16)
        client.call("fill_pattern", path=str(drifting), pattern="rock-8", first_measure=1, last_measure=16)
        client.call("edit_score", path=str(drifting), commands=["@5 tempo:124", "@9 tempo:117", "@13 tempo:121"])
        drift_wav = folder / "ai_drift.wav"
        client.call("export_score", path=str(drifting), output_path=str(drift_wav))
        drift_back = folder / "ai_drift_back.kitnote"
        drift_back.unlink(missing_ok=True)
        error, text = client.call("transcribe_audio", audio_path=str(drift_wav), output_path=str(drift_back), separate=False)
        info = json.loads(text[text.index("{"):]) if not error else {}
        check("transcribe_audio: the score follows the recording's timing with hidden tempo changes, one tempo shown",
              info.get("tempo_follows_recording") is True and info.get("hidden_tempo_changes", 0) >= 2
              and "tempo_marks" not in info and "own timing" in text, text[:800])
        back_wav = folder / "ai_drift_back.wav"
        client.call("export_score", path=str(drift_back), output_path=str(back_wav))
        rate, played = wav_mono(back_wav)
        _, recorded = wav_mono(drift_wav)
        offset = info.get("recording", {}).get("offset_seconds", 0.0)
        # late in each passage (at one tempo the score would be a quarter of a second off by measure 9)
        lags = [lag_ms(played, recorded, rate, at, offset) for at in (7.5, 15.0, 23.5, 31.0)] if played and recorded else []
        check("transcribe_audio: the score keeps with its recording (within 20 ms)",
              len(lags) == 4 and all(abs(lag) <= 20 for lag in lags), str(lags))
    else:
        print("skip  transcribe_audio (the audio engine is not installed)")

    error, text = client.call("read_score", path=str(folder / "nothing.kitnote"))
    check("a missing score is an error, not a crash", error, text)

    client.close()
    print("mcp: all checks passed" if not failures else f"mcp: {len(failures)} failure(s)")
    return 0 if not failures else 1


if __name__ == "__main__":
    sys.exit(main())
