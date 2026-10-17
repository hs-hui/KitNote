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

#pragma once

#include "settings.h"

// KitNote: at which edge of the score the rail with the main tools floats (Preferences > Appearance).
// The score page has no row of toolbars above it; the rail (NotationPage/ZenBar.qml) holds what that
// row held in MuseScore Studio: the way back to Home, note input, the panels, playback, undo.
namespace kn::appshell {
enum class ToolbarPosition {
    Top = 0,
    Left = 1,
    Right = 2,
    Bottom = 3
};

inline const kfw::Settings::Key& toolbarPositionKey()
{
    // The default also gives the stored value its type: read back from the settings file it is a string.
    static const kfw::Settings::Key key = [] {
        const kfw::Settings::Key k("appshell", "ui/kitnote/toolbarPosition");
        kfw::settings()->setDefaultValue(k, kfw::Val(static_cast<int>(ToolbarPosition::Left)));
        return k;
    }();
    return key;
}

inline ToolbarPosition toolbarPosition()
{
    const int value = kfw::settings()->value(toolbarPositionKey()).toInt();
    return value >= 0 && value <= 3 ? static_cast<ToolbarPosition>(value) : ToolbarPosition::Left;
}

inline void setToolbarPosition(ToolbarPosition position)
{
    kfw::settings()->setSharedValue(toolbarPositionKey(), kfw::Val(static_cast<int>(position)));
}
}
