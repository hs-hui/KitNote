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
import KitNote.Inspector

import "../../common"

Column {
    id: root

    required property LyricsSettingsModel model

    property NavigationPanel navigationPanel: null
    property int navigationRowStart: 1

    objectName: "LyricsSettings"

    spacing: 12

    function focusOnFirst() {
        setVerse.focusOnFirst()
    }

    SpinBoxPropertyView {
        id: setVerse
        titleText: qsTrc("inspector", "Set to verse")
        propertyItem: root.model ? root.model.verse : null

        decimals: 0
        step: 1
        minValue: 1

        navigationName: "Verse"
        navigationPanel: root.navigationPanel
        navigationRowStart: root.navigationRowStart
    }

    PropertyCheckBox {
        navigation.name: "Avoid barlines"
        navigation.panel: root.navigationPanel
        navigation.row: setVerse.navigationRowEnd + 1

        text: qsTrc("inspector", "Avoid barlines")

        propertyItem: root.model ? root.model.avoidBarlines : null
    }
}
