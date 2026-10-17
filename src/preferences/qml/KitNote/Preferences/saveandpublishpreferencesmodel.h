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

#include <qqmlintegration.h>

#include <QObject>

#include "modularity/ioc.h"
#include "async/asyncable.h"

#include "project/iprojectconfiguration.h"

namespace kn::preferences {
class SaveAndPublishPreferencesModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT
    QML_ELEMENT;

    Q_PROPERTY(bool isAutoSaveEnabled READ isAutoSaveEnabled WRITE setAutoSaveEnabled NOTIFY autoSaveEnabledChanged)
    Q_PROPERTY(int autoSaveInterval READ autoSaveInterval WRITE setAutoSaveInterval NOTIFY autoSaveIntervalChanged)

    kfw::GlobalInject<project::IProjectConfiguration> projectConfiguration;

public:
    explicit SaveAndPublishPreferencesModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();

    bool isAutoSaveEnabled() const;
    int autoSaveInterval() const;

public slots:
    void setAutoSaveEnabled(bool enabled);
    void setAutoSaveInterval(int minutes);

signals:
    void autoSaveEnabledChanged(bool enabled);
    void autoSaveIntervalChanged(int minutes);
};
}
