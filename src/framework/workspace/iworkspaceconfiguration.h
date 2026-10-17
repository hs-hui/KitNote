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
#ifndef KFW_WORKSPACE_IWORKSPACECONFIGURATION_H
#define KFW_WORKSPACE_IWORKSPACECONFIGURATION_H

#include <vector>

#include "io/path.h"
#include "modularity/imoduleinterface.h"
#include "async/channel.h"

namespace kfw::workspace {
class IWorkspaceConfiguration : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IWorkspaceConfiguration)

public:
    virtual ~IWorkspaceConfiguration() = default;

    virtual io::paths_t workspacePaths() const = 0;

    virtual io::paths_t builtinWorkspacesFilePaths() const = 0;
    virtual io::path_t userWorkspacesPath() const = 0;

    virtual std::string defaultWorkspaceName() const = 0;

    virtual std::string currentWorkspaceName() const = 0;
    virtual void setCurrentWorkspaceName(const std::string& workspaceName) = 0;
    virtual async::Channel<std::string> currentWorkspaceNameChanged() const = 0;
};
}

#endif // KFW_WORKSPACE_IWORKSPACECONFIGURATION_H
