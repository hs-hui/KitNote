/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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

import KitNote.Ui
import KitNote.UiComponents

//! KitNote: at which edge of the score the rail with the main tools floats.
BaseSection {
    id: root

    title: qsTrc("preferences", "Toolbar position")

    property int position: 1

    signal positionChangeRequested(int newPosition)

    Column {
        spacing: 10

        Row {
            spacing: 12

            Repeater {
                model: [
                    { value: 0, text: qsTrc("preferences", "Top") },
                    { value: 3, text: qsTrc("preferences", "Bottom") },
                    { value: 1, text: qsTrc("preferences", "Left") },
                    { value: 2, text: qsTrc("preferences", "Right") }
                ]

                RoundedRadioButton {
                    required property var modelData
                    required property int index

                    checked: root.position === modelData.value
                    text: modelData.text

                    navigation.name: "ToolbarPosition" + index
                    navigation.panel: root.navigation
                    navigation.column: index

                    onToggled: {
                        root.positionChangeRequested(modelData.value)
                    }
                }
            }
        }

        StyledTextLabel {
            width: root.width
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.WordWrap
            opacity: 0.7

            text: qsTrc("preferences", "The edge of the score at which the rail with the main tools floats.")
        }
    }
}
