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
import KitNote.Inspector
import KitNote.UiComponents

import "../../common"

Column {
    id: root

    required property JumpSettingsModel model

    property NavigationPanel navigationPanel: null
    property int navigationRowStart: 1

    objectName: "JumpSettings"

    spacing: 12

    function focusOnFirst() {
        jumpTo.focusOnFirst()
    }

    TextSection {
        id: jumpTo
        titleText: qsTrc("inspector", "Jump to")
        propertyItem: root.model ? root.model.jumpTo : null

        navigationPanel: root.navigationPanel
        navigationRowStart: root.navigationRowStart
    }

    TextSection {
        id: playUntil
        titleText: qsTrc("inspector", "Play until")
        propertyItem: root.model ? root.model.playUntil : null

        navigationPanel: root.navigationPanel
        navigationRowStart: jumpTo.navigationRowEnd + 1
    }

    TextSection {
        id: continueAt
        titleText: qsTrc("inspector", "Continue at")
        propertyItem: root.model ? root.model.continueAt : null

        navigationPanel: root.navigationPanel
        navigationRowStart: playUntil.navigationRowEnd + 1
    }

    PropertyCheckBox {
        text: qsTrc("inspector", "Play repeats")
        propertyItem: root.model ? root.model.hasToPlayRepeats : null

        navigation.name: "PlayRepeats"
        navigation.panel: root.navigationPanel
        navigation.row: continueAt.navigationRowEnd + 1
    }
}
