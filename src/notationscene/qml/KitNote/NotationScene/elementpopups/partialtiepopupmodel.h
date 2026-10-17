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

#include "uicomponents/qml/KitNote/UiComponents/menuitem.h"

#include "../abstractelementpopupmodel.h"

namespace kn::engraving {
class Tie;
class TieJumpPoint;
}

namespace kn::notation {
class PartialTiePopupModel : public AbstractElementPopupModel
{
    Q_OBJECT
    QML_ELEMENT;

    Q_PROPERTY(QVariantList items READ items NOTIFY itemsChanged)
    Q_PROPERTY(bool tieDirection READ tieDirection NOTIFY tieDirectionChanged)
    Q_PROPERTY(QPointF dialogPosition READ dialogPosition CONSTANT)

public:
    explicit PartialTiePopupModel(QObject* parent = nullptr);

    QVariantList items() const;
    bool tieDirection() const;
    static bool canOpen(const EngravingItem* element);
    QPointF dialogPosition() const;

    Q_INVOKABLE void init() override;
    Q_INVOKABLE void toggleItemChecked(const QString& id);

    Q_INVOKABLE void onClosed();

signals:
    void tieDirectionChanged(bool direction);
    void itemsChanged();

private:
    void load();
    kfw::uicomponents::MenuItemList makeMenuItems();
    kfw::uicomponents::MenuItem* makeMenuItem(const engraving::TieJumpPoint* jumpPoint);
    kn::engraving::Tie* tie() const;

    kfw::uicomponents::MenuItemList m_items;
};
}
