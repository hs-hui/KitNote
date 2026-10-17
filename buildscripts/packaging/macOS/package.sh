#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2021 MuseScore Limited and others
# Modified for KitNote, 2026
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
# Turns the installed KitNote.app (ninja install into applebuild/) into
# KitNote-<version>-macos-<arch>.dmg.
#
#   - checks that kitnote-mcp is in Contents/MacOS (the install puts it there; --mcp copies another one in)
#   - runs macdeployqt (Qt frameworks and plugins, the Quick Look extension)
#   - signs every nested item first, then the app:
#       KITNOTE_CODESIGN_IDENTITY set   Developer ID Application, hardened runtime, timestamp
#       KITNOTE_CODESIGN_IDENTITY empty ad-hoc signature ("-"), which is enough to run on Apple
#                                       silicon after the user allows the app once
#   - notarizes and staples the app and the disk image when KITNOTE_APPLE_ID,
#     KITNOTE_APPLE_TEAM_ID and KITNOTE_APPLE_APP_PASSWORD are all set (and a Developer ID is used)
#
# Usage: buildscripts/packaging/macOS/package.sh [--app <KitNote.app>] [--mcp <kitnote-mcp>]
#                                                [--out <folder>] [--dmg-name <name.dmg>]
# Environment: QT_ROOT_DIR (the Qt 6.10 macos folder; for macdeployqt and the offscreen plugin)

set -euo pipefail

die() { echo "error: $*" >&2; exit 1; }

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
cd "$REPO_ROOT"

APP="applebuild/KitNote.app"
MCP=""                          # empty: the kitnote-mcp that the install put in the bundle
OUT_DIR="build.artifacts"
DMG_NAME=""
VOL_NAME="KitNote"

while [[ "$#" -gt 0 ]]; do
    case $1 in
        --app) APP="$2"; shift ;;
        --mcp) MCP="$2"; shift ;;
        --out) OUT_DIR="$2"; shift ;;
        --dmg-name) DMG_NAME="$2"; shift ;;
        --vol-name) VOL_NAME="$2"; shift ;;
        *) die "unknown parameter: $1" ;;
    esac
    shift
done

[ -d "$APP" ] || die "$APP not found: build and install KitNote first"

APP_BIN="$APP/Contents/MacOS/KitNote"
APPEX="$APP/Contents/PlugIns/KitNoteQuickLookPreviewExtension.appex"
MCP_IN_APP="$APP/Contents/MacOS/kitnote-mcp"
if [ -n "$MCP" ]; then
    [ -x "$MCP" ] || die "$MCP not found"
else
    [ -x "$MCP_IN_APP" ] || die "$MCP_IN_APP not found: build and install KitNote first"
fi
mkdir -p "$OUT_DIR"

# Version from version.cmake, the same way package_windows.ps1 reads it
version_part() { sed -n "s/^set(KITNOTE_APP_VERSION_$1 \"\([0-9]*\)\")/\1/p" version.cmake; }
VERSION="$(version_part MAJOR).$(version_part MINOR).$(version_part PATCH)"

# Architecture from the binary: universal, arm64 or x86_64
ARCHS="$(lipo -archs "$APP_BIN")"
if [[ "$ARCHS" == *" "* ]]; then ARCH_LABEL="universal"; else ARCH_LABEL="$ARCHS"; fi

if [ -z "$DMG_NAME" ]; then DMG_NAME="KitNote-${VERSION}-macos-${ARCH_LABEL}.dmg"; fi
FINAL_DMG="$OUT_DIR/$DMG_NAME"
echo "Package KitNote $VERSION ($ARCHS) as $FINAL_DMG"

SIGN_ID="${KITNOTE_CODESIGN_IDENTITY:-}"
if [ -n "$SIGN_ID" ]; then SIGN_LABEL="$SIGN_ID"; else SIGN_ID="-"; SIGN_LABEL="ad-hoc"; fi
NOTARIZE=false
if [ "$SIGN_ID" != "-" ] && [ -n "${KITNOTE_APPLE_ID:-}" ] && [ -n "${KITNOTE_APPLE_TEAM_ID:-}" ] && [ -n "${KITNOTE_APPLE_APP_PASSWORD:-}" ]; then
    NOTARIZE=true
fi
echo "Signing: $SIGN_LABEL; notarization: $NOTARIZE"

################################################################
# Assemble the bundle
################################################################

