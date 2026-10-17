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

import KitNote.UiComponents
import KitNote.Project

//! KitNote: exporting as a drum tab has no settings; the page says what the file will be.
ExportSettingsPage {
    id: root

    StyledTextLabel {
        width: parent.width
        text: qsTrc("project/export", "A text file with the score as a drum tab: a line for every drum, a column for every sixteenth note. It lines up in a font with characters of equal width.")
        horizontalAlignment: Text.AlignLeft
        wrapMode: Text.WordWrap
    }
}
