/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include <QAbstractListModel>
#include <QHash>
#include <qqmlintegration.h>

#include "async/asyncable.h"

#include "context/iglobalcontext.h"
#include "modularity/ioc.h"

namespace kn::engraving {
class EngravingStyleModel : public QAbstractListModel, public kfw::async::Asyncable, public kfw::Contextable
{
    Q_OBJECT
    QML_ELEMENT;

    kfw::ContextInject<context::IGlobalContext> context = { this };

public:
    explicit EngravingStyleModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void init();
    Q_INVOKABLE void changeVal(int index, QVariant newVal);

private:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        KeyRole,
        TypeRole,
        ValueRole,
        isDefaultRole
    };

    QString typeToString(P_TYPE pt) const;

    void onNotationChanged();
};
}
