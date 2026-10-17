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
import QtQuick.Layouts

import KitNote.Ui
import KitNote.UiComponents
import KitNote.GraphicalEffects
import KitNote.Project

ListItemBlank {
    id: root

    required property var score
    property alias columns: columnsRepeater.model

    property alias thumbnailComponent: thumbnailLoader.sourceComponent

    property real itemInset: 12
    property real columnSpacing: 44
    property alias showBottomBorder: bottomBorder.visible

    implicitHeight: 64

    navigation.accessible.name: root.score.name ?? ""
    navigation.onActiveChanged: {
        if (navigation.active) {
            root.scrollIntoView()
        }
    }

    focusBorder.anchors.bottomMargin: bottomBorder.visible ? bottomBorder.height : 0

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: root.itemInset
        anchors.rightMargin: root.itemInset

        spacing: root.columnSpacing

        RowLayout {
            spacing: 24

            Loader {
                id: thumbnailLoader

                Layout.preferredWidth: 30
                Layout.preferredHeight: 40

                sourceComponent: ScoreThumbnail {
                    path: root.score.path ?? ""
                    suffix: root.score.suffix ?? ""
                    thumbnailUrl: root.score.thumbnailUrl ?? ""
                }

                layer.enabled: ui.isEffectsAllowed
                layer.effect: RoundedCornersEffect {
                    radius: 2
                }
            }

            StyledTextLabel {
                Layout.fillWidth: true

                text: root.score.name ?? ""
                font: ui.theme.largeBodyFont
                horizontalAlignment: Text.AlignLeft
            }

        }

        Repeater {
            id: columnsRepeater

            delegate: Loader {
                Layout.preferredWidth: modelData.width(parent.width)

                // These properties are here to give the delegate access to them
                readonly property ScoreListItem listItem: root
                readonly property var score: root.score
                readonly property NavigationPanel navigationPanel: root.navigation.panel
                readonly property int navigationRow: root.navigation.row
                readonly property int navigationColumnStart: 100 * (model.index + 1)

                sourceComponent: modelData.delegate
            }
        }
    }

    SeparatorLine {
        id: bottomBorder
        anchors.bottom: parent.bottom
    }
}
