# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2026 MuseScore Limited and others
# Modified for KitNote, 2026
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

kfw_create_module(kfw_workspace ALIAS kfw::workspace)

set(MODULE_QRC workspace.qrc)

target_sources(kfw_workspace PRIVATE
    workspacemodule.cpp
    workspacemodule.h
    workspacetypes.h
    workspaceerrors.h
    iworkspaceconfiguration.h
    iworkspace.h
    iworkspacemanager.h
    iworkspacesdataprovider.h

    internal/workspaceconfiguration.cpp
    internal/workspaceconfiguration.h
    internal/workspace.cpp
    internal/workspace.h
    internal/workspacemanager.cpp
    internal/workspacemanager.h
    internal/workspacesdataprovider.cpp
    internal/workspacesdataprovider.h
    internal/workspacefile.cpp
    internal/workspacefile.h
    internal/workspaceuiactions.cpp
    internal/workspaceuiactions.h
    internal/workspaceactioncontroller.cpp
    internal/workspaceactioncontroller.h
    internal/workspaceutils.cpp
    internal/workspaceutils.h
)

if (KFW_MODULE_WORKSPACE_QML)
    add_subdirectory(qml/KitNote/Workspace)
endif()
