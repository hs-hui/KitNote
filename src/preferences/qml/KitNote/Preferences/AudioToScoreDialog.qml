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
import KitNote.Preferences

//! KitNote: File > Drum score from a recording.
StyledDialogView {
    id: root

    title: qsTrc("preferences/ai", "Drum score from a recording")

    contentWidth: 520
    contentHeight: content.implicitHeight + 48

    // (Escape does not end a transcription by accident)
    closeOnEscape: !audioModel.transcribing

    //! a recording dropped on the window
    property string audio: ""

    AudioToScoreModel {
        id: audioModel

        Component.onCompleted: {
            if (root.audio !== "") {
                setAudio(root.audio)
            }
        }

        onDone: {
            root.hide()
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
            text: qsTrc("preferences/ai", "KitNote separates the drums from the other instruments, finds their strokes and the beat, and writes them as a score next to the recording. It takes about as long as the song, or longer. The result is a starting point to correct, not a finished transcription.")
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
            border.color: dropZone.hovered || audioModel.audioPath ? ui.theme.accentColor : ui.theme.strokeColor
            opacity: audioModel.busy ? 0.6 : 1.0

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
                    color: audioModel.audioPath ? ui.theme.accentColor : ui.theme.fontPrimaryColor
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
                        text: audioModel.audioPath ? audioModel.audioPath.replace(/^.*[\\/]/, "")
                                                   : qsTrc("preferences/ai", "Drop a recording here, or click to choose one")
                    }

                    StyledTextLabel {
                        width: parent.width
                        horizontalAlignment: Text.AlignLeft
                        elide: Text.ElideMiddle
                        opacity: 0.6
                        // (as the system writes paths: with backslashes on Windows)
                        text: audioModel.audioPath ? (Qt.platform.os === "windows" ? audioModel.audioPath.replace(/\//g, "\\")
                                                                                    : audioModel.audioPath)
                                                   : qsTrc("preferences/ai", "wav, mp3, flac, ogg (m4a with ffmpeg)")
                    }
                }
            }

            MouseArea {
                id: clickArea

                anchors.fill: parent
                enabled: !audioModel.busy
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: audioModel.chooseAudio()
            }

            DropArea {
                id: dropArea

                anchors.fill: parent
                enabled: !audioModel.busy
                onDropped: function(drop) {
                    if (drop.hasUrls) {
                        audioModel.setAudio(drop.urls[0].toString())
                        drop.accept()
                    }
                }
            }
        }

        CheckBox {
            id: drumsOnlyBox

            enabled: !audioModel.busy
            text: qsTrc("preferences/ai", "The recording is of drums alone (no separation)")
            onClicked: checked = !checked
        }

        CheckBox {
            id: ghostsBox

            enabled: !audioModel.busy
            text: qsTrc("preferences/ai", "Also write ghost notes (weak snare strokes)")
            onClicked: checked = !checked
        }

        CheckBox {
            id: accurateBox

            enabled: !audioModel.busy && !drumsOnlyBox.checked
            text: qsTrc("preferences/ai", "Separate more accurately (about four times slower; downloads about 320 MB more from Meta the first time)")
            onClicked: checked = !checked
        }

        CheckBox {
            id: sectionsBox

            enabled: !audioModel.busy
            checked: true
            text: qsTrc("preferences/ai", "Mark the sections where the groove changes (A, B ...)")
            onClicked: checked = !checked
        }

        CheckBox {
            id: repeatsBox

            enabled: !audioModel.busy
            checked: true
            text: qsTrc("preferences/ai", "Write measures that repeat the one before as % signs")
            onClicked: checked = !checked
        }

        CheckBox {
            id: stemsBox

            enabled: !audioModel.busy && !drumsOnlyBox.checked
            text: qsTrc("preferences/ai", "Also keep the separated instruments (drums, bass, vocals, other) as wav files")
            onClicked: checked = !checked
        }

        Row {
            spacing: 8

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTrc("preferences/ai", "Time signature:")
            }

            StyledDropdown {
                id: meterBox

                width: 130
                enabled: !audioModel.busy
                model: [{ text: qsTrc("preferences/ai", "Automatic"), value: "auto" }, { text: "4/4", value: "4/4" }, { text: "3/4", value: "3/4" }, { text: "2/4", value: "2/4" }, { text: "6/8", value: "6/8" }, { text: "12/8", value: "12/8" }]
                currentIndex: 0

                onActivated: function(index, value) {
                    currentIndex = index
                }
            }
        }

        Row {
            spacing: 8

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter
                text: qsTrc("preferences/ai", "Tempo, if you know it:")
            }

            // the note the tempo counts, as a tempo marking writes it (TempoView): a quarter note, or in
            // 6/8 and 12/8 a dotted quarter
            Row {
                id: tempoNote

                readonly property bool compound: meterBox.currentValue === "6/8" || meterBox.currentValue === "12/8"

                // (as high as the field: the musical font is far taller than its note)
                height: tempoField.height
                anchors.verticalCenter: parent.verticalCenter
                visible: meterBox.currentValue !== "auto"

                StyledTextLabel {
                    height: parent.height
                    topPadding: 10
                    lineHeightMode: Text.FixedHeight
                    lineHeight: 10
                    font.family: ui.theme.musicalFont.family
                    font.pixelSize: ui.theme.musicalFont.pixelSize
                    font.letterSpacing: 1
                    text: String.fromCharCode(MusicalSymbolCodes.CROTCHET) + (tempoNote.compound ? String.fromCharCode(MusicalSymbolCodes.DOT) : "")
                }

                StyledTextLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    text: " ="
                }
            }

            IncrementalPropertyControl {
                id: tempoField

                anchors.verticalCenter: parent.verticalCenter

                navigation.accessible.name: qsTrc("preferences/ai", "Tempo, if you know it:") + " "
                                            + (!tempoNote.visible ? "" : tempoNote.compound ? qsTrc("preferences/ai", "dotted quarter notes per minute")
                                                                                           : qsTrc("preferences/ai", "quarter notes per minute"))

                width: 110
                enabled: !audioModel.busy
                currentValue: 0
                minValue: 0
                maxValue: 300
                step: 1
                decimals: 0
                measureUnitsSymbol: " BPM"

                onValueEdited: function(newValue) {
                    currentValue = newValue
                }
            }

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter
                opacity: 0.6
                text: qsTrc("preferences/ai", "(0: found from the music)")
            }
        }

        StyledTextLabel {
            width: parent.width
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
            visible: !audioModel.engineInstalled && !audioModel.busy
            text: qsTrc("preferences/ai", "The audio engine is not installed yet (about 1 GB, installed only if you agree).")
        }

        ProgressBar {
            width: parent.width
            height: 6
            visible: audioModel.busy
            from: 0
            to: 100
            value: audioModel.progress
        }

        StyledTextLabel {
            width: parent.width
            horizontalAlignment: Text.AlignLeft
            wrapMode: Text.Wrap
            visible: text !== ""
            text: audioModel.busy ? audioModel.progress + "% - " + audioModel.status : audioModel.status
        }

        Row {
            anchors.right: parent.right
            spacing: 8

            // while the recording is transcribed: Stop (closing the dialog would end it without a word)
            FlatButton {
                text: audioModel.transcribing ? qsTrc("preferences/ai", "Stop") : qsTrc("global", "Close")
                onClicked: {
                    if (audioModel.transcribing) {
                        audioModel.stop()
                    } else {
                        root.hide()
                    }
                }
            }

            FlatButton {
                visible: !audioModel.engineInstalled
                enabled: !audioModel.busy
                accentButton: true
                text: qsTrc("preferences/ai", "Install the audio engine…")
                onClicked: audioModel.installEngine()
            }

            FlatButton {
                visible: audioModel.engineInstalled
                enabled: !audioModel.busy && audioModel.audioPath !== ""
                accentButton: true
                text: qsTrc("preferences/ai", "Make the score")
                onClicked: audioModel.start(!drumsOnlyBox.checked, tempoField.currentValue, stemsBox.checked, meterBox.currentValue, accurateBox.checked, repeatsBox.checked, sectionsBox.checked, ghostsBox.checked)
            }
        }
    }
}
