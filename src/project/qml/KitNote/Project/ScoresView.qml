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

import KitNote.UiComponents
import KitNote.Ui
import KitNote.Project

Loader {
    id: root

    property string searchText

    property int viewType: ScoresPageModel.Grid

    property color backgroundColor: ui.theme.backgroundSecondaryColor
    property real sideMargin: 46

    property NavigationSection navigationSection
    property int navigationOrder

    signal createNewScoreRequested()
    signal openScoreRequested(var scorePath, var displayName)
}
