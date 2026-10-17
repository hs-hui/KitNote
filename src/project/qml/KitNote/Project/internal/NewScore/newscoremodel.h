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

#include <QObject>
#include <qqmlintegration.h>

#include "modularity/ioc.h"

#include "project/iprojectconfiguration.h"
#include "project/iprojectcreator.h"
#include "notation/notationtypes.h"
#include "context/iglobalcontext.h"
#include "iinteractive.h"
#include "notation/iinstrumentsrepository.h"

namespace kn::project {
class NewScoreModel : public QObject, public kfw::Contextable
{
    Q_OBJECT

    QML_ELEMENT

    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::GlobalInject<IProjectCreator> notationCreator;
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<notation::IInstrumentsRepository> instrumentsRepository = { this };

public:
    explicit NewScoreModel(QObject* parent = nullptr);

    Q_INVOKABLE QString preferredScoreCreationMode() const;
    Q_INVOKABLE bool createScore(const QVariant& info);

private:
    project::ProjectCreateOptions parseOptions(const QVariantMap& info) const;
    notation::DurationType noteIconToDurationType(int noteIconCode) const;
    void updatePreferredScoreCreationMode(bool isScoreCreatedFromInstruments);
};
}
