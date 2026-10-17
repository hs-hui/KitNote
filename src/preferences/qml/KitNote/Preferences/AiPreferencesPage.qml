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
pragma ComponentBehavior: Bound

import QtQuick

import KitNote.Ui
import KitNote.UiComponents
import KitNote.Preferences

import "internal"

//! KitNote: Preferences > AI - KitNote in AI tools, and the audio engine.
PreferencesPage {
    id: root

    AiPreferencesModel {
        id: aiModel
    }

    Column {
        width: parent.width
        spacing: root.sectionsSpacing

        BaseSection {
            width: parent.width
            title: qsTrc("preferences/ai", "KitNote in AI tools")

            navigation.section: root.navigationSection
            navigation.order: root.navigationOrderStart + 1

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.Wrap
                text: qsTrc("preferences/ai", "AI tools can write, change, read and export drum scores through KitNote: its MCP server and its skill (how to write drums as a tab) are put into the settings of the tool. Then ask it, for example, \"a rock groove in 4/4 at 120 with a fill every four measures, as a PDF\". Restart the tool afterwards.")
            }

            Column {
                width: parent.width
                spacing: 6

                Repeater {
                    model: aiModel.clients

                    Rectangle {
                        id: clientRow

                        required property var modelData

                        width: parent.width
                        height: 52
                        radius: 6
                        color: Qt.alpha(ui.theme.fontPrimaryColor, 0.05)

                        Column {
                            anchors.left: parent.left
                            anchors.leftMargin: 14
                            anchors.right: clientButton.left
                            anchors.rightMargin: 12
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 2

                            StyledTextLabel {
                                width: parent.width
                                horizontalAlignment: Text.AlignLeft
                                font: ui.theme.bodyBoldFont
                                text: clientRow.modelData.name + "  ·  "
                                      + (clientRow.modelData.installed ? qsTrc("preferences/ai", "set up")
                                         : clientRow.modelData.found ? qsTrc("preferences/ai", "found on this computer")
                                                                     : qsTrc("preferences/ai", "not found"))
                            }

                            StyledTextLabel {
                                width: parent.width
                                horizontalAlignment: Text.AlignLeft
                                elide: Text.ElideMiddle
                                opacity: 0.6
                                text: clientRow.modelData.where + (clientRow.modelData.skill ? qsTrc("preferences/ai", "  + skill") : "")
                            }
                        }

                        FlatButton {
                            id: clientButton

                            anchors.right: parent.right
                            anchors.rightMargin: 10
                            anchors.verticalCenter: parent.verticalCenter
                            width: 110

                            accentButton: !clientRow.modelData.installed && clientRow.modelData.found
                            text: clientRow.modelData.installed ? qsTrc("preferences/ai", "Remove") : qsTrc("preferences/ai", "Set up")

                            onClicked: {
                                if (clientRow.modelData.installed) {
                                    aiModel.remove(clientRow.modelData.id)
                                } else {
                                    aiModel.install(clientRow.modelData.id)
                                }
                            }
                        }
                    }
                }
            }

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.WrapAnywhere
                opacity: 0.7
                text: qsTrc("preferences/ai", "For other tools: the MCP server is the program %1 (stdio, no arguments).").arg(aiModel.serverPath)
            }

            Row {
                spacing: 8

                FlatButton {
                    text: qsTrc("preferences/ai", "Copy the server path")
                    onClicked: aiModel.copyServerCommand()
                }

                FlatButton {
                    text: qsTrc("preferences/ai", "Open the skill folder")
                    onClicked: aiModel.openSkillFolder()
                }
            }
        }

        SeparatorLine {}

        BaseSection {
            width: parent.width
            title: qsTrc("preferences/ai", "How to use it")

            navigation.section: root.navigationSection
            navigation.order: root.navigationOrderStart + 2

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.Wrap
                text: qsTrc("preferences/ai", "After installing, start a new session of the AI tool and ask in your own words, for example:")
            }

            Column {
                width: parent.width
                spacing: 4

                Repeater {
                    model: [
                        qsTrc("preferences/ai", "“With KitNote, write an 8-measure funk groove at 96 BPM with ghost notes, and give me the PDF.”"),
                        qsTrc("preferences/ai", "“Open Documents/KitNote/song.kitnote and put a fill at the end of every chorus.”"),
                        qsTrc("preferences/ai", "“Make a drum score from C:/Music/song.mp3.”")
                    ]

                    StyledTextLabel {
                        required property string modelData

                        width: parent.width
                        horizontalAlignment: Text.AlignLeft
                        wrapMode: Text.Wrap
                        opacity: 0.8
                        text: "•  " + modelData
                    }
                }
            }
        }

        SeparatorLine {}

        BaseSection {
            width: parent.width
            title: qsTrc("preferences/ai", "Audio engine: drum scores from recordings")

            navigation.section: root.navigationSection
            navigation.order: root.navigationOrderStart + 3

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.Wrap
                text: qsTrc("preferences/ai", "File > Drum score from a recording - and an AI tool given a recording - separates the drums from the other instruments of a song (Demucs), finds their strokes and writes them as a score. It needs an engine of about 1 GB that is installed only if you agree (here, or in the AI tool's chat): Python, PyTorch and the separation model, in %1.").arg(aiModel.engineFolder)
            }

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                font: ui.theme.bodyBoldFont
                wrapMode: Text.Wrap
                text: aiModel.engineBusy ? qsTrc("preferences/ai", "Installing: %1% - %2").arg(aiModel.engineProgress).arg(aiModel.engineStatus)
                                         : aiModel.engineUpgradable ? qsTrc("preferences/ai", "Installed: engine 1. Engine 2 - with a model that finds the beats and the measures, and KitNote's drum model - transcribes far better: update it (about 100 MB more).")
                                         : aiModel.engineInstalled ? qsTrc("preferences/ai", "Installed.")
                                                                   : qsTrc("preferences/ai", "Not installed.")
            }

            StyledTextLabel {
                width: parent.width
                horizontalAlignment: Text.AlignLeft
                wrapMode: Text.Wrap
                visible: aiModel.engineInstalled && !aiModel.engineUpgradable && !aiModel.engineBusy
                text: aiModel.drumModel
            }

            ProgressBar {
                width: parent.width
                height: 6
                visible: aiModel.engineBusy
                from: 0
                to: 100
                value: aiModel.engineProgress
            }

            Row {
                spacing: 8

                FlatButton {
                    visible: aiModel.engineUpgradable
                    enabled: !aiModel.engineBusy
                    accentButton: true
                    text: qsTrc("preferences/ai", "Update the audio engine…")
                    onClicked: aiModel.installEngine()
                }

                FlatButton {
                    enabled: !aiModel.engineBusy
                    accentButton: !aiModel.engineInstalled
                    text: aiModel.engineInstalled ? qsTrc("preferences/ai", "Remove the audio engine") : qsTrc("preferences/ai", "Install the audio engine…")

                    onClicked: {
                        if (aiModel.engineInstalled) {
                            aiModel.removeEngine()
                        } else {
                            aiModel.installEngine()
                        }
                    }
                }
            }
        }
    }
}
