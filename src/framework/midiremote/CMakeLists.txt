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

kfw_create_module(kfw_midiremote ALIAS kfw::midiremote)

target_sources(kfw_midiremote PRIVATE
    midiremotemodule.cpp
    midiremotemodule.h
    midiremotetypes.h
    imidiremoteconfiguration.h
    imidiremote.h

    mmctypes.h
    immcdecoder.h
    immcdecoderfactory.h

    internal/midiremoteconfiguration.cpp
    internal/midiremoteconfiguration.h
    internal/midiremote.cpp
    internal/midiremote.h

    internal/mmcdecoder.cpp
    internal/mmcdecoder.h
    internal/mmcdecoderfactory.cpp
    internal/mmcdecoderfactory.h
)

if (KFW_MODULE_MIDIREMOTE_QML)
    add_subdirectory(qml/KitNote/MidiRemote)
endif()

if (KFW_MODULE_MIDIREMOTE_TESTS)
    add_subdirectory(tests)
endif()
