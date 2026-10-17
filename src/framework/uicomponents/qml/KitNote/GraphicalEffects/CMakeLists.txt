# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2025 MuseScore Limited and others
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

kfw_create_qml_module(kfw_graphicaleffects_qml ALIAS kfw::graphicaleffects_qml FOR kfw_uicomponents)

qt_add_qml_module(kfw_graphicaleffects_qml
    URI KitNote.GraphicalEffects
    VERSION 1.0
    QML_FILES
        EffectDropShadow.qml
        EffectRectangularGlow.qml
        EffectFastBlur.qml
        EffectColorOverlay.qml
        RoundedCornersEffect.qml
)

fixup_qml_module_dependencies(kfw_graphicaleffects_qml)

qt_add_shaders(kfw_graphicaleffects_qml kfw_graphicaleffects_qml_shaders
    PREFIX
        "/qt/qml/KitNote/GraphicalEffects"
    FILES
        "shaders/roundcorners.frag"
)
