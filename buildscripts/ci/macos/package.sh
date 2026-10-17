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
# Packs applebuild/KitNote.app into build.artifacts/KitNote-<version>-macos-<arch>.dmg.
# Signing and notarization happen only when the signing secrets are set in the environment
# (see import_signing_certificate.sh); otherwise the app is ad-hoc signed.

echo "Package KitNote"
trap 'echo "Package failed"; exit 1' ERR
set -euo pipefail

ARTIFACTS_DIR="build.artifacts"

bash ./buildscripts/packaging/macOS/package.sh --out "$ARTIFACTS_DIR"

DMG_FILE="$(ls "$ARTIFACTS_DIR"/KitNote-*-macos-*.dmg)"
DMG_NAME="$(basename "$DMG_FILE")"
echo "DMG_FILE: $DMG_FILE"

bash ./buildscripts/ci/tools/make_artifact_name_env.sh "$DMG_NAME"
