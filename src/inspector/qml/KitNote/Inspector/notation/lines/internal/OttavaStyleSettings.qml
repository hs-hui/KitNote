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
import QtQuick

import KitNote.UiComponents
import KitNote.Ui
import KitNote.Inspector

import "../../../common"

FocusableItem {
    id: root

    required property OttavaSettingsModel model

    property NavigationPanel navigationPanel: null
    property int navigationRowStart: 1

    implicitHeight: contentColumn.height
    width: parent.width

    Column {
        id: contentColumn

        width: parent.width

        spacing: 12

        DropdownPropertyView {
            id: typeSection
            titleText: qsTrc("inspector", "Type")
            propertyItem: root.model ? root.model.ottavaType : null
            model: root.model ? root.model.possibleOttavaTypes() : null

            navigationPanel: root.navigationPanel
            navigationRowStart: root.navigationRowStart
        }

        PropertyCheckBox {
            id: showNumbersOnlyCheckBox
            text: qsTrc("inspector", "Show numbers only")
            propertyItem: root.model ? root.model.showNumbersOnly : null

            navigation.name: "ShowNumbersOnly"
            navigation.panel: root.navigationPanel
            navigation.row: typeSection.navigationRowEnd + 1
        }

        SeparatorLine { anchors.margins: -12 }

        LineWithHooksCommonStyleSettings {
            model: root.model

            navigationPanel: root.navigationPanel
            navigationRowStart: showNumbersOnlyCheckBox.navigation.row + 1
        }
    }
}
