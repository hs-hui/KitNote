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

ColumnLayout {
    id: root

    property ArticulationsProfileEditorModel model: null

    height: 400
    width: 400
    spacing: 12

    StyledTabBar {
        id: tabBar
        width: parent.width
        spacing: 12

        StyledTabButton {
            fillWidth: true
            text: /*qsTrc*/ "Single note"
        }

        StyledTabButton {
            fillWidth: true
            text: /*qsTrc*/ "Multi note"
        }
    }

    StackLayout {
        currentIndex: tabBar.currentIndex

        ArticulationPatternsList {
            editorModel: root.model
            model: root.model ? root.model.singleNoteItems : null
        }

        ArticulationPatternsList {
            editorModel: root.model
            model: root.model ? root.model.multiNoteItems : null
        }
    }
}
