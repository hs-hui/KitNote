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

#include <qqmlintegration.h>

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"

#include "uicomponents/qml/KitNote/UiComponents/selectableitemlistmodel.h"

namespace kn::engraving {
class FBox;
}

namespace kn::inspector {
class FretFrameChordItem;
class FretFrameChordListModel : public kfw::uicomponents::SelectableItemListModel, public kfw::Contextable
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable from QML")

    Q_PROPERTY(QItemSelectionModel * selectionModel READ selectionModel NOTIFY selectionChanged)

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    explicit FretFrameChordListModel(QObject* parent = nullptr);

    void load();

    void setFBox(engraving::FBox* box);

    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void setChordVisible(int index, bool visible);
    Q_INVOKABLE void moveSelectionUp();
    Q_INVOKABLE void moveSelectionDown();

    QItemSelectionModel* selectionModel() const;

private:
    enum Roles {
        ItemRole = SelectableItemListModel::UserRole + 1
    };

    FretFrameChordItem* modelIndexToItem(const QModelIndex& index) const;

    void saveOrder();

    engraving::FBox* m_fretBox = nullptr;
};
}
