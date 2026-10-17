# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2024 MuseScore Limited and others
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

if (KITNOTE_COMPILE_USE_SYSTEM_HARFBUZZ)
    find_package(HarfBuzz)

    if (HarfBuzz_FOUND)
        message(STATUS "Found HarfBuzz")

        # See HarfBuzz's harfbuzz-config.cmake, which is quite minimalistic
        set(HARFBUZZ_LIBRARIES harfbuzz::harfbuzz)
        set(HARFBUZZ_INCLUDE_DIRS ${HARFBUZZ_INCLUDE_DIR})

        return()
    else()
        message(WARNING "Set KITNOTE_COMPILE_USE_SYSTEM_HARFBUZZ=ON, but system harfbuzz not found, built-in will be used")
    endif()
endif()

# If not KITNOTE_COMPILE_USE_SYSTEM_HARFBUZZ, or if it was not found,
# build the vendored HarfBuzz 12.3.0 in thirdparty/harfbuzz (upstream source, no download).

set(HB_HAVE_FREETYPE ON)

add_subdirectory(${PROJECT_SOURCE_DIR}/thirdparty/harfbuzz thirdparty_harfbuzz)

set(HARFBUZZ_LIBRARIES harfbuzz)
set(HARFBUZZ_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/thirdparty/harfbuzz/src)
