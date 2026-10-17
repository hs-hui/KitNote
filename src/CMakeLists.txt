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

# Framework, should be first to work pch
add_subdirectory(framework)

# Modules must be listed in dependency order
if (KITNOTE_BUILD_BRAILLE_MODULE)
    add_subdirectory(braille)
else()
    add_subdirectory(stubs/braille)
endif()

add_subdirectory(context)

if (KITNOTE_BUILD_CONVERTER_MODULE)
    add_subdirectory(converter)
endif()

add_subdirectory(engraving)

add_subdirectory(importexport)

if (KITNOTE_BUILD_INSPECTOR_MODULE)
    add_subdirectory(inspector)
else()
    add_subdirectory(stubs/inspector)
endif()

if (KITNOTE_BUILD_INSTRUMENTSSCENE_MODULE)
    add_subdirectory(instrumentsscene)
else()
    add_subdirectory(stubs/instrumentsscene)
endif()

if (KITNOTE_BUILD_NOTATION_MODULE)
    add_subdirectory(notation)
else()
    add_subdirectory(stubs/notation)
endif()

if (KITNOTE_BUILD_NOTATIONSCENE_MODULE)
    add_subdirectory(notationscene)
else()
    add_subdirectory(stubs/notationscene)
endif()

if (KITNOTE_BUILD_PALETTE_MODULE)
    add_subdirectory(palette)
else()
    add_subdirectory(stubs/palette)
endif()

if (KITNOTE_BUILD_PLAYBACK_MODULE)
    add_subdirectory(playback)
else()
    add_subdirectory(stubs/playback)
endif()

if (KITNOTE_BUILD_PREFERENCES_MODULE)
    add_subdirectory(preferences)
endif()

if (KITNOTE_BUILD_PRINT_MODULE)
    add_subdirectory(print)
endif()

if (KITNOTE_BUILD_PROJECT_MODULE)
    add_subdirectory(project)
endif()

if (KITNOTE_BUILD_APPSHELL_MODULE)
    if (KITNOTE_CONFIGURATION_IS_APPWEB)
        add_subdirectory(web/appshell)
    else()
        add_subdirectory(appshell)
    endif()
endif()

# App (main)
add_subdirectory(app)

# KitNote: KitNote as an MCP server for AI tools
if (NOT KITNOTE_CONFIGURATION_IS_APPWEB)
    add_subdirectory(kitnotemcp)
endif()
if (KITNOTE_CONFIGURATION_IS_APPWEB)
    add_subdirectory(web/appjs)
endif()

if (KITNOTE_BUILD_MACOS_INTEGRATION AND OS_IS_MAC)
    add_subdirectory(macos_integration)
endif()
