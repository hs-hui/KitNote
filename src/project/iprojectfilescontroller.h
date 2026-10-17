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
#ifndef KN_PROJECT_IPROJECTFILESCONTROLLER_H
#define KN_PROJECT_IPROJECTFILESCONTROLLER_H

#include "modularity/imoduleinterface.h"
#include "types/ret.h"
#include "io/path.h"

#include "types/projecttypes.h"

class QUrl;

namespace kn::project {
class IProjectFilesController : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IProjectFilesController)

public:
    virtual ~IProjectFilesController() = default;

    virtual bool isUrlSupported(const QUrl& url) const = 0;
    virtual bool isFileSupported(const kfw::io::path_t& path) const = 0;
    virtual kfw::Ret openProject(const ProjectFile& file) = 0;
    virtual bool closeOpenedProject(bool goToHome = true) = 0;
    virtual bool saveProject(const kfw::io::path_t& path = kfw::io::path_t()) = 0;
    virtual bool saveProjectLocally(
        const kfw::io::path_t& path = kfw::io::path_t(), SaveMode saveMode = SaveMode::Save, bool createBackup = true) = 0;

};
}

#endif // KN_PROJECT_IPROJECTFILESCONTROLLER_H
