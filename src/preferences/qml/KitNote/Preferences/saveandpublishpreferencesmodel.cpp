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
#include "saveandpublishpreferencesmodel.h"

using namespace kn::preferences;

SaveAndPublishPreferencesModel::SaveAndPublishPreferencesModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void SaveAndPublishPreferencesModel::load()
{
    projectConfiguration()->autoSaveEnabledChanged().onReceive(this, [this](bool enabled) {
        emit autoSaveEnabledChanged(enabled);
    });

    projectConfiguration()->autoSaveIntervalChanged().onReceive(this, [this](int minutes) {
        emit autoSaveIntervalChanged(minutes);
    });

}

bool SaveAndPublishPreferencesModel::isAutoSaveEnabled() const
{
    return projectConfiguration()->isAutoSaveEnabled();
}

int SaveAndPublishPreferencesModel::autoSaveInterval() const
{
    return projectConfiguration()->autoSaveIntervalMinutes();
}

void SaveAndPublishPreferencesModel::setAutoSaveEnabled(bool enabled)
{
    if (enabled == isAutoSaveEnabled()) {
        return;
    }

    projectConfiguration()->setAutoSaveEnabled(enabled);
}

void SaveAndPublishPreferencesModel::setAutoSaveInterval(int minutes)
{
    if (minutes == autoSaveInterval()) {
        return;
    }

    projectConfiguration()->setAutoSaveInterval(minutes);
}
