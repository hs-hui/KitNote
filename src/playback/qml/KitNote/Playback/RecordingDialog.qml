/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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
import KitNote.Playback

//! KitNote: File > Play along with a recording - the recording of the song, played in the Mixer's
//! "Recording" channel with the score, and where measure 1 begins in it.
StyledDialogView {
    id: root

    title: qsTrc("playback", "Play along with a recording")

    contentWidth: 520
    contentHeight: content.implicitHeight + 48

    RecordingModel {
        id: recordingModel

        Component.onCompleted: {
            load()
        }
    }

    Column {
        id: content

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 24

        spacing: 14

        StyledTextLabel {
            width: parent.width
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
            text: qsTrc("playback", "To check a score against the song, KitNote plays a recording along with it, in the Mixer's “Recording” channel: its fader, mute and solo set the balance (solo: the recording alone; mute: the drums alone). A score made from a recording plays it already.")
        }

        // the recording: a click chooses one, or one is dropped here
        Rectangle {
            id: dropZone

            readonly property bool hovered: dropArea.containsDrag || clickArea.containsMouse

            width: parent.width
            height: 76
            radius: 8
            color: dropZone.hovered ? Utils.colorWithAlpha(ui.theme.accentColor, 0.16) : ui.theme.textFieldColor
            border.width: 1
            border.color: dropZone.hovered || recordingModel.attached ? ui.theme.accentColor : ui.theme.strokeColor

            Row {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 16
                spacing: 14

                StyledIconLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    iconCode: IconCode.MICROPHONE
                    font.pixelSize: 26
                    color: recordingModel.attached ? ui.theme.accentColor : ui.theme.fontPrimaryColor
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width - 54
                    spacing: 4

                    StyledTextLabel {
                        width: parent.width
                        horizontalAlignment: Text.AlignLeft
                        elide: Text.ElideMiddle
                        font: ui.theme.bodyBoldFont
                        text: recordingModel.attached ? recordingModel.fileName
                                                      : qsTrc("playback", "Drop a recording here, or click to choose one")
                    }

                    StyledTextLabel {
                        width: parent.width
                        horizontalAlignment: Text.AlignLeft
                        elide: Text.ElideMiddle
                        opacity: 0.6
                        text: recordingModel.attached ? recordingModel.filePath : "wav, flac, mp3, ogg"
                    }
                }
            }

            MouseArea {
                id: clickArea

                anchors.fill: parent
                enabled: !recordingModel.finding
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: recordingModel.chooseFile()
            }

            DropArea {
                id: dropArea

                anchors.fill: parent
                enabled: !recordingModel.finding
                onDropped: function(drop) {
                    if (drop.hasUrls) {
                        recordingModel.setFile(drop.urls[0].toString())
                        drop.accept()
                    }
                }
            }
        }

        Row {
            spacing: 8
            enabled: recordingModel.attached && !recordingModel.finding

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTrc("playback", "Measure 1 begins at")
            }

            IncrementalPropertyControl {
                id: offsetField

                width: 120
                currentValue: recordingModel.offset
                minValue: -60
                maxValue: 3600
                step: 0.01
                decimals: 3
                measureUnitsSymbol: qsTrc("playback", "s")

                onValueEdited: function(newValue) {
                    recordingModel.setOffset(newValue)
                }
            }

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTrc("playback", "in the recording")
            }
        }

        // found from the notes, then checked by ear: play from here, move the offset, play again
        Row {
            spacing: 8
            enabled: recordingModel.attached && !recordingModel.finding

            FlatButton {
                text: qsTrc("playback", "Find automatically")
                toolTipTitle: qsTrc("playback", "Find where measure 1 begins")
                toolTipDescription: qsTrc("playback", "Matches the notes of the score with the strokes heard in the recording")
                onClicked: recordingModel.findOffset()
            }

            FlatButton {
                icon: recordingModel.playing ? IconCode.PAUSE : IconCode.PLAY
                text: recordingModel.playing ? qsTrc("action", "Pause") : qsTrc("action", "Play")
                orientation: Qt.Horizontal
                onClicked: recordingModel.togglePlay()
            }
        }

        StyledTextLabel {
            width: parent.width
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
            visible: text !== ""
            font: recordingModel.warning ? ui.theme.bodyBoldFont : ui.theme.bodyFont
            text: recordingModel.status
        }

        Item {
            width: parent.width
            height: closeButton.height

            FlatButton {
                anchors.left: parent.left
                visible: recordingModel.attached
                enabled: !recordingModel.finding
                text: qsTrc("playback", "Remove the recording")
                onClicked: recordingModel.remove()
            }

            FlatButton {
                id: closeButton

                anchors.right: parent.right
                accentButton: true
                text: qsTrc("global", "Close")
                onClicked: root.hide()
            }
        }
    }
}
