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
import QtQuick.Layouts

import KitNote.Ui
import KitNote.UiComponents

Item {
    id: root

    height: parent ? parent.height : implicitHeight
    width: parent ? parent.width : implicitWidth

    property var item

    property NavigationPanel navigationPanel: null
    property int navigationRow: 0

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6

        spacing: 16

        VisibilityBox {
            id: visibilityButton

            Layout.alignment: Qt.AlignLeft

            navigation.panel: root.navigationPanel
            navigation.row: root.navigationRow
            navigation.column: 1
            accessibleText: titleLabel.text

            isVisible: root.item.checked

            onVisibleToggled: {
                root.item.checked = !root.item.checked
            }
        }

        StyledIconLabel {
            Layout.alignment: Qt.AlignLeft

            Layout.preferredWidth: 36
            Layout.preferredHeight: 36

            iconCode: Boolean(root.item) ? root.item.icon : IconCode.NONE
        }

        StyledTextLabel {
            id: titleLabel

            Layout.fillWidth: true

            horizontalAlignment: Qt.AlignLeft
            text: Boolean(root.item) ? root.item.title : ""
        }
    }
}
