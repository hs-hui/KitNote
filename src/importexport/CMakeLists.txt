# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2026 MuseScore Limited and others
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
# along with this program. If not, see <https://www.gnu.org/licenses/>.

if (KITNOTE_BUILD_IMPEXP_AUDIOEXPORT_MODULE)
    add_subdirectory(audioexport)
endif()
if (KITNOTE_BUILD_IMPEXP_BB_MODULE)
    add_subdirectory(bb)
endif()
if (KITNOTE_BUILD_IMPEXP_BWW_MODULE)
    add_subdirectory(bww)
endif()
if (KITNOTE_BUILD_IMPEXP_CAPELLA_MODULE)
    add_subdirectory(capella)
endif()
if (KITNOTE_BUILD_IMPEXP_GUITARPRO_MODULE)
    add_subdirectory(guitarpro)
endif()
if (KITNOTE_BUILD_IMPEXP_IMAGESEXPORT_MODULE)
    add_subdirectory(imagesexport)
endif()
if (KITNOTE_BUILD_IMPEXP_LYRICS_MODULE)
    add_subdirectory(lyricsexport)
endif()
if (KITNOTE_BUILD_IMPEXP_MEI_MODULE)
    add_subdirectory(mei)
else()
    add_subdirectory(../stubs/importexport/mei stubs/mei)
endif()
if (KITNOTE_BUILD_IMPEXP_MIDI_MODULE)
    add_subdirectory(midi)
else()
    add_subdirectory(../stubs/importexport/midi stubs/midi)
endif()
if (KITNOTE_BUILD_IMPEXP_MNX_MODULE)
    add_subdirectory(mnx)
else()
    add_subdirectory(../stubs/importexport/mnx stubs/mnx)
endif()
if (KITNOTE_BUILD_IMPEXP_MUSEDATA_MODULE)
    add_subdirectory(musedata)
endif()
if (KITNOTE_BUILD_IMPEXP_MUSICXML_MODULE)
    add_subdirectory(musicxml)
else()
    add_subdirectory(../stubs/importexport/musicxml stubs/musicxml)
endif()
if (KITNOTE_BUILD_IMPEXP_OVE_MODULE)
    add_subdirectory(ove)
else()
    add_subdirectory(../stubs/importexport/ove stubs/ove)
endif()
if (KITNOTE_BUILD_IMPEXP_TABLEDIT_MODULE)
    add_subdirectory(tabledit)
endif()
