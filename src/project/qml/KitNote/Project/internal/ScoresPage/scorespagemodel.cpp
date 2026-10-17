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

#include "scorespagemodel.h"

#include <QString>

#include "actions/actiontypes.h"
#include "translation.h"

using namespace kn::project;
using namespace kfw::actions;

ScoresPageModel::ScoresPageModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void ScoresPageModel::createNewScore()
{
    dispatcher()->dispatch("file-new");
}

QVariantList ScoresPageModel::quickStartTemplates() const
{
    QVariantList result;

    const kfw::RetVal<Templates> templates = templatesRepository()->templates();
    for (const Template& templ : templates.val) {
        if (templ.isCustom || templ.meta.title.isEmpty()) {
            continue;
        }

        result << QVariantMap {
            { "title", kfw::qtrc("project", templ.meta.title.toUtf8().constData()) },
            { "path", templ.meta.filePath.toQString() }
        };
    }

    return result;
}

void ScoresPageModel::createNewScoreFromTemplate(const QString& templatePath)
{
    dispatcher()->dispatch("file-new-from-template", ActionData::make_arg1<QString>(templatePath));
}

// KitNote: File > Drum score from a recording
void ScoresPageModel::scoreFromRecording()
{
    dispatcher()->dispatch("kitnote-audio-to-score");
}

void ScoresPageModel::openOther()
{
    dispatcher()->dispatch("file-open");
}

void ScoresPageModel::openScore(const QString& scorePath, const QString& displayNameOverride)
{
    dispatcher()->dispatch("file-open", ActionData::make_arg2<QUrl, QString>(QUrl::fromLocalFile(scorePath), displayNameOverride));
}

ScoresPageModel::ViewType ScoresPageModel::viewType() const
{
    return static_cast<ViewType>(configuration()->homeScoresPageViewType());
}

void ScoresPageModel::setViewType(ViewType type)
{
    if (viewType() == type) {
        return;
    }

    configuration()->setHomeScoresPageViewType(IProjectConfiguration::HomeScoresPageViewType(type));
    emit viewTypeChanged();
}
