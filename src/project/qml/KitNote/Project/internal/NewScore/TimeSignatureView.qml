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
import KitNote.Project

Item {
    id: root

    property var numerator: 0
    property var denominator: 0
    property var type: AdditionalInfoModel.Fraction

    height: contentColumn.height
    width: contentColumn.width

    Column {
        id: contentColumn
        visible: parent.type === AdditionalInfoModel.Fraction
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        spacing: 0

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: 0

            Repeater {
                model: root.numerator
                StyledIconLabel {
                    font.family: ui.theme.musicalFont.family
                    font.pixelSize: 60
                    height: 30
                    lineHeightMode: Text.FixedHeight
                    lineHeight: 30
                    iconCode: modelData
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: 0

            Repeater {
                model: root.denominator
                StyledIconLabel {
                    font.family: ui.theme.musicalFont.family
                    font.pixelSize: 60
                    height: 30
                    lineHeightMode: Text.FixedHeight
                    lineHeight: 30
                    iconCode: modelData
                }
            }
        }
    }

    StyledIconLabel {
        id: iconLabel
        visible: parent.type !== AdditionalInfoModel.Fraction
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        font.family: ui.theme.musicalFont.family
        font.pixelSize: 60
        iconCode: parent.type === AdditionalInfoModel.Common ? MusicalSymbolCodes.TIMESIG_COMMON
                                                             : MusicalSymbolCodes.TIMESIG_CUT
    }
}
