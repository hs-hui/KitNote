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

file(GLOB TS_FILES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/*.ts)

# KitNote: English and Korean are the only interface languages in the tree.
set(KITNOTE_QT_LANG_REGEX "ko")

if (KITNOTE_RUN_LRELEASE)
    add_custom_target(translations ALL
        COMMENT "Generating .qm files" VERBATIM
    )

    qt_add_lrelease(translations TS_FILES ${TS_FILES} NO_GLOBAL_TARGET QM_FILES_OUTPUT_VARIABLE QM_FILES)
endif()

# KitNote: the "en" catalogs are the source strings; only "en_US" and "ko" are selectable,
# and a language loads just the files that carry its own code.
if (QM_FILES)
    list(FILTER QM_FILES EXCLUDE REGEX "_en\\.qm$")
endif()

install(FILES
    ${CMAKE_CURRENT_LIST_DIR}/languages.json
    ${QM_FILES}
    DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}locale
)

install(DIRECTORY ${QT_INSTALL_PREFIX}/translations/
    DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}locale
    FILES_MATCHING
    REGEX "qt_help_.*\\.qm" EXCLUDE
    REGEX "qtbase_(${KITNOTE_QT_LANG_REGEX})\\.qm"
    REGEX "qt*_en\\.qm" EXCLUDE)
