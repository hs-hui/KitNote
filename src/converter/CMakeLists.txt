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

# This is module for convert mscz files to various formats (image, pdf, audio and etc).
# Main goal - determine what need to do and call necessary functions.
# Usually use from command line.

kfw_create_module(converter)

target_sources(converter PRIVATE
    convertermodule.cpp
    convertermodule.h
    convertercodes.h
    convertertypes.h
    iconvertercontroller.h

    internal/convertercontroller.cpp
    internal/convertercontroller.h
    internal/converterutils.cpp
    internal/converterutils.h
    internal/compat/backendapi.cpp
    internal/compat/backendapi.h
    internal/compat/backendjsonwriter.cpp
    internal/compat/backendjsonwriter.h
    internal/compat/backendtypes.h
    internal/compat/notationmeta.cpp
    internal/compat/notationmeta.h
    internal/compat/scoreelementsscanner.cpp
    internal/compat/scoreelementsscanner.h
)

if (KITNOTE_BUILD_CONVERTER_TESTS)
    add_subdirectory(tests)
endif()
