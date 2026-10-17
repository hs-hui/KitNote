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
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

set(MODULE_TEST iex_mnx_tests)

option(KITNOTE_MNX_WRITE_REFS "Write MNX reference files instead of comparing" OFF)

set(MODULE_TEST_DEF
    KITNOTE_MNX_WRITE_REFS=$<IF:$<BOOL:${KITNOTE_MNX_WRITE_REFS}>,1,0>
)

set(MODULE_TEST_SRC
    ${PROJECT_SOURCE_DIR}/src/engraving/tests/utils/scorerw.cpp
    ${PROJECT_SOURCE_DIR}/src/engraving/tests/utils/scorerw.h
    ${PROJECT_SOURCE_DIR}/src/engraving/tests/utils/scorecomp.cpp
    ${PROJECT_SOURCE_DIR}/src/engraving/tests/utils/scorecomp.h

    ${CMAKE_CURRENT_LIST_DIR}/environment.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mnx_tests.cpp
)

set(MODULE_TEST_LINK
    engraving
    iex_mnx
    mnxdom
)

set(MODULE_TEST_DATA_ROOT ${CMAKE_CURRENT_LIST_DIR})

include(SetupGTest)
