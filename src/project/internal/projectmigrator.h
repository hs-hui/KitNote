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
#ifndef KN_PROJECT_PROJECTMIGRATOR_H
#define KN_PROJECT_PROJECTMIGRATOR_H

#include "iprojectmigrator.h"

#include "modularity/ioc.h"
#include "iinteractive.h"
#include "iprojectconfiguration.h"

namespace kn::project {
class ProjectMigrator : public IProjectMigrator, public kfw::Contextable
{
    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
public:
    ProjectMigrator(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    kfw::Ret migrateEngravingProjectIfNeed(engraving::EngravingProjectPtr project) override;

private:

    kfw::Ret askAboutMigration(MigrationOptions& out, const QString& appVersion, MigrationType migrationType);

    kfw::Ret migrateProject(engraving::EngravingProjectPtr project, const MigrationOptions& opt);

    bool applyLelandStyle(kn::engraving::MasterScore* score);
    bool applyEdwinStyle(kn::engraving::MasterScore* score);
    bool resetAllElementsPositions(kn::engraving::MasterScore* score);
    bool resetCrossBeams(kn::engraving::MasterScore* score);
    void resetStyleSettings(kn::engraving::MasterScore* score);

    bool m_resetStyleSettings{ false };
};
}

#endif // KN_PROJECT_PROJECTMIGRATOR_H
