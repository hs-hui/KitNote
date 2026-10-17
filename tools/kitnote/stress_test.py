#!/usr/bin/env python3
"""Runs every drum pattern and rudiment, chained with the other drum commands, on every bundled template.

A longer companion to smoke_test.py (about seven minutes): it looks for crashes and for commands that
do not undo each other, not for what exactly is written. For each template and pattern KitNote is asked
to fill the score with the pattern, count it, add the drum key, replace repeated measures with repeat
signs, add sticking, write the signs out again, renew the key, remove the key and fill the score once
more. What comes out must be the same score as the pattern alone gives.

    python tools/kitnote/stress_test.py [path\\to\\KitNote.exe]
"""
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
EXE = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "build.install" / "bin" / "KitNote.exe"

PATTERNS = ["rock-8", "rock-8-push", "rock-16", "quarter", "four-floor", "half-time", "ride-8", "crash-rock-8",
            "motown", "shuffle", "triplet-feel", "jazz-ride", "disco", "bossa-nova", "reggae", "six-eight",
            "snare-fill", "tom-fill", "triplet-fill", "half-bar-fill",
            "single-stroke", "double-stroke", "paradiddle", "stroke-triplets", "double-paradiddle",
            "paradiddle-diddle", "flams", "flam-tap", "flam-accent"]
TAGS = ("Measure", "Note", "Chord", "Rest", "Tuplet", "Sticking", "Lyrics", "MeasureRepeat", "HBox", "irregular", "Tempo")


def counts(source: Path, commands: str, folder: Path):
    """The element counts of the score after the commands, or None when KitNote wrote nothing."""
    out = folder / "out.mscx"
    out.unlink(missing_ok=True)
    try:
        subprocess.run([str(EXE), "-o", str(out), str(source)], env=dict(os.environ, KITNOTE_TEST_APPLY=commands),
                       capture_output=True, timeout=300)
    except subprocess.TimeoutExpired:
        return None
    if not out.exists():
        return None
    text = out.read_text(encoding="utf-8")
    return {tag: len(re.findall(f"<{tag}[ >/]", text)) for tag in TAGS}


def main() -> int:
    if not EXE.exists():
        print(f"{EXE} not found")
        return 2

    templates = sorted((EXE.parent.parent / "templates").rglob("*.mscx"))
    if not templates:
        print("no templates found next to the executable")
        return 2

    failures = 0
    with tempfile.TemporaryDirectory() as tmp:
        folder = Path(tmp)
        for template in templates:
            for pattern in PATTERNS:
                fill = f"pattern:{pattern}"
                plain = counts(template, fill, folder)
                chained = counts(template, f"{fill};counting;key;repeats;sticking-l;write-out;key;key-remove;{fill}", folder)
                if plain is None or chained is None:
                    # A kit without the drums of the pattern (the marching battery) is refused with a
                    # message and left as it was: then both runs give the untouched score.
                    failures += 1
                    print(f"FAIL  {template.stem}: {pattern}: no output")
                elif plain != chained:
                    failures += 1
                    print(f"FAIL  {template.stem}: {pattern}: {chained} != {plain}")
            print(f"done  {template.stem}")

    print("all runs passed" if not failures else f"{failures} failure(s)")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
