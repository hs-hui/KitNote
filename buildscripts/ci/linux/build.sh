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
echo "Build Linux KitNote AppImage"

#set -x
trap 'echo Build failed; exit 1' ERR

df -h .

BUILD_TOOLS=$HOME/build_tools
ARTIFACTS_DIR=build.artifacts
BUILD_NUMBER=""
BUILD_MODE=""
SUFFIX="" # appended to `mscore` command name to avoid conflicts (e.g. `mscoredev`)
BUILD_PIPEWIRE=OFF

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -n|--number) BUILD_NUMBER="$2"; shift ;;
        --build_mode) BUILD_MODE="$2"; shift ;;
        --arch) PACKARCH="$2"; shift ;;
        --build-pipewire) BUILD_PIPEWIRE=ON ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

if [ -z "$BUILD_NUMBER" ]; then echo "error: not set BUILD_NUMBER"; exit 1; fi
if [ -z "$BUILD_MODE" ]; then BUILD_MODE=$(cat $ARTIFACTS_DIR/env/build_mode.env); fi

KITNOTE_APP_BUILD_MODE=dev

case "${BUILD_MODE}" in
"devel")   KITNOTE_APP_BUILD_MODE=dev; SUFFIX=dev;;
"nightly") KITNOTE_APP_BUILD_MODE=dev; SUFFIX=nightly;;
"testing") KITNOTE_APP_BUILD_MODE=testing; SUFFIX=testing;;
"stable")  KITNOTE_APP_BUILD_MODE=release; SUFFIX="";;
esac

echo "KITNOTE_APP_BUILD_MODE: $KITNOTE_APP_BUILD_MODE"
echo "BUILD_NUMBER: $BUILD_NUMBER"
echo "BUILD_MODE: $BUILD_MODE"

echo "=== ENVIRONMENT === "

cat $BUILD_TOOLS/environment.sh
source $BUILD_TOOLS/environment.sh

# disable update module due to current broken functionality
if [ "$PACKARCH" == "aarch64" ] || [ "$PACKARCH" == "armv7l" ]; then
  KITNOTE_BUILD_UPDATE_MODULE="OFF"
fi

echo "=== BUILD ==="

KITNOTE_REVISION=$(git rev-parse --short=7 HEAD)

# Build portable AppImage
KITNOTE_APP_BUILD_MODE=$KITNOTE_APP_BUILD_MODE \
KITNOTE_APP_INSTALL_SUFFIX=$SUFFIX \
KITNOTE_BUILD_NUMBER=$BUILD_NUMBER \
KITNOTE_REVISION=$KITNOTE_REVISION \
KITNOTE_BUILD_UPDATE_MODULE=${KITNOTE_BUILD_UPDATE_MODULE:-"ON"} \
KITNOTE_BUILD_VST_MODULE="ON" \
KITNOTE_BUILD_WEBSOCKET="ON" \
KITNOTE_BUILD_PIPEWIRE_AUDIO_DRIVER=$BUILD_PIPEWIRE \
bash ./ninja_build.sh -t appimage


bash ./buildscripts/ci/tools/make_release_channel_env.sh -c $KITNOTE_APP_BUILD_MODE
bash ./buildscripts/ci/tools/make_version_env.sh $BUILD_NUMBER
bash ./buildscripts/ci/tools/make_revision_env.sh $KITNOTE_REVISION
bash ./buildscripts/ci/tools/make_branch_env.sh

df -h .
