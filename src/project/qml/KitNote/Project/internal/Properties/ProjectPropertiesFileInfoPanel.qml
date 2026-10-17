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

import KitNote.Project
import KitNote.Ui
import KitNote.UiComponents

ColumnLayout {
    id: root

    required property ProjectPropertiesModel propertiesModel

    property int propertyNameWidth: 160
    property int propertyRowHorizontalSpacing: 8
    property int propertyRowRightMargin: 0

    property NavigationPanel navigationPanel: null
    property int navigationColumnEnd: apiLevelProperty.navigationColumnEnd

    spacing: 8

    RowLayout {
        spacing: root.propertyRowHorizontalSpacing

        PropertyItem {
            id: filePathProperty

            propertyNameWidth: root.propertyNameWidth
            index: 0
            propertyName: qsTrc("project/properties", "File path:")
            propertyValue: root.propertiesModel.filePath
            isFileInfoPanelProperty: true
            valueFillWidth: true

            navigationPanel: root.navigationPanel
        }

        FlatButton {
            id: openFileLocation

            Layout.preferredWidth: 30
            Layout.preferredHeight: 30

            icon: IconCode.OPEN_FILE

            navigation.name: "OpenFileLocation"
            navigation.panel: root.navigationPanel
            navigation.column: filePathProperty.navigationColumnEnd + 1
            accessible.name: "Open file location"

            onClicked: root.propertiesModel.openFileLocation()
        }
    }

    RowLayout {
        spacing: root.propertyRowHorizontalSpacing

        PropertyItem {
            index: 2
            // KitNote: files record the version of the MuseScore engine, not KitNote's own
            propertyName: qsTrc("project/properties", "MuseScore engine version:")
            propertyValue: root.propertiesModel.version
            isFileInfoPanelProperty: true

            navigationPanel: root.navigationPanel
        }

        PropertyItem {
            index: 3
            propertyName: qsTrc("project/properties", "Revision:")
            propertyValue: root.propertiesModel.revision
            isFileInfoPanelProperty: true

            Layout.leftMargin: 32
            Layout.rightMargin: 32

            navigationPanel: root.navigationPanel
        }

        PropertyItem {
            id: apiLevelProperty

            index: 4
            propertyName: qsTrc("project/properties", "API-Level:")
            propertyValue: root.propertiesModel.apiLevel
            isFileInfoPanelProperty: true

            navigationPanel: root.navigationPanel
        }
    }
}
