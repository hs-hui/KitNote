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
#ifndef KFW_WORKSPACE_IWORKSPACESDATAPROVIDER_H
#define KFW_WORKSPACE_IWORKSPACESDATAPROVIDER_H

#include <QByteArray>

#include "modularity/imoduleinterface.h"
#include "workspacetypes.h"
#include "types/retval.h"
#include "async/notification.h"

namespace kfw::workspace {
class IWorkspacesDataProvider : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IWorkspacesDataProvider)
public:
    virtual ~IWorkspacesDataProvider() = default;

    virtual RetVal<QByteArray> rawData(DataKey key) const = 0;
    virtual Ret setRawData(DataKey key, const QByteArray& data) = 0;
    virtual async::Notification dataChanged(DataKey key) const = 0;

    virtual async::Notification workspaceChanged() const = 0;
};
}

#endif // KFW_WORKSPACE_IWORKSPACESDATAPROVIDER_H
