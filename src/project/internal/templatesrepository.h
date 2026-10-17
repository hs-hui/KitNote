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

#ifndef KN_PROJECT_TEMPLATESREPOSITORY_H
#define KN_PROJECT_TEMPLATESREPOSITORY_H

#include "modularity/ioc.h"

#include "itemplatesrepository.h"
#include "project/iprojectconfiguration.h"
#include "project/imscmetareader.h"
#include "io/ifilesystem.h"

namespace kn::project {
class TemplatesRepository : public ITemplatesRepository
{
public:
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;
    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::GlobalInject<IMscMetaReader> mscReader;

public:
    TemplatesRepository() = default;

    kfw::RetVal<Templates> templates() const override;

private:
    Templates readTemplates(const kfw::io::path_t& dirPath) const;

    Templates readTemplates(const kfw::io::paths_t& files, const QString& category, bool isCustom,
                            const kfw::io::path_t& dirPath = kfw::io::path_t()) const;
};
}

#endif // KN_PROJECT_TEMPLATESREPOSITORY_H
