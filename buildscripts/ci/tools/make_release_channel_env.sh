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
ARTIFACTS_DIR=build.artifacts
KITNOTE_APP_BUILD_MODE=dev

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -c|--build-mode) KITNOTE_APP_BUILD_MODE="$2"; shift ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

if [ -z "$KITNOTE_APP_BUILD_MODE" ]; then echo "error: not set KITNOTE_APP_BUILD_MODE"; exit 1; fi

echo "KITNOTE_APP_BUILD_MODE: $KITNOTE_APP_BUILD_MODE"

export KITNOTE_APP_RELEASE_CHANNEL=$(cmake -DKITNOTE_APP_BUILD_MODE=$KITNOTE_APP_BUILD_MODE -P version.cmake | sed -n -e 's/^.*KITNOTE_APP_RELEASE_CHANNEL  *//p')

echo ${KITNOTE_APP_RELEASE_CHANNEL} > $ARTIFACTS_DIR/env/release_channel.env
cat $ARTIFACTS_DIR/env/release_channel.env
