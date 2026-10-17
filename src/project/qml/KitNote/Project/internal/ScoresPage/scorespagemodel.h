/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#include <QVariantList>
#include <qqmlintegration.h>

#include "modularity/ioc.h"
#include "iprojectconfiguration.h"
#include "internal/itemplatesrepository.h"
#include "actions/iactionsdispatcher.h"
#include "iinteractive.h"

class QString;

namespace kn::project {
class ScoresPageModel : public QObject, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(ViewType viewType READ viewType WRITE setViewType NOTIFY viewTypeChanged)

    QML_ELEMENT

    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<ITemplatesRepository> templatesRepository = { this };
public:
    explicit ScoresPageModel(QObject* parent = nullptr);

    enum ViewType {
        Grid = int(IProjectConfiguration::HomeScoresPageViewType::Grid),
        List = int(IProjectConfiguration::HomeScoresPageViewType::List),
    };
    Q_ENUM(ViewType);

    ViewType viewType() const;
    void setViewType(ViewType type);

    Q_INVOKABLE void createNewScore();

    // KitNote: the bundled templates, each as { title, path }, and a new score made from one of them
    Q_INVOKABLE QVariantList quickStartTemplates() const;
    Q_INVOKABLE void createNewScoreFromTemplate(const QString& templatePath);
    Q_INVOKABLE void openOther();
    Q_INVOKABLE void scoreFromRecording(); // KitNote
    Q_INVOKABLE void openScore(const QString& scorePath, const QString& displayNameOverride);

signals:
    void viewTypeChanged();
};
}
