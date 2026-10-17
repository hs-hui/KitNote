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
import KitNote.UiComponents

FocusScope {
    id: root

    property string text: ""
    property string progressStatus: ""

    property real from: 0.0
    property real to: 1.0
    property real value: 0.0

    property string navigationName: ""
    property NavigationPanel navigationPanel: null
    property int navigationColumn: 0

    signal clicked()

    width: loader.width
    height: loader.height

    Loader {
        id: loader

        sourceComponent: root.value === root.from || root.value === root.to ? button : progressBar

        onLoaded: {
            if (sourceComponent == button) {
                width = item.width
                height = item.height
            }

            item.navigation.name = root.navigationName
            item.navigation.column = root.navigationColumn
            item.navigation.panel = root.navigationPanel
        }
    }

    Component {
        id: button

        FlatButton {
            text: root.text

            onClicked: {
                root.clicked()
            }
        }
    }

    Component {
        id: progressBar

        ProgressBar {
            from: root.from
            to: root.to
            value: root.value
            progressStatus: root.progressStatus
        }
    }
}
