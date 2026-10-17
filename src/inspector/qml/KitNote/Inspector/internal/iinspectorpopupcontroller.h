/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2026 MuseScore Limited and others
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

#pragma once

#include "modularity/ioc.h"
#include "async/notification.h"

class QQuickItem;

namespace kfw::uicomponents {
class PopupView;
class DropdownView;
class MenuView;
}

namespace kn::inspector {
class IInspectorPopupController : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IInspectorPopupController)

public:
    virtual void setNotationView(const QQuickItem* view) = 0;

    virtual kfw::uicomponents::PopupView* popup() const = 0;
    virtual void setPopup(kfw::uicomponents::PopupView* popup, QQuickItem* control = nullptr) = 0;
    virtual kfw::async::Notification popupChanged() const = 0;

    virtual kfw::uicomponents::DropdownView* dropdown() const = 0;
    virtual void setDropdown(kfw::uicomponents::DropdownView* dropdown) = 0;
    virtual kfw::async::Notification dropdownChanged() const = 0;

    virtual kfw::uicomponents::MenuView* menu() const = 0;
    virtual void setMenu(kfw::uicomponents::MenuView* menu) = 0;
    virtual kfw::async::Notification menuChanged() const = 0;
};
}
