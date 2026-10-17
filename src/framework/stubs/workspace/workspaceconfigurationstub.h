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
#ifndef KFW_WORKSPACE_WORKSPACECONFIGURATIONSTUB_H
#define KFW_WORKSPACE_WORKSPACECONFIGURATIONSTUB_H

#include "workspace/iworkspaceconfiguration.h"

namespace kfw::workspace {
class WorkspaceConfigurationStub : public IWorkspaceConfiguration
{
public:

    io::paths_t workspacePaths() const override;

    io::paths_t builtinWorkspacesFilePaths() const override;
    io::path_t userWorkspacesPath() const override;

    std::string defaultWorkspaceName() const override;

    std::string currentWorkspaceName() const override;
    void setCurrentWorkspaceName(const std::string& workspaceName) override;
    async::Channel<std::string> currentWorkspaceNameChanged() const override;
};
}

#endif // KN_WORKSPACE_WORKSPACECONFIGURATIONSTUB_H
