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

kfw_create_module(kfw_uicomponents ALIAS kfw::uicomponents)

target_sources(kfw_uicomponents PRIVATE
    uicomponentsmodule.cpp
    uicomponentsmodule.h
    view/modelutils.h
    view/topleveldialog.cpp
    view/topleveldialog.h
    view/widgets/radiobuttongroupbox.cpp
    view/widgets/radiobuttongroupbox.h
)

target_link_libraries(kfw_uicomponents PRIVATE Qt::Widgets Qt::Quick Qt::QmlIntegration)

if (KFW_MODULE_UI_QML)
    add_subdirectory(qml/KitNote/UiComponents)
    add_subdirectory(qml/KitNote/GraphicalEffects)
endif()

