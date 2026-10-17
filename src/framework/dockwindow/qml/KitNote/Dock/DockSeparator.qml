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

import KitNote.Ui

Rectangle {
    id: root

    anchors.fill: parent

    color: ui.theme.strokeColor

    readonly property QtObject separatorCpp: parent

    visible: separatorCpp && separatorCpp.isSeparatorVisible

    MouseArea {
        anchors.fill: parent
        anchors.margins: -5 //! NOTE: extra space for user convenience

        cursorShape: (root.separatorCpp && separatorCpp.showResizeCursor)
                     ? (root.separatorCpp.isVertical ? Qt.SizeVerCursor : Qt.SizeHorCursor)
                     : Qt.ArrowCursor

        onPressed: {
            root.separatorCpp.onMousePressed()
        }

        onReleased: {
            root.separatorCpp.onMouseReleased()
        }

        onPositionChanged: function(mouse) {
            root.separatorCpp.onMouseMoved(Qt.point(mouse.x, mouse.y))
        }

        onDoubleClicked: {
            root.separatorCpp.onMouseDoubleClicked()
        }
    }
}
