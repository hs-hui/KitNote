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

#include <qqmlintegration.h>

#include "abstractscoresmodel.h"

#include "async/asyncable.h"

#include "iprojectconfiguration.h"
#include "irecentfilescontroller.h"
#include "io/ifilesystem.h"
#include "modularity/ioc.h"

namespace kn::project {
class RecentScoresModel : public AbstractScoresModel, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT
    QML_ELEMENT
    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::ContextInject<IRecentFilesController> recentFilesController = { this };
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;

public:
    RecentScoresModel(QObject* parent = nullptr);

    void load() override;

    QList<int> nonScoreItemIndices() const override;

private:
    void updateRecentScores();
    void setRecentScores(const std::vector<QVariantMap>& items);
};
}
