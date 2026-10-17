/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

import "../../../common"

Item {
    id: root

    property PropertyItem textScale: null
    property PropertyItem diagramScale: null

    property NavigationPanel navigationPanel: null
    property int navigationRowStart: 1
    property int navigationRowEnd: diagramScaleView.navigationRowEnd

    height: childrenRect.height
    width: parent.width

    function focusOnFirst() {
        textScaleView.focusOnFirst()
    }

    SpinBoxPropertyView {
        id: textScaleView
        anchors.left: parent.left
        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 4

        titleText: qsTrc("inspector", "Text scale")
        propertyItem: root.textScale

        measureUnitsSymbol: "%"
        step: 1
        decimals: 0
        maxValue: 300
        minValue: 1

        navigationPanel: root.navigationPanel
        navigationRowStart: root.navigationRowStart + 1
    }

    SpinBoxPropertyView {
        id: diagramScaleView
        anchors.left: parent.horizontalCenter
        anchors.leftMargin: 4
        anchors.right: parent.right

        titleText: qsTrc("inspector", "Diagram scale")
        propertyItem: root.diagramScale

        measureUnitsSymbol: "%"
        step: 1
        decimals: 0
        maxValue: 300
        minValue: 1

        navigationPanel: root.navigationPanel
        navigationRowStart: textScaleView.navigationRowEnd + 1
    }
}
