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

import KitNote.Shortcuts

QtObject {
    id: root

    Component.onCompleted: {
        shortcutsModel.init()
    }

    property var objects: []

    property ShortcutsInstanceModel model: ShortcutsInstanceModel {
        id: shortcutsModel

        onShortcutsChanged: {
            for (var s = 0; s < root.objects.length; ++s) {
                root.objects[s].enabled = false
                root.objects[s].destroy()
            }
            root.objects = []

            for (var key in shortcutsModel.shortcuts) {
                var autoRepeat = Boolean(shortcutsModel.shortcuts[key]);
                var obj = shortcutComponent.createObject(root, {sequence: key, autoRepeat: autoRepeat})
                root.objects.push(obj)
            }
        }
    }

    property Component component: Component {
        id: shortcutComponent
        Shortcut {
            context: Qt.ApplicationShortcut
            enabled: shortcutsModel.active
            onActivated: shortcutsModel.activate(sequence)
        }
    }
}
