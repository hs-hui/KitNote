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

# adding or renaming templates here needs to be reflected in ../instruments/instrumentsxml.h,
# like by running ../instruments/update_instruments_xml.py

install(FILES
      Marching_Bass_Drums.drm
      Marching_Cymbals.drm
      Marching_Snare_Drums.drm
      Marching_Tenors.drm
      drumset_fr.drm
      orchestral.drm
      My_First_Score.mscx
      categories.json
      DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}templates
      )
# KitNote ships drum templates only.
install(DIRECTORY
      01-Drums/01-Drum_Kit
      01-Drums/02-Battery_Percussion
      01-Drums/03-Drum_Kit_4-piece
      01-Drums/04-Drum_Kit_large
      DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}templates/01-Drums
      )
