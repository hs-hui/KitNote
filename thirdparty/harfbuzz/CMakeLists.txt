# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
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

# Vendored HarfBuzz 12.3.0 (https://github.com/harfbuzz/harfbuzz/releases/tag/12.3.0,
# harfbuzz-12.3.0.tar.xz, SHA-256 8660ebd3c27d9407fc8433b5d172bafba5f0317cb0bb4339f28e5370c93d42b7).
# The src/ folder is the upstream tree unchanged. Licence: MIT (see COPYING in this folder).
# Built as one unity file, with FreeType support, the same way the previous muse_deps package was built.

kfw_create_thirdparty_module(harfbuzz)

target_sources(harfbuzz PRIVATE
    src/harfbuzz.cc
)

target_include_directories(harfbuzz PRIVATE ${FREETYPE_INCLUDE_DIRS})

target_compile_definitions(harfbuzz PRIVATE HAVE_FREETYPE)

target_no_warning(harfbuzz -Wno-conversion)
target_no_warning(harfbuzz -Wno-unused-parameter)
target_no_warning(harfbuzz -Wno-unused-variable)
target_no_warning(harfbuzz -WMSVC-no-hides-previous)
target_no_warning(harfbuzz -WMSVC-no-unreachable)
target_no_warning(harfbuzz -Wno-truncate)
target_no_warning(harfbuzz -Wno-uninitialized)
