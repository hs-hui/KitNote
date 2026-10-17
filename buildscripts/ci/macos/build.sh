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
# Builds and installs KitNote.app into applebuild/ (see ninja_build.sh).
# Needs Qt 6.10.2 (macos, see install_qt.sh) and QT_ROOT_DIR, CMAKE_PREFIX_PATH is set from it.
#
# KitNote: nothing is downloaded here. Like the Windows build, zlib, HarfBuzz, the ASIO SDK,
# opus, opusenc, flac, lame, fdk-aac and freetype come from thirdparty/ and src/framework/**/thirdparty.
#
# Environment (optional):
#   KITNOTE_MACOS_ARCHITECTURES   "arm64;x86_64" (default, universal) or "arm64" or "x86_64"
#   KITNOTE_MACOS_DEPLOYMENT_TARGET  overrides the default in buildscripts/cmake/SetupBuildEnvironment.cmake

echo "Build KitNote"
trap 'echo "Build failed"; exit 1' ERR
set -euo pipefail

ARTIFACTS_DIR=build.artifacts

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -n|--number) BUILD_NUMBER="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

if [ -z "${BUILD_NUMBER:-}" ]; then echo "error: not set BUILD_NUMBER"; exit 1; fi
if [ -z "${QT_ROOT_DIR:-}" ]; then echo "error: QT_ROOT_DIR is not set (see install_qt.sh)"; exit 1; fi

BUILD_MODE=$(cat $ARTIFACTS_DIR/env/build_mode.env)
KITNOTE_APP_BUILD_MODE=dev
if [ "$BUILD_MODE" == "devel" ]; then KITNOTE_APP_BUILD_MODE=dev; fi
if [ "$BUILD_MODE" == "nightly" ]; then KITNOTE_APP_BUILD_MODE=dev; fi
if [ "$BUILD_MODE" == "testing" ]; then KITNOTE_APP_BUILD_MODE=testing; fi
if [ "$BUILD_MODE" == "stable" ]; then KITNOTE_APP_BUILD_MODE=release; fi

KITNOTE_MACOS_ARCHITECTURES="${KITNOTE_MACOS_ARCHITECTURES:-arm64;x86_64}"

echo "KITNOTE_APP_BUILD_MODE: $KITNOTE_APP_BUILD_MODE"
echo "BUILD_NUMBER: $BUILD_NUMBER"
echo "ARCHITECTURES: $KITNOTE_MACOS_ARCHITECTURES"

KITNOTE_REVISION=$(git rev-parse --short=7 HEAD)

# The drum SoundFont that the build installs in place of the 50 MB MS Basic.sf3
python3 tools/kitnote/make_drum_soundfont.py "share/sound/MS Basic.sf3" "share/sound/KitNote Drums.sf3"

CMAKE_OSX_ARCHITECTURES="$KITNOTE_MACOS_ARCHITECTURES" \
KITNOTE_INSTALL_DIR="../applebuild" \
KITNOTE_APP_BUILD_MODE=$KITNOTE_APP_BUILD_MODE \
KITNOTE_BUILD_NUMBER=$BUILD_NUMBER \
KITNOTE_REVISION=$KITNOTE_REVISION \
KITNOTE_BUILD_VST_MODULE="OFF" \
KITNOTE_BUILD_WEBSOCKET="ON" \
KITNOTE_RUN_WINDEPLOYQT="OFF" \
KITNOTE_COMPILE_USE_UNITY="ON" \
bash ./ninja_build.sh -t install -j "$(sysctl -n hw.ncpu)"

bash ./buildscripts/ci/tools/make_release_channel_env.sh -c $KITNOTE_APP_BUILD_MODE
bash ./buildscripts/ci/tools/make_version_env.sh $BUILD_NUMBER
bash ./buildscripts/ci/tools/make_revision_env.sh $KITNOTE_REVISION
bash ./buildscripts/ci/tools/make_branch_env.sh
