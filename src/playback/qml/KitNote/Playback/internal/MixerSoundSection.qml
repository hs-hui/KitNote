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

pragma ComponentBehavior: Bound

import QtQuick

import KitNote.Ui
import KitNote.UiComponents

MixerPanelSection {
    id: root

    property bool resourcePickingActive: false

    headerTitle: qsTrc("playback", "Sound")

    Item {
        id: content

        required property MixerChannelItem channelItem

        height: resourceControl.height
        width: root.channelItemWidth

        property string accessibleName: (Boolean(root.needReadChannelName) ? channelItem.title + " " : "") + root.headerTitle

        visible: !channelItem.outputOnly

        AudioResourceControl {
            id: resourceControl

            anchors.horizontalCenter: parent.horizontalCenter
            height: 26
            visible: content.channelItem.type !== MixerChannelItem.Recording

            supportsByPassing: false
            resourceItemModel: content.channelItem.inputResourceItem ?? null

            navigationPanel: content.channelItem.panel
            navigationRowStart: root.navigationRowStart
            accessibleName: content.accessibleName

            onTitleClicked: {
                if (content.channelItem.inputResourceItem) {
                    content.channelItem.inputResourceItem.requestToLaunchNativeEditorView()
                }
            }

            onNavigateControlIndexChanged: function(index) {
                root.navigateControlIndexChanged(index)
            }

            onResourcePickingActiveChanged: {
                root.resourcePickingActive = resourceControl.resourcePickingActive
            }
        }

        // KitNote: the "Recording" channel plays a file - its name here; a click opens the recording's dialog
        FlatButton {
            id: recordingButton

            anchors.horizontalCenter: parent.horizontalCenter
            width: 96
            height: 26
            visible: content.channelItem.type === MixerChannelItem.Recording

            text: content.channelItem.recordingText
            textFont: ui.theme.bodyFont
            icon: content.channelItem.recordingWarning ? IconCode.WARNING : IconCode.NONE
            orientation: Qt.Horizontal

            navigation.panel: content.channelItem.panel
            navigation.row: root.navigationRowStart
            navigation.accessible.name: content.accessibleName + " " + content.channelItem.recordingText

            toolTipTitle: content.channelItem.recordingText
            toolTipDescription: content.channelItem.recordingToolTip

            onClicked: content.channelItem.editRecording()
        }
    }
}
