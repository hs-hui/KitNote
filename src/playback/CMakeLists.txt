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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

kfw_create_module(playback)

target_sources(playback PRIVATE
    playbackmodule.cpp
    playbackmodule.h
    iplaybackcontroller.h
    iplaybackconfiguration.h
    isoundprofilesrepository.h
    playbacktypes.h
    kitnoterecording.cpp # KitNote
    kitnoterecording.h

    internal/playbackcontroller.cpp
    internal/playbackcontroller.h
    internal/onlinesoundscontroller.cpp
    internal/onlinesoundscontroller.h
    internal/playbackuiactions.cpp
    internal/playbackuiactions.h
    internal/playbackconfiguration.cpp
    internal/playbackconfiguration.h
    internal/soundprofilesrepository.cpp
    internal/soundprofilesrepository.h
    internal/drumsetloader.cpp
    internal/drumsetloader.h
)

if (KITNOTE_BUILD_PLAYBACK_TESTS)
    add_subdirectory(tests)
endif()

if (KITNOTE_BUILD_PLAYBACK_QML)
    add_subdirectory(qml/KitNote/Playback)
endif()
