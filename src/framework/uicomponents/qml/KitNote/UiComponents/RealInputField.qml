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

TextInputField {

    id: root

    property real currentValue: 0.0

    property real max: 10000.0
    property real min: -10000.0
    property int decimals: 2

    currentText: ui.df.formatReal(root.currentValue, root.decimals)

    validator: DoubleInputValidator {
        top: root.max
        bottom: root.min
        decimal: root.decimals
    }

    onTextEdited: function(newTextValue) {
        root.currentValue = parseFloat(newTextValue)
    }
}
