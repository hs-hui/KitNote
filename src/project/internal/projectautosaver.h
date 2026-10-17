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
#ifndef KN_PROJECT_PROJECTAUTOSAVER_H
#define KN_PROJECT_PROJECTAUTOSAVER_H

#include <QTimer>

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "io/ifilesystem.h"
#include "iprojectconfiguration.h"

#include "../iprojectautosaver.h"

namespace kn::project {
class ProjectAutoSaver : public IProjectAutoSaver, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;
    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    ProjectAutoSaver(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {}

    void init();

    bool projectHasUnsavedChanges(const kfw::io::path_t& projectPath) const override;
    void removeProjectUnsavedChanges(const kfw::io::path_t& projectPath) override;

    bool isAutosaveOfNewlyCreatedProject(const kfw::io::path_t& projectPath) const override;

    kfw::io::path_t projectOriginalPath(const kfw::io::path_t& projectAutoSavePath) const override;
    kfw::io::path_t projectAutoSavePath(const kfw::io::path_t& projectPath) const override;

private:
    INotationProjectPtr currentProject() const;

    void update();

    void onTrySave();

    kfw::io::path_t projectPath(INotationProjectPtr project) const;

    QTimer m_timer;
    kfw::io::path_t m_lastProjectPathNeedingAutosave;
};
}

#endif // KN_PROJECT_PROJECTAUTOSAVER_H
