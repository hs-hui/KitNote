#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
"""Checks that KitNote exits cleanly when it is told to quit while the "save changes?" question is open (it
crashed then: the question's handlers wrote into a function that had returned). Opens a score, changes it,
asks to quit, and ends the application from the test driver (KITNOTE_TEST_UI) with the question still up.
A window opens for about half a minute.

    python tools/kitnote/quit_check.py [build.install/bin/KitNote.exe]

The session, the recent scores and the settings of the profile are put back as they were afterwards.
"""
import os
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXE = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "build.install" / "bin" / "KitNote.exe"
KEEP = [Path(os.path.expandvars(p)) for p in (r"%LOCALAPPDATA%\KitNote\KitNote\session\session.json",
                                              r"%LOCALAPPDATA%\KitNote\KitNote\session\new_project.kitnote",
                                              r"%LOCALAPPDATA%\KitNote\KitNote\recent_files.json",
                                              r"%APPDATA%\KitNote\KitNote.ini")]


def main() -> int:
    saved = {p: p.read_bytes() if p.exists() else None for p in KEEP}
    with tempfile.TemporaryDirectory() as folder:
        folder = Path(folder)
        score = folder / "quit check.kitnote"
        # a plain score: the drum kit template with a groove in it (KitNote's command line makes it)
        template = ROOT / "share" / "templates" / "01-Drums" / "01-Drum_Kit" / "01-Drum_Kit.mscx"
        made = subprocess.run([str(EXE), "-o", str(score), str(template)], capture_output=True, timeout=120,
                              env=dict(os.environ, KITNOTE_APPLY="pattern:rock-8"))
        if not score.exists():
            print("FAIL could not make a score:", made.returncode, made.stderr.decode(errors="replace")[-300:])
            return 1
        before = score.read_bytes()
        script = folder / "script.txt"
        shot = folder / "question.png"
        script.write_text("\n".join([
            "wait 1500",
            "key Ctrl+A",          # a change: the score needs saving
            "key Delete",
            "wait 800",
            "do quit",             # the question comes up
            "wait 2000",
            f"shot {shot.as_posix()}",
            "quit",                # the application ends with the question open
        ]) + "\n", encoding="utf-8")
        try:
            run = subprocess.run([str(EXE), str(score)], env=dict(os.environ, KITNOTE_TEST_UI=str(script)),
                                 capture_output=True, timeout=150)
            code = run.returncode
        except subprocess.TimeoutExpired:
            code = None
        finally:
            for path, data in saved.items():
                if data is None:
                    if path.exists():
                        path.unlink()
                else:
                    path.write_bytes(data)
        # (the picture is of the front window: the question, far smaller than the main window)
        width = int.from_bytes(shot.read_bytes()[16:20], "big") if shot.exists() else 0
        failed = 0
        for name, ok, detail in (("the question was up", 0 < width < 900, f"front window {width} wide"),
                                 ("exits without crashing", code == 0, f"exit code {code}"),
                                 ("the score was not saved without asking", score.read_bytes() == before, "")):
            print("ok  " if ok else "FAIL", name, "" if ok else detail)
            failed += not ok
        return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
