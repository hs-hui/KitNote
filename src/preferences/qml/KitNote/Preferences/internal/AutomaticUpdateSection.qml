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

import KitNote.Ui
import KitNote.UiComponents

BaseSection {
    id: root

    title: qsTrc("preferences", "Automatic update check")

    property bool isAppUpdatable: true
    property alias needCheckForNewAppVersion: needCheckBox.checked
    property string kitNotePrivacyPolicyUrl

    signal needCheckForNewAppVersionChangeRequested(bool check)

    CheckBox {
        id: needCheckBox
        width: parent.width

        text: qsTrc("preferences", "Check to see if a new version of KitNote is available")

        visible: root.isAppUpdatable

        navigation.name: "NeedCheckBox"
        navigation.panel: root.navigation
        navigation.row: 0

        onClicked: {
            root.needCheckForNewAppVersionChangeRequested(!checked)
        }
    }

    StyledTextLabel {
        width: parent.width

        text: qsTrc("preferences", "Update checking requires network access. In order to protect your privacy, KitNote does not store any personal information. See our <a href=\"%1\">privacy policy</a> for more info.")
              .arg(root.kitNotePrivacyPolicyUrl)
              .replace("\n", "<br>")

        horizontalAlignment: Qt.AlignLeft
        wrapMode: Text.WordWrap
        maximumLineCount: 3
    }
}
