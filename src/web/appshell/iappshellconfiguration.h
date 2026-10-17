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
#ifndef KN_APPSHELL_IAPPSHELLCONFIGURATION_H
#define KN_APPSHELL_IAPPSHELLCONFIGURATION_H

#include "modularity/imoduleinterface.h"
#include "async/notification.h"

namespace kn::appshell {
class IAppShellConfiguration : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IAppshellConfiguration)

public:
    virtual ~IAppShellConfiguration() = default;

    virtual std::string kitNoteVersion() const = 0;
    virtual std::string kitNoteRevision() const = 0;

    virtual bool isNotationNavigatorVisible() const = 0;
    virtual void setIsNotationNavigatorVisible(bool visible) const = 0;
    virtual kfw::async::Notification isNotationNavigatorVisibleChanged() const = 0;
};
}

#endif // KN_APPSHELL_IAPPSHELLCONFIGURATION_H
