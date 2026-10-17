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

#include <QAbstractItemModel>

#include "modularity/ioc.h"
#include "async/asyncable.h"
#include "actions/iactionsdispatcher.h"
#include "ui/iuiactionsregister.h"
#include "ui/view/iconcodes.h"

#include "appshell/iappshellconfiguration.h"

#include "preferencepageitem.h"
#include "iinteractive.h"

namespace kn::preferences {
class PreferencesModel : public QAbstractItemModel, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT
    QML_ELEMENT;

    Q_PROPERTY(QString currentPageId READ currentPageId WRITE setCurrentPageId NOTIFY currentPageIdChanged)

    kfw::GlobalInject<appshell::IAppShellConfiguration> configuration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::ui::IUiActionsRegister> actionsRegister = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    explicit PreferencesModel(QObject* parent = nullptr);
    ~PreferencesModel();

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QString currentPageId() const;

    Q_INVOKABLE void load(const QString& currentPageId);
    Q_INVOKABLE void askForConfirmationOfPreferencesReset();
    Q_INVOKABLE void resetFactorySettings();
    Q_INVOKABLE void apply();
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void selectRow(const QModelIndex& rowIndex);

    Q_INVOKABLE QVariantList availablePages() const;

public slots:
    void setCurrentPageId(QString currentPageId);

signals:
    void userAgreesToResetPreferences();
    void currentPageIdChanged(QString currentPageId);

private:

    enum RoleNames {
        ItemRole = Qt::UserRole + 1
    };

    PreferencePageItem* makeItem(const QString& id, const QString& title, kfw::ui::IconCode::Code icon = kfw::ui::IconCode::Code::NONE,
                                 const QString& path = "", const QList<PreferencePageItem*>& children = {}) const;

    PreferencePageItem* modelIndexToItem(const QModelIndex& index) const;

    PreferencePageItem* m_rootItem = nullptr;
    QString m_currentPageId;
};
}
