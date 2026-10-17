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

#pragma once

#include "engraving/infrastructure/ifileinfoprovider.h"

#include "modularity/ioc.h"
#include "io/ifilesystem.h"

namespace kn::project {
class NotationProject;
class ProjectFileInfoProvider : public engraving::IFileInfoProvider
{
    kfw::GlobalInject<kfw::io::IFileSystem> filesystem;

public:
    explicit ProjectFileInfoProvider(NotationProject* project);

    bool saved() const override;

    kfw::io::path_t path() const override;
    kfw::io::path_t fileName(bool includingExtension = true) const override;
    kfw::io::path_t absoluteDirPath() const override;

    kfw::String displayName() const override;

    kfw::DateTime birthTime() const override;
    kfw::DateTime lastModified() const override;
    bool isNewlyCreated() const override;

private:
    NotationProject* m_project = nullptr;
};
}
