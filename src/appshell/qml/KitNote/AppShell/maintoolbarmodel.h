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

#include <QAbstractListModel>
#include <qqmlintegration.h>

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "iglobalconfiguration.h"

namespace kn::appshell {
class MainToolBarModel : public QAbstractListModel, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT

    QML_ELEMENT

    kfw::ContextInject<context::IGlobalContext> context = { this };
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;

public:
    explicit MainToolBarModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void load();

private:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        UriRole,
        IsTitleBoldRole
    };

    void updateNotationPageItem();

    QList<QVariantMap> m_items;
};
}
