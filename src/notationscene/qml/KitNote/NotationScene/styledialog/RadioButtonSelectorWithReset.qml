/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2021 MuseScore Limited and others
 * Modified for KitNote, 2026
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

pragma ComponentBehavior: Bound

import QtQuick

import KitNote.Ui
import KitNote.UiComponents
import KitNote.NotationScene

StyleControlRowWithReset {
    id: root

    property alias model: radioButtonGroup.model

    RadioButtonGroup {
        id: radioButtonGroup
        anchors.fill: parent

        delegate: FlatRadioButton {
            required property var modelData

            height: 30
            checked: modelData.value === root.styleItem.value
            text: modelData.text || ""
            iconCode: modelData.iconCode || IconCode.NONE
            navigation.accessible.name: modelData.title || modelData.text || ""

            onToggled: {
                root.styleItem.value = modelData.value
            }
        }
    }
}