if [ -n "$MCP" ]; then
    echo "Copy $MCP to Contents/MacOS"
    cp -p "$MCP" "$MCP_IN_APP"
fi

# The offscreen platform plugin lets KitNote convert files without a display (smoke test, server use).
if [ -n "${QT_ROOT_DIR:-}" ] && [ -f "$QT_ROOT_DIR/plugins/platforms/libqoffscreen.dylib" ]; then
    mkdir -p "$APP/Contents/PlugIns/platforms"
    cp -p "$QT_ROOT_DIR/plugins/platforms/libqoffscreen.dylib" "$APP/Contents/PlugIns/platforms/"
fi

MACDEPLOYQT=""
if [ -n "${QT_ROOT_DIR:-}" ] && [ -x "$QT_ROOT_DIR/bin/macdeployqt" ]; then
    MACDEPLOYQT="$QT_ROOT_DIR/bin/macdeployqt"
elif command -v macdeployqt >/dev/null 2>&1; then
    MACDEPLOYQT="$(command -v macdeployqt)"
else
    die "macdeployqt not found: set QT_ROOT_DIR to the Qt 6.10.2 macos folder"
fi

echo "macdeployqt"
# -executable: the Quick Look extension and kitnote-mcp load Qt too, so their dependencies are copied as well.
"$MACDEPLOYQT" "$APP" \
    -verbose=2 \
    -qmldir=src \
    -executable="$APPEX/Contents/MacOS/KitNoteQuickLookPreviewExtension" \
    -executable="$MCP_IN_APP"

echo "Remove dSYM files"
find "$APP/Contents" -name "*.dSYM" -prune -exec rm -rf {} +

echo "Check that no build-machine library is linked"
if otool -L "$APP_BIN" "$MCP_IN_APP" | grep -E '/opt/homebrew/|/usr/local/(opt|Cellar)/|/Users/' ; then
    die "KitNote links a library outside the bundle (see above)"
fi

################################################################
# Sign
################################################################

sign_items() {
    local -a common=(--force -s "$SIGN_ID")
    local -a hardened=()
    if [ "$SIGN_ID" != "-" ]; then
        common+=(--timestamp)
        hardened=(--options runtime)
    fi

    # Nested code first: dylibs, frameworks, the Quick Look extension, kitnote-mcp, then the app.
    # (/bin/bash on macOS is 3.2: an empty array under set -u needs the ${a[@]+...} form.)
    while IFS= read -r -d '' f; do
        codesign "${common[@]}" ${hardened[@]+"${hardened[@]}"} "$f"
    done < <(find "$APP/Contents" -name "*.dylib" -print0)

    while IFS= read -r -d '' f; do
        codesign "${common[@]}" ${hardened[@]+"${hardened[@]}"} "$f"
    done < <(find "$APP/Contents/Frameworks" -maxdepth 1 -name "*.framework" -print0)

    codesign "${common[@]}" ${hardened[@]+"${hardened[@]}"} --entitlements "$REPO_ROOT/src/macos_integration/entitlements.plist" "$APPEX"
    codesign "${common[@]}" ${hardened[@]+"${hardened[@]}"} "$MCP_IN_APP"
    codesign "${common[@]}" ${hardened[@]+"${hardened[@]}"} --entitlements "$REPO_ROOT/buildscripts/packaging/macOS/entitlements.plist" "$APP"
}

echo "Sign the app ($SIGN_LABEL)"
sign_items
codesign --verify --deep --strict --verbose=2 "$APP"

# Submits a file to the notary service and fails unless it is accepted.
notarize() {
    local file="$1"
    echo "Notarize $(basename "$file")"
    local json id="" status="" rc=0
    # notarytool exits non-zero for a rejected submission too: keep its output, then report the status
    json="$(xcrun notarytool submit "$file" \
        --apple-id "$KITNOTE_APPLE_ID" \
        --team-id "$KITNOTE_APPLE_TEAM_ID" \
        --password "$KITNOTE_APPLE_APP_PASSWORD" \
        --wait --output-format json)" || rc=$?
    echo "$json"
    id="$(python3 -c 'import json,sys; print(json.load(sys.stdin).get("id",""))' <<<"$json" 2>/dev/null || true)"
    status="$(python3 -c 'import json,sys; print(json.load(sys.stdin).get("status",""))' <<<"$json" 2>/dev/null || true)"
    if [ "$rc" -ne 0 ] || [ "$status" != "Accepted" ]; then
        [ -n "$id" ] && xcrun notarytool log "$id" \
            --apple-id "$KITNOTE_APPLE_ID" --team-id "$KITNOTE_APPLE_TEAM_ID" --password "$KITNOTE_APPLE_APP_PASSWORD" || true
        die "notarization of $(basename "$file") ended with status: ${status:-unknown}"
    fi
}

