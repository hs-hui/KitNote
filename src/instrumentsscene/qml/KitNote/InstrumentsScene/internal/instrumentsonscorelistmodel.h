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

#include "uicomponents/qml/KitNote/UiComponents/selectableitemlistmodel.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "notation/iinstrumentsrepository.h"

namespace kn::instrumentsscene {
class InstrumentsOnScoreListModel : public kfw::uicomponents::SelectableItemListModel, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(QStringList orders READ orders NOTIFY ordersChanged)
    Q_PROPERTY(int currentOrderIndex READ currentOrderIndex WRITE setCurrentOrderIndex NOTIFY currentOrderChanged)

    QML_ELEMENT

    kfw::ContextInject<context::IGlobalContext> context = { this };
    kfw::ContextInject<notation::IInstrumentsRepository> repository = { this };

public:
    InstrumentsOnScoreListModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    QStringList orders() const;
    int currentOrderIndex() const;

    Q_INVOKABLE void load();
    Q_INVOKABLE void addInstruments(const QStringList& instrumentIdList);

    Q_INVOKABLE QVariant currentOrder() const;
    Q_INVOKABLE QVariantList instruments() const;

public slots:
    void setCurrentOrderIndex(int index);

signals:
    void ordersChanged();
    void currentOrderChanged();

private:
    class InstrumentItem;

    enum Roles {
        RoleName = SelectableItemListModel::UserRole + 1,
        RoleDescription,
        RoleIsSoloist
    };

    void loadOrders();

    int resolveInstrumentSequenceNumber(const kfw::String& instrumentId) const;
    void updateInstrumentsOrder();
    void sortInstruments(ItemList& instruments);
    void insertInstrument(ItemList& instruments, InstrumentItem* newInstrument);

    InstrumentItem* modelIndexToItem(const QModelIndex& index) const;
    const notation::ScoreOrder& currentScoreOrder() const;

    void onRowsMoved() override;
    void onRowsRemoved() override;
    void doSetCurrentOrderIndex(int index);
    bool matchesScoreOrder() const;
    void verifyScoreOrder();
    int createCustomizedScoreOrder(const notation::ScoreOrder& order);
    void removeCustomizedScoreOrder(const notation::ScoreOrder& order);

    notation::ScoreOrderList m_scoreOrders;
    int m_currentOrderIndex = 0;
};
}
