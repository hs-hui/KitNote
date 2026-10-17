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

install(FILES
      DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}manual
      )

# KitNote: the user guide that Help > KitNote user guide opens (tools/kitnote/make_manual.py writes it).
install(DIRECTORY kitnote
      DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}manual
      FILES_MATCHING PATTERN "*.html"
      )

if (IS_DIRECTORY en)
install (DIRECTORY
      en
      DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}manual
      )
endif (IS_DIRECTORY en)

if (IS_DIRECTORY de)
install (DIRECTORY
      de
      DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}manual
      )
endif (IS_DIRECTORY de)

