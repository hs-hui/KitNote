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

kfw_create_module(kfw_tours ALIAS kfw::tours)

target_sources(kfw_tours PRIVATE
    toursmodule.cpp
    toursmodule.h
    tourstypes.h
    itoursconfiguration.h
    itoursservice.h
    itoursprovider.h

    internal/toursconfiguration.cpp
    internal/toursconfiguration.h
    internal/toursservice.cpp
    internal/toursservice.h
    internal/toursprovider.cpp
    internal/toursprovider.h
)

if (KFW_MODULE_TOURS_TESTS)
    # add_subdirectory(tests) # todo
endif()

if (KFW_MODULE_TOURS_QML)
    add_subdirectory(qml/KitNote/Tours)
endif()
