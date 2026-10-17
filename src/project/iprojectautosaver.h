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
#ifndef KN_PROJECT_IPROJECTAUTOSAVER_H
#define KN_PROJECT_IPROJECTAUTOSAVER_H

#include "io/path.h"

#include "modularity/imoduleinterface.h"

namespace kn::project {
class IProjectAutoSaver : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IProjectAutoSaver)

public:
    virtual ~IProjectAutoSaver() = default;

    virtual bool projectHasUnsavedChanges(const kfw::io::path_t& projectPath) const = 0;
    virtual void removeProjectUnsavedChanges(const kfw::io::path_t& projectPath) = 0;

    virtual bool isAutosaveOfNewlyCreatedProject(const kfw::io::path_t& projectPath) const = 0;

    virtual kfw::io::path_t projectOriginalPath(const kfw::io::path_t& projectAutoSavePath) const = 0;
    virtual kfw::io::path_t projectAutoSavePath(const kfw::io::path_t& projectPath) const = 0;

    static inline const std::string AUTOSAVE_SUFFIX = "autosave";
};
}

#endif // KN_PROJECT_IPROJECTAUTOSAVER_H
