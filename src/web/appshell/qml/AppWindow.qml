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
import QtQuick 2.15
import QtQuick.Controls 2.15

import KitNote.Ui 1.0
import KitNote.Shortcuts 1.0
import KitNote.AppShell 1.0

import KitNote.Tours 1.0

ApplicationWindow {
    id: root

    default property alias windowContent: windowContentItem.data

    objectName: "ApplicationWindow"

    width: 1150
    height: 800

    minimumWidth: 1050
    minimumHeight: 500

    visible: false

    color: ui.theme.backgroundPrimaryColor

    Component.onCompleted: {
        ui.rootItem = root.contentItem
    }


    MainWindowBridge {
        id: bridge

        window: root
    }

    GraphicsTestObject {}

    ToolTipProvider { }

    Shortcuts { }

    Item {
        id: windowContentItem
        anchors.fill: parent
    }

    function showMinimizedWithSavePreviousState() {
        bridge.showMinimizedWithSavePreviousState()
    }
}
