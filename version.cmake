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

# KitNote is a drum-notation edition of MuseScore Studio. The names below
# also decide the settings and data folders, which keeps a KitNote install
# from sharing state with a MuseScore Studio install on the same machine.
set(KITNOTE_APP_NAME_HUMAN_READABLE "KitNote")
set(KITNOTE_APP_NAME_MACHINE_READABLE "KitNote")

set(KITNOTE_APP_NAME_HUMAN_READABLE_COMPAT "KitNote")
set(KITNOTE_APP_NAME_MACHINE_READABLE_COMPAT "KitNote")

set(KITNOTE_APP_GUI_IDENTIFIER org.kitnote.${KITNOTE_APP_NAME_MACHINE_READABLE_COMPAT})

# KitNote's own version.
set(KITNOTE_APP_VERSION_MAJOR "1")
set(KITNOTE_APP_VERSION_MINOR "0")
set(KITNOTE_APP_VERSION_PATCH "0")

# The MuseScore Studio release the engine comes from. Score files record this,
# not KitNote's version: both MuseScore and KitNote decide which compatibility
# fixes to apply to a file by comparing its program version with MuseScore's
# release numbers, and plugins check it the same way.
set(KITNOTE_ENGINE_VERSION "4.7.5")
add_compile_definitions(KITNOTE_ENGINE_VERSION="${KITNOTE_ENGINE_VERSION}")
set(KITNOTE_APP_VERSION_MAJ_MIN "${KITNOTE_APP_VERSION_MAJOR}.${KITNOTE_APP_VERSION_MINOR}")
set(KITNOTE_APP_VERSION "${KITNOTE_APP_VERSION_MAJ_MIN}.${KITNOTE_APP_VERSION_PATCH}")

set(KITNOTE_APP_VERSION_LABEL "")

if (NOT CMAKE_BUILD_NUMBER)
    set(CMAKE_BUILD_NUMBER "1")
endif()

# TODO: Remove
set(KITNOTE_APP_TITLE "${KITNOTE_APP_NAME_HUMAN_READABLE}")
set(KITNOTE_APP_NAME "${KITNOTE_APP_NAME_MACHINE_READABLE_COMPAT}")
set(KITNOTE_APP_TITLE_VERSION "${KITNOTE_APP_TITLE} ${KITNOTE_APP_VERSION_MAJOR}")
set(KITNOTE_APP_NAME_VERSION "${KITNOTE_APP_NAME} ${KITNOTE_APP_VERSION_MAJOR}")
# ---

set(KITNOTE_APP_UNSTABLE ON)
set(KITNOTE_APP_IS_PRERELEASE ON)

if(KITNOTE_APP_BUILD_MODE MATCHES "dev")
    set(KITNOTE_APP_RELEASE_CHANNEL "devel")
endif()

if(KITNOTE_APP_BUILD_MODE MATCHES "testing")
    set(KITNOTE_APP_RELEASE_CHANNEL "testing")
endif()

if(KITNOTE_APP_BUILD_MODE MATCHES "release")
    set(KITNOTE_APP_RELEASE_CHANNEL "stable")
endif()

# Print variables which are needed by CI build scripts.
message(STATUS "KITNOTE_APP_RELEASE_CHANNEL ${KITNOTE_APP_RELEASE_CHANNEL}")
message(STATUS "KITNOTE_APP_VERSION ${KITNOTE_APP_VERSION}")
