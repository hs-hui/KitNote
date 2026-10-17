/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2021-2024 MuseScore Limited and others
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

import KitNote.AppShell

StyledDialogView {
    id: root

    title: qsTrc("appshell/about", "About KitNote")

    contentHeight: 424
    contentWidth: 480

    AboutModel {
        id: aboutModel
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 30

        ColumnLayout {
            id: content

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 36
            Layout.leftMargin: 40
            Layout.rightMargin: 40

            spacing: 32

            Image {
                id: logo
                Layout.alignment: Qt.AlignHCenter

                source: "resources/mu_logo.svg"
                sourceSize: Qt.size(100, 100)

                MouseArea {
                    anchors.fill: parent

                    property int clickCount: 0

                    onClicked: {
                        clickCount++

                        if (clickCount % 3 == 0) {
                            aboutModel.toggleDevMode()
                        }
                    }
                }
            }

            Column {
                spacing: 8
                Layout.fillWidth: true

                StyledTextLabel {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTrc("appshell/about", "Version:") + " " + aboutModel.kitNoteVersion()
                    font: ui.theme.bodyBoldFont
                }

                Row {
                    spacing: 4
                    anchors.horizontalCenter: parent.horizontalCenter

                    StyledTextLabel {
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTrc("appshell/about", "Revision:") + " " + aboutModel.kitNoteRevision()
                    }

                    FlatButton {
                        anchors.verticalCenter: parent.verticalCenter
                        icon: IconCode.COPY

                        onClicked: {
                            aboutModel.copyRevisionToClipboard()
                        }
                    }
                }
            }

            StyledTextLabel {
                Layout.fillWidth: true
                //: KitNote: replaces the upstream links to musescore.org, its forum and its privacy policy
                text: qsTrc("appshell/about", "KitNote is a drum notation editor built from the source code of MuseScore 4.7.5. It is an independent project and is not affiliated with MuseScore Limited.")
                wrapMode: Text.WordWrap
                maximumLineCount: 4
            }

            StyledTextLabel {
                Layout.fillWidth: true
                text: qsTrc("appshell/about", "Copyright © 1999-2026 MuseScore Limited and others.\nPublished under the <a href=\"%1\">GNU General Public License version 3</a>.")
                      .arg("https://www.gnu.org/licenses/gpl-3.0.html")
                      .replace("\n", "<br>")

                wrapMode: Text.WordWrap
                maximumLineCount: 3
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignRight
            Layout.rightMargin: 16
            Layout.bottomMargin: 16

            spacing: 12

            FlatButton {
                text: qsTrc("global", "OK")

                onClicked: {
                    root.hide()
                }
            }
        }
    }
}
