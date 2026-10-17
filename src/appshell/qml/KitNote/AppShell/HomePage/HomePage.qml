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
import KitNote.Dock

import KitNote.Learn
import KitNote.Project

DockPage {
    id: root

    property string section: "scores"
    property string subSection: ""

    property var window: null

    objectName: "Home"
    uri: "kitnote://home"

    onSetParamsRequested: function(params) {
        if (Boolean(params["section"])) {
            setCurrentCentral(params["section"])

            if (Boolean(params["subSection"])) {
                subSection = params["subSection"]
            }
        }
    }

    onSectionChanged: {
        Qt.callLater(root.setCurrentCentral, section)
    }

    function setCurrentCentral(name) {
        if (section === name || !Boolean(name)) {
            return
        }

        section = name

        switch (name) {
        case "scores": root.central = scoresComp; break
        // KitNote: the Plugins page is gone; an old link to it lands on the scores
        case "plugins": root.central = scoresComp; break
        case "extensions": root.central = scoresComp; break
        case "learn": root.central = learnComp; break
        }
    }

    panels: [
        DockPanel {
            id: menuPanel

            objectName: "homeMenu"

            //! NOTE KitNote: Home has one page, the scores, so there is nothing to choose in a side menu
            visible: false

            readonly property int maxFixedWidth: 260
            readonly property int minFixedWidth: 76
            readonly property bool iconsOnly: root.window
                                                ? root.window.width < (root.window.minimumWidth + maxFixedWidth - minFixedWidth)
                                                : false
            readonly property int currentFixedWidth: iconsOnly ? minFixedWidth : maxFixedWidth

            width: currentFixedWidth
            minimumWidth: currentFixedWidth
            maximumWidth: currentFixedWidth

            floatable: false
            closable: false

            HomeMenu {
                currentPageName: root.section
                iconsOnly: menuPanel.iconsOnly

                onSelected: function(name) {
                    root.setCurrentCentral(name)
                }
            }
        }
    ]

    central: scoresComp

    Component {
        id: scoresComp

        ScoresPage {}
    }

    Component {
        id: learnComp

        LearnPage {
            section: root.subSection
        }
    }
}
