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

# KitNote: when the percussion-only SoundFont has been generated with
# tools/kitnote/make_drum_soundfont.py, it is installed in place of the full
# 50 MB one. It keeps the name the audio engine looks for.
if (KITNOTE_INSTALL_SOUNDFONT AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/KitNote Drums.sf3")
    install(FILES "KitNote Drums.sf3"
        DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}sound
        RENAME "MS Basic.sf3")
    install(FILES "MS Basic_License.md"
        DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}sound)
    set(KITNOTE_INSTALL_SOUNDFONT OFF)
endif()

if (KITNOTE_INSTALL_SOUNDFONT)

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/MS Basic.sf3")
      install (FILES FluidR3Mono_GM.sf3
          DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}sound
          RENAME "MS Basic.sf3"
          )
    else ()
      install(FILES
          "MS Basic.sf3"
          DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}sound
          )
    endif ()

    if (NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/MS Basic_License.md")
      install (FILES FluidR3Mono_License.md
          DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}sound
          RENAME "MS Basic_License.md")
    else ()
      install(FILES
          "MS Basic_License.md"
          DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}sound
          )
    endif ()

endif (KITNOTE_INSTALL_SOUNDFONT)

