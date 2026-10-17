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

InspectorPropertyView {
    id: root

    property alias spinBox: spinBoxItem

    property alias minValue: spinBoxItem.minValue
    property alias maxValue: spinBoxItem.maxValue
    property alias step: spinBoxItem.step
    property alias decimals: spinBoxItem.decimals
    property alias measureUnitsSymbol: spinBoxItem.measureUnitsSymbol
    property alias icon: spinBoxItem.icon
    property alias iconMode: spinBoxItem.iconMode
    property alias wrap: spinBoxItem.wrap

    navigationName: "SpinBoxPropertyView"
    navigationRowEnd: spinBoxItem.navigation.row

    function focusOnFirst() {
        spinBoxItem.navigation.requestActive()
    }

    IncrementalPropertyControl {
        id: spinBoxItem

        navigation.name: root.navigationName + " Spinbox"
        navigation.panel: root.navigationPanel
        navigation.row: root.navigationRowStart + 1
        navigation.accessible.name: root.accessibleName

        isIndeterminate: root.propertyItem ? root.propertyItem.isUndefined : true
        currentValue: Boolean(root.propertyItem) && !Boolean(root.propertyItem.isUndefined) ? root.propertyItem.value : 0.0

        onValueEditingFinished: function(newValue) {
            if (root.propertyItem) {
                root.propertyItem.value = newValue
            }
        }
    }
}
