/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

import KitNote.Ui
import KitNote.UiComponents
import KitNote.NotationScene

Column {
    id: root

    property var padModel: null

    property int panelMode: -1
    property bool useNotationPreview: false
    property alias notationPreviewNumStaffLines: notationPreview.numStaffLines
    property color notationPreviewBackgroundColor: "transparent"

    property alias footerHeight: footerArea.height

    property bool padSwapActive: false

    property real cornerRadius: 0

    function openContextMenu(pos) {
        if (!root.padModel) {
            return
        }

        if (!pos) {
            pos = menuLoader.parent.mapFromItem(root, 0, root.height)
        }

        menuLoader.show(pos, root.padModel.contextMenuItems)
    }

    QtObject {
        id: prv

        //! NOTE KitNote: the pads are coloured by the kind of drum, so that the kit can be read at a glance
        readonly property var familyColors: ["#B9B6AC", "#D98B6A", "#8DB3A4", "#E3B95B", "#D9A441", "#8FA6C4"]
        readonly property color familyColor: familyColors[Boolean(root.padModel) ? root.padModel.drumFamily : 0]

        readonly property var currentColor: root.useNotationPreview ? root.notationPreviewBackgroundColor : prv.familyColor

        readonly property var currentOpacityNormal: root.useNotationPreview ? 0.9 : 0.85
        readonly property var currentOpacityHover: root.useNotationPreview ? 0.7 : 0.65
        readonly property var currentOpacityHit: root.useNotationPreview ? 1.0 : ui.theme.buttonOpacityHit
    }

    Item {
        id: mainContentArea

        width: parent.width
        height: parent.height - separator.height - footerArea.height

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            enabled: mainContentArea.enabled
            hoverEnabled: true

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressed: function(event) {
                ui.tooltip.hide(root)

                if (!Boolean(root.padModel)) {
                    return
                }

                if (event.button === Qt.RightButton) {
                    let pos = menuLoader.parent.mapFromItem(mouseArea, event.x, event.y)
                    root.openContextMenu(pos)
                    return
                }

                root.padModel.triggerPad(event.modifiers)
            }

            onContainsMouseChanged: {
                if (!Boolean(root.padModel)) {
                    ui.tooltip.hide(root)
                    return
                }

                if (mouseArea.containsMouse && root.useNotationPreview) {
                    ui.tooltip.show(root, root.padModel.padName)
                } else {
                    ui.tooltip.hide(root)
                }
            }
        }

        RoundedRectangle {
            id: padBackground

            color: Utils.colorWithAlpha(prv.currentColor, prv.currentOpacityNormal)

            anchors.fill: parent
            topLeftRadius: root.cornerRadius
            topRightRadius: root.cornerRadius
        }

        StyledTextLabel {
            id: padNameLabel

            visible: !root.useNotationPreview

            anchors.centerIn: parent
            width: parent.width - 12

            wrapMode: Text.WordWrap
            maximumLineCount: 4
            font: ui.theme.bodyBoldFont
            color: "#2B2B28"    // KitNote: the pads keep their colours in the dark theme, and dark text on them

            text: Boolean(root.padModel) ? root.padModel.padName : ""
        }

        PaintedEngravingItem {
            id: notationPreview

            visible: root.useNotationPreview

            anchors.fill: parent

            engravingItem: Boolean(root.padModel) ? root.padModel.notationPreviewItem : null
            spatium: 6.25 // Value approximated visually (needs to accomodate "extreme ledger line" situations)

            opacity: 0.9
        }

        states: [
            State {
                name: "MOUSE_HOVERED"
                when: mouseArea.containsMouse && !mouseArea.pressed && !root.padSwapActive
                PropertyChanges {
                    target: padBackground
                    color: Utils.colorWithAlpha(prv.currentColor, prv.currentOpacityHover)
                }
            },
            State {
                name: "MOUSE_HIT"
                when: mouseArea.pressed || root.padSwapActive
                PropertyChanges {
                    target: padBackground
                    color: Utils.colorWithAlpha(prv.currentColor, prv.currentOpacityHit)
                }
            }
        ]
    }

    Rectangle {
        id: separator

        width: parent.width
        height: 1

        color: root.useNotationPreview ? ui.theme.strokeColor : prv.familyColor
    }

    RoundedRectangle {
        id: footerArea

        width: parent.width
        bottomLeftRadius: root.cornerRadius
        bottomRightRadius: root.cornerRadius

        color: Utils.colorWithAlpha(ui.theme.buttonColor, ui.theme.buttonOpacityNormal)

        MouseArea {
            id: footerMouseArea

            anchors.fill: parent
            enabled: root.panelMode !== PanelMode.EDIT_LAYOUT
            hoverEnabled: true

            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressed: function(event) {
                let pos = menuLoader.parent.mapFromItem(footerMouseArea, event.x, event.y)
                root.openContextMenu(pos)
            }
        }

        StyledTextLabel {
            id: shortcutLabel

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.margins: 6

            font: ui.theme.bodyBoldFont
            color: ui.theme.fontPrimaryColor

            text: Boolean(root.padModel) ? root.padModel.keyboardShortcut : ""
        }

        StyledIconLabel {
            id: midiNoteIcon

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: midiNoteLabel.left

            opacity: 0.45   // KitNote: the MIDI note matters less than the key
            color: ui.theme.fontPrimaryColor

            iconCode: IconCode.SINGLE_NOTE
        }

        StyledTextLabel {
            id: midiNoteLabel

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.margins: 6

            opacity: 0.45
            font: ui.theme.bodyFont
            color: ui.theme.fontPrimaryColor

            text: Boolean(root.padModel) ? root.padModel.midiNote : ""
        }
    }

    ContextMenuLoader {
        id: menuLoader

        onHandleMenuItem: function(itemId) {
            root.padModel.handleMenuItem(itemId)
        }

        states: [
            State {
                name: "MOUSE_HOVERED"
                when: footerMouseArea.containsMouse && !footerMouseArea.pressed
                PropertyChanges {
                    target: footerArea
                    color: Utils.colorWithAlpha(ui.theme.buttonColor, ui.theme.buttonOpacityHover)
                }
            },
            State {
                name: "MOUSE_HIT"
                when: footerMouseArea.pressed
                PropertyChanges {
                    target: footerArea
                    color: Utils.colorWithAlpha(ui.theme.buttonColor, ui.theme.buttonOpacityHit)
                }
            }
        ]
    }
}
