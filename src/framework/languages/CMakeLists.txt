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

kfw_create_module(kfw_languages ALIAS kfw::languages)

target_sources(kfw_languages PRIVATE
    languagesmodule.cpp
    languagesmodule.h

    languagestypes.h
    languageserrors.h

    ilanguagesconfiguration.h
    ilanguagesservice.h

    internal/languagesconfiguration.cpp
    internal/languagesconfiguration.h
    internal/languagesservice.cpp
    internal/languagesservice.h
)

if (KFW_QT_SUPPORT)
    target_link_libraries(kfw_languages PRIVATE Qt::Concurrent Qt::Gui Qt::Network Qt::Qml)
endif()
