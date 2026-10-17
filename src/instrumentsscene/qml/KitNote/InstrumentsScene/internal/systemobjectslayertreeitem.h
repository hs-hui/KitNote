/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include "abstractlayoutpaneltreeitem.h"

#include "notation/inotation.h"

#include "layoutpanelutils.h"

namespace kn::instrumentsscene {
class SystemObjectsLayerTreeItem : public AbstractLayoutPanelTreeItem
{
    Q_OBJECT

    QML_ELEMENT;
    QML_UNCREATABLE("Must be created in C++ only")

public:
    SystemObjectsLayerTreeItem(notation::IMasterNotationPtr masterNotation, notation::INotationPtr notation, QObject* parent);

    void init(const kn::engraving::Staff* staff, const SystemObjectGroups& systemObjects);

    const kn::engraving::Staff* staff() const;
    void setStaff(const kn::engraving::Staff* staff);
    void setSystemObjects(const SystemObjectGroups& systemObjects);

    Q_INVOKABLE QString staffId() const;
    Q_INVOKABLE bool canAcceptDrop(const QVariant& item) const override;

private:
    void onScoreChanged(const kn::engraving::ScoreChanges& changes) override;

    bool addSystemObject(kn::engraving::EngravingItem* obj);
    bool removeSystemObject(kn::engraving::EngravingItem* obj);

    void updateState();

    const kn::engraving::Staff* m_staff = nullptr;
    kn::engraving::staff_idx_t m_staffIdx = kfw::nidx;
    SystemObjectGroups m_systemObjectGroups;
};
}
