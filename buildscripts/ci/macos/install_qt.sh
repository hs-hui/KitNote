#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2025 MuseScore Limited and others
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
# Installs Qt 6.10.2 for macOS from the official Qt mirrors with aqtinstall, the same modules as the
# Windows build (see KITNOTE.md). Run after actions/setup-python. The Qt tools (lupdate, lrelease,
# macdeployqt) come with it.

echo "Install Qt"
trap 'code=$?; echo "error: Install Qt: command \`$BASH_COMMAND\` exited with code $code." >&2; exit 1' ERR
set -euo pipefail

# Pinned: aqtinstall 3.3.0 is the newest release on PyPI when this was written (2026-10).
AQT_VERSION="3.3.0"
QT_VERSION="6.10.2"
QT_ARCH="clang_64"              # universal (arm64 and x86_64) on macOS
QT_MODULES="qt5compat qtnetworkauth qtshadertools qtwebsockets"
QT_BASE_DIR="$HOME/Qt"
QT_DIR="$QT_BASE_DIR/$QT_VERSION/macos"

python3 -m pip install --disable-pip-version-check "aqtinstall==${AQT_VERSION}"

# Skip the download when the cache already has it
if [ ! -f "$QT_DIR/lib/cmake/Qt6/Qt6Config.cmake" ]; then
    python3 -m aqt install-qt mac desktop "$QT_VERSION" "$QT_ARCH" \
        --modules $QT_MODULES \
        --outputdir "$QT_BASE_DIR"
fi

[ -x "$QT_DIR/bin/macdeployqt" ] || { echo "error: macdeployqt not found in $QT_DIR/bin"; exit 1; }

echo "$QT_DIR/bin" >> "$GITHUB_PATH"
echo "QT_ROOT_DIR=$QT_DIR" >> "$GITHUB_ENV"
echo "CMAKE_PREFIX_PATH=$QT_DIR" >> "$GITHUB_ENV"
echo "QT_PLUGIN_PATH=$QT_DIR/plugins" >> "$GITHUB_ENV"
echo "QML2_IMPORT_PATH=$QT_DIR/qml" >> "$GITHUB_ENV"

echo "Install Qt done: $QT_DIR"
