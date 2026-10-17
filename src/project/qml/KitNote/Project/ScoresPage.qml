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
import QtQuick.Layouts

import KitNote.Ui
import KitNote.UiComponents
import KitNote.Project

import "internal/ScoresPage"

FocusScope {
    id: root

    QtObject {
        id: prv

        readonly property int sideMargin: 46
    }

    NavigationSection {
        id: navSec
        name: "RecentScores"
        enabled: root.enabled && root.visible
        order: 3
        onActiveChanged: {
            if (navSec.active) {
                root.forceActiveFocus()
            }
        }
    }

    ScoresPageModel {
        id: scoresPageModel
    }

    Rectangle {
        id: background

        anchors.fill: parent

        color: ui.theme.backgroundSecondaryColor
    }

    RowLayout {
        id: topLayout

        anchors.top: parent.top
        anchors.topMargin: prv.sideMargin
        anchors.left: parent.left
        anchors.leftMargin: prv.sideMargin
        anchors.right: parent.right
        anchors.rightMargin: prv.sideMargin

        spacing: 12

        NavigationPanel {
            id: navSearchPanel
            name: "HomeScoresSearch"
            enabled: topLayout.enabled && topLayout.visible
            section: navSec
            order: 1
            accessible.name: qsTrc("project", "Scores")
        }

        StyledTextLabel {
            id: pageTitle
            Layout.fillWidth: true

            text: qsTrc("project", "What will you write today?")
            font: ui.theme.titleBoldFont
            horizontalAlignment: Text.AlignLeft
        }

        SearchField {
            id: searchField

            Layout.preferredWidth: 220

            navigation.name: "Scores Search"
            navigation.panel: navSearchPanel
            navigation.order: 1
            accessible.name: qsTrc("project", "Search recent scores")
        }
    }

    //! NOTE KitNote: "new score" as a row of cards - one per bundled template, a click opens a score made
    //! from it - then the wizard for everything else, and "open". They replace the buttons at the bottom.
    component StartCard: Rectangle {
        id: card

        property int icon: IconCode.NONE
        property string title: ""
        property string subtitle: ""
        property bool primary: false

        property alias navigation: cardNav

        signal clicked()

        // all the cards in one row as long as they fit: the row shares the width of the page
        width: root.startCardWidth
        height: 104
        radius: 12

        color: card.primary ? ui.theme.fontPrimaryColor : ui.theme.backgroundPrimaryColor
        border.width: cardNav.active ? 2 : 1
        border.color: cardNav.active ? ui.theme.focusColor
                                     : cardMouse.containsMouse ? ui.theme.accentColor : ui.theme.strokeColor

        readonly property color ink: card.primary ? ui.theme.backgroundPrimaryColor : ui.theme.fontPrimaryColor

        scale: cardMouse.pressed ? 0.98 : 1.0
        Behavior on scale { NumberAnimation { duration: 80 } }

        NavigationControl {
            id: cardNav

            name: card.title
            enabled: card.enabled && card.visible
            accessible.role: MUAccessible.Button
            accessible.name: card.title

            onTriggered: {
                card.clicked()
            }
        }

        Rectangle {
            id: iconDisc

            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.top: parent.top
            anchors.topMargin: 14

            width: 32
            height: 32
            radius: 16
            color: card.primary ? Qt.alpha(card.ink, 0.16) : Qt.alpha(ui.theme.accentColor, 0.45)

            StyledIconLabel {
                anchors.centerIn: parent
                iconCode: card.icon
                color: card.ink
            }
        }

        StyledTextLabel {
            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: subtitleLabel.top
            anchors.bottomMargin: 2

            horizontalAlignment: Text.AlignLeft
            font: ui.theme.bodyBoldFont
            color: card.ink
            text: card.title
        }

        StyledTextLabel {
            id: subtitleLabel

            anchors.left: parent.left
            anchors.leftMargin: 14
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 12

            horizontalAlignment: Text.AlignLeft
            color: card.ink
            opacity: 0.6
            text: card.subtitle
        }

        MouseArea {
            id: cardMouse

            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                card.clicked()
            }
        }
    }

    NavigationPanel {
        id: navStartPanel
        name: "HomeStartCards"
        section: navSec
        direction: NavigationPanel.Horizontal
        order: 2
        accessible.name: qsTrc("project", "New score")
    }

    // KitNote: the start cards (the templates, then four others) share the width of the page, down to 132
    readonly property real startCardWidth: Math.max(132, Math.min(200,
        Math.floor((startRow.width - 12 * (quickStartRepeater.count + 3)) / (quickStartRepeater.count + 4))))

    Flickable {
        id: startRow

        anchors.top: topLayout.bottom
        anchors.topMargin: 28
        anchors.left: parent.left
        anchors.leftMargin: prv.sideMargin
        anchors.right: parent.right
        anchors.rightMargin: prv.sideMargin

        height: 104
        contentWidth: startCards.width
        contentHeight: height
        clip: true
        boundsBehavior: Flickable.StopAtBounds
        flickableDirection: Flickable.HorizontalFlick

        Row {
            id: startCards

            spacing: 12

            Repeater {
                id: quickStartRepeater

                model: scoresPageModel.quickStartTemplates()

                StartCard {
                    required property var modelData
                    required property int index

                    icon: IconCode.PERCUSSION
                    title: modelData.title
                    subtitle: qsTrc("project", "4/4, 32 measures")
                    primary: index === 0

                    navigation.panel: navStartPanel
                    navigation.column: index

                    onClicked: {
                        scoresPageModel.createNewScoreFromTemplate(modelData.path)
                    }
                }
            }

            StartCard {
                icon: IconCode.NEW_FILE
                title: qsTrc("project", "Set it up yourself")
                subtitle: qsTrc("project", "Kit, meter, tempo, length")

                navigation.panel: navStartPanel
                navigation.column: 20

                onClicked: {
                    scoresPageModel.createNewScore()
                }
            }

            StartCard {
                icon: IconCode.MICROPHONE
                title: qsTrc("project", "From a recording")
                subtitle: qsTrc("project", "The drums of a song, by AI")

                navigation.panel: navStartPanel
                navigation.column: 22

                onClicked: {
                    scoresPageModel.scoreFromRecording()
                }
            }

            StartCard {
                icon: IconCode.OPEN_FILE
                title: qsTrc("project", "Open a file")
                subtitle: qsTrc("project", "KitNote, MuseScore, MusicXML, MIDI")

                navigation.panel: navStartPanel
                navigation.column: 21

                onClicked: {
                    scoresPageModel.openOther()
                }
            }
        }
    }

    RowLayout {
        id: controlsRow

        anchors.top: startRow.bottom
        anchors.topMargin: 32
        anchors.left: parent.left
        anchors.leftMargin: prv.sideMargin
        anchors.right: parent.right
        anchors.rightMargin: prv.sideMargin

        spacing: 12

        StyledTextLabel {
            Layout.fillWidth: true

            text: qsTrc("project", "Recent scores")
            font: ui.theme.largeBodyBoldFont
            horizontalAlignment: Text.AlignLeft
        }

        NavigationPanel {
            id: viewButtonsNavPanel
            name: "ViewButtons"
            enabled: viewTypeRadioButtons.enabled && viewTypeRadioButtons.visible
            section: navSec
            order: 3
            direction: NavigationPanel.Horizontal
            accessible.name: qsTrc("project", "View buttons")
        }

        RadioButtonGroup {
            id: viewTypeRadioButtons

            property int navigationOrderStart: 1

            implicitHeight: ui.theme.defaultButtonSize

            model: [
                { "icon": IconCode.GRID, "title": qsTrc("project", "Grid view"), "value": ScoresPageModel.Grid },
                { "icon": IconCode.LIST, "title": qsTrc("project", "List view"), "value": ScoresPageModel.List }
            ]

            delegate: FlatRadioButton {
                implicitWidth: ui.theme.defaultButtonSize
                implicitHeight: ui.theme.defaultButtonSize

                checked: scoresPageModel.viewType === modelData.value

                iconCode: modelData.icon
                transparent: true
                checkedColor: ui.theme.buttonColor

                navigation.name: "ViewType_" + modelData.title
                navigation.panel: viewButtonsNavPanel
                navigation.order: viewTypeRadioButtons.navigationOrderStart + model.index
                navigation.accessible.name: modelData.title

                onToggled: {
                    scoresPageModel.viewType = modelData.value
                }
            }
        }
    }

    Loader {
        id: contentLoader

        anchors.top: controlsRow.bottom
        anchors.topMargin: 16
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: buttonsPanel.top

        sourceComponent: newAndRecentComp
    }

    Component {
        id: newAndRecentComp

        RecentScoresView {
            anchors.fill: parent

            viewType: scoresPageModel.viewType
            searchText: searchField.searchText

            backgroundColor: background.color
            sideMargin: prv.sideMargin

            navigationSection: navSec
            navigationOrder: 4

            onCreateNewScoreRequested: {
                scoresPageModel.createNewScore()
            }

            onOpenScoreRequested: function(scorePath, displayName) {
                Qt.callLater(scoresPageModel.openScore, scorePath, displayName)
            }
        }
    }

    Item {
        id: buttonsPanel

        anchors.bottom: parent.bottom

        height: 0
        width: parent.width
    }
}