WORK_DIR="$(mktemp -d "${TMPDIR:-/tmp}/kitnote-dmg.XXXXXX")"
DEV=""
cleanup() {
    if [ -n "$DEV" ]; then hdiutil detach -force "$DEV" >/dev/null 2>&1 || true; fi
    rm -rf "$WORK_DIR"
}
trap cleanup EXIT

if [ "$NOTARIZE" = true ]; then
    ditto -c -k --keepParent "$APP" "$WORK_DIR/app-notarization.zip"
    notarize "$WORK_DIR/app-notarization.zip"
    xcrun stapler staple "$APP"
    xcrun stapler validate "$APP"
else
    echo "Skipping notarization"
fi

################################################################
# Disk image
################################################################

RW_DMG="$WORK_DIR/rw.dmg"
# The app plus room for the Finder layout files and the file system
SIZE_MB=$(( $(du -sm "$APP" | cut -f1) + 200 ))

echo "Create the disk image (${SIZE_MB} MB)"
hdiutil create -size "${SIZE_MB}m" -fs HFS+ -volname "$VOL_NAME" -ov "$RW_DMG" >/dev/null

echo "Mount the disk image"
read -r DEV MNT < <(hdiutil attach "$RW_DMG" -readwrite -noverify -noautoopen -nobrowse -plist | python3 -c '
import plistlib, sys
entities = plistlib.load(sys.stdin.buffer)["system-entities"]
dev = next(e["dev-entry"] for e in entities if "dev-entry" in e)
mnt = next(e["mount-point"] for e in entities if "mount-point" in e)
print(dev, mnt)
')
[ -n "$MNT" ] || die "the disk image did not mount"

echo "Copy KitNote.app and the Applications link"
ditto "$APP" "$MNT/$(basename "$APP")"
ln -s /Applications "$MNT/Applications"

echo "Set the Finder layout (best effort)"
mkdir -p "$MNT/Pictures"
cp "$REPO_ROOT/buildscripts/packaging/macOS/kitnote-dmg-background.tiff" "$MNT/Pictures/background.tiff"
osascript <<EOF >/dev/null 2>&1 || echo "warning: the Finder layout was not applied"
tell application "Finder"
    set f to POSIX file ("$MNT" as string) as alias
    tell folder f
        open
        tell container window
            set toolbar visible to false
            set statusbar visible to false
            set current view to icon view
            delay 1
            set the bounds to {0, 0, 589, 435}
        end tell
        delay 1
        set icon size of the icon view options of container window to 120
        set arrangement of the icon view options of container window to not arranged
        set position of item "$(basename "$APP")" to {150, 200}
        close
        set position of item "Applications" to {439, 200}
        open
        set background picture of the icon view options of container window to file "background.tiff" of folder "Pictures"
        set the bounds of the container window to {0, 0, 589, 435}
        update without registering applications
        delay 5
        close
    end tell
    delay 5
end tell
EOF
# Hidden, so the background does not show in the window; the Finder layout keeps pointing at it.
mv "$MNT/Pictures" "$MNT/.Pictures"
sync

echo "Unmount"
for i in $(seq 1 20); do
    if hdiutil detach "$DEV" >/dev/null 2>&1; then DEV=""; break; fi
    if [ "$i" -eq 20 ]; then die "could not unmount $DEV"; fi
    sleep 15
done

echo "Compress the disk image"
rm -f "$FINAL_DMG"
hdiutil convert "$RW_DMG" -format UDZO -imagekey zlib-level=9 -ov -o "$FINAL_DMG" >/dev/null

if [ "$SIGN_ID" != "-" ]; then
    echo "Sign the disk image"
    codesign --force --timestamp -s "$SIGN_ID" "$FINAL_DMG"
    codesign --verify --verbose=2 "$FINAL_DMG"
    if [ "$NOTARIZE" = true ]; then
        notarize "$FINAL_DMG"
        xcrun stapler staple "$FINAL_DMG"
        xcrun stapler validate "$FINAL_DMG"
    fi
fi

shasum -a 256 "$FINAL_DMG"
echo "DMG: $FINAL_DMG"
