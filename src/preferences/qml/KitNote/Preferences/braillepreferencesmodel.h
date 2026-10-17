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

#include "async/asyncable.h"
#include "modularity/ioc.h"
#include "braille/ibrailleconfiguration.h"

namespace kn::preferences {
class BraillePreferencesModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT
    QML_ELEMENT;

    Q_PROPERTY(bool braillePanelEnabled READ braillePanelEnabled WRITE setBraillePanelEnabled NOTIFY braillePanelEnabledChanged)
    Q_PROPERTY(QString brailleTable READ brailleTable WRITE setBrailleTable NOTIFY brailleTableChanged)
    Q_PROPERTY(int intervalDirection READ intervalDirection WRITE setIntervalDirection NOTIFY intervalDirectionChanged)

    kfw::GlobalInject<braille::IBrailleConfiguration> brailleConfiguration;

public:
    explicit BraillePreferencesModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();

    bool braillePanelEnabled() const;
    QString brailleTable() const;
    int intervalDirection() const;

    Q_INVOKABLE QStringList brailleTables() const;
    Q_INVOKABLE QVariantList intervalDirections() const;

public slots:
    void setBraillePanelEnabled(bool value);
    void setBrailleTable(QString table);
    void setIntervalDirection(int direction);

signals:
    void braillePanelEnabledChanged(bool value);
    void brailleTableChanged(QString value);
    void intervalDirectionChanged(int value);
};
}
