#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2026 KitNote contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
# Checks the installed KitNote.app without a display: the bundle layout, the signature,
# the architectures, the version output, kitnote-mcp, and a drum template converted to PDF
# with the offscreen platform plugin.
#
# Usage: buildscripts/ci/macos/smoke_test.sh [path/to/KitNote.app]

set -uo pipefail

APP="${1:-applebuild/KitNote.app}"
FAILURES=0

check() {
    local name="$1"; shift
    if "$@" >/dev/null 2>&1; then
        echo "ok:   $name"
    else
        echo "FAIL: $name"
        FAILURES=$((FAILURES + 1))
    fi
}

BIN="$APP/Contents/MacOS/KitNote"
MCP="$APP/Contents/MacOS/kitnote-mcp"
RES="$APP/Contents/Resources"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

echo "Smoke test: $APP"
check "KitNote.app exists" test -d "$APP"
check "KitNote executable exists" test -x "$BIN"
check "kitnote-mcp is in Contents/MacOS" test -x "$MCP"
check "Quick Look extension is in PlugIns" test -d "$APP/Contents/PlugIns/KitNoteQuickLookPreviewExtension.appex"
check "drum SoundFont is installed" test -f "$RES/sound/MS Basic.sf3"
check "AI skill is installed" test -f "$RES/ai/SKILL.md"
check "audio engine script is installed" test -f "$RES/ai/kitnote_audio.py"
check "app icon is in the bundle" bash -c "test -f '$RES/kitnote_AppIcon.icns' || test -f '$RES/Assets.car'"
check "Info.plist bundle identifier" bash -c "/usr/libexec/PlistBuddy -c 'Print :CFBundleIdentifier' '$APP/Contents/Info.plist' | grep -qx org.kitnote.KitNote"
check "no Homebrew or build-machine library is linked" bash -c "! otool -L '$BIN' '$MCP' | grep -E '/opt/homebrew/|/usr/local/(opt|Cellar)/|/Users/'"

echo "Architectures: $(lipo -archs "$BIN")"
check "code signature is valid" codesign --verify --deep --strict "$APP"

check "KitNote --version" bash -c "'$BIN' --version | grep -q KitNote"
check "kitnote-mcp --version" bash -c "'$MCP' --version | grep -q kitnote-mcp"

TEMPLATE="$(find "$RES/templates" -name '*.mscx' | sort | head -n 1)"
if [ -z "$TEMPLATE" ]; then
    echo "FAIL: no drum template in $RES/templates"
    FAILURES=$((FAILURES + 1))
else
    echo "Convert $(basename "$TEMPLATE") to PDF (offscreen)"
    QT_QPA_PLATFORM=offscreen "$BIN" -o "$TMP/template.pdf" "$TEMPLATE" >"$TMP/convert.log" 2>&1
    check "template converted to PDF" bash -c "test -s '$TMP/template.pdf' && head -c 5 '$TMP/template.pdf' | grep -q '%PDF-'"
    if [ -s "$TMP/convert.log" ]; then
        echo "convert output:"; sed -n '1,40p' "$TMP/convert.log"
    fi
fi

if [ "$FAILURES" -gt 0 ]; then
    echo "$FAILURES check(s) failed"
    exit 1
fi
echo "smoke test passed"
