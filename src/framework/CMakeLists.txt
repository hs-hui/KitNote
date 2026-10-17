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

if (KFW_COMPILE_USE_PCH)
    add_subdirectory(${KFW_FRAMEWORK_PATH}/buildscripts/pch pch)
endif()

# should be first to work pch
if (KFW_MODULE_GLOBAL)
    add_subdirectory(global)
endif()

# modules need to be listed in dependency order
if (KFW_MODULE_UI)
    add_subdirectory(ui)
    add_subdirectory(uicomponents)

    if (KFW_MODULE_UI_QML)
        # This sub-part of the UI module depends on the `uicomponents` module,
        # so needs to be declared after it.
        add_subdirectory(ui/qml/KitNote/Ui/Dialogs)
    endif()
endif()

if (KFW_MODULE_DOCKWINDOW)
    add_subdirectory(dockwindow)
endif()

if (KFW_MODULE_ACCESSIBILITY)
    add_subdirectory(accessibility)
endif()

if (KFW_MODULE_ACTIONS)
    add_subdirectory(actions)
endif()

if (KFW_MODULE_AUDIO)
    add_subdirectory(audio)
endif()

if (KFW_MODULE_AUDIOPLUGINS)
    add_subdirectory(audioplugins)
endif()

if (KFW_MODULE_DIAGNOSTICS)
    add_subdirectory(diagnostics)
endif()

if (KFW_MODULE_DRAW)
    add_subdirectory(draw)
endif()

if (KFW_MODULE_MIDI)
    add_subdirectory(midi)
endif()

if (KFW_MODULE_MIDIREMOTE)
    add_subdirectory(midiremote)
endif()

if (KFW_MODULE_MPE)
    add_subdirectory(mpe)
endif()

if (KFW_MODULE_NETWORK)
    add_subdirectory(network)
endif()

if (KFW_MODULE_SHORTCUTS)
    add_subdirectory(shortcuts)
endif()

if (KFW_MODULE_TOURS)
    add_subdirectory(tours)
endif()

if (KFW_MODULE_MULTIWINDOWS)
    add_subdirectory(multiwindows)
endif()

if (KFW_MODULE_LANGUAGES)
    add_subdirectory(languages)
endif()

if (KFW_MODULE_MEDIA)
    add_subdirectory(media)
endif()

if (KFW_MODULE_WORKSPACE)
    add_subdirectory(workspace)
endif()

if (KFW_MODULE_VST)
    add_subdirectory(vst)
endif()

# Tests
if (KFW_ENABLE_UNIT_TESTS)
    define_property(TARGET PROPERTY OUTPUT_XML
        BRIEF_DOCS "List XML files outputted by google test."
        FULL_DOCS "List XML files outputted by google test."
    )

    set(INSTALL_GTEST OFF)
    add_subdirectory(testing/thirdparty/googletest)
endif()

# Stubs
add_subdirectory(stubs)
