/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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

import "../../common"

Column {
    id: root

    required property RestSettingsProxyModel model

    property NavigationPanel navigationPanel: null
    property int navigationRowStart: 1

    objectName: "RestSettings"

    width: parent.width
    spacing: 12

    readonly property RestBeamSettingsModel beamModel: model ? model.modelByType(AbstractInspectorModel.TYPE_BEAM) : null
    readonly property RestSettingsModel restModel: model ? model.modelByType(AbstractInspectorModel.TYPE_REST_REST) : null

    function focusOnFirst() {
        restBeamSettings.focusOnFirst()
    }

    RestBeamSettings {
        id: restBeamSettings
        width: parent.width

        model: root.beamModel

        navigationPanel: root.navigationPanel
        navigationRowStart: root.navigationRowStart + 1000
    }

    PropertyToggle {
        id: alignWithOtherRests
        width: parent.width

        navigation.name: "Align with other rests"
        navigation.panel: root.navigationPanel
        navigation.row: restBeamSettings.navigationRowEnd + 1

        propertyItem: root.restModel ? root.restModel.alignWithOtherRests : null
        text: qsTrc("inspector", "Align with other rests in the same voice")
    }
}
