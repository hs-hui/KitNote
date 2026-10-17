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

#include "async/asyncable.h"
#include "modularity/ioc.h"
#include "iinteractive.h"
#include "notationscene/iselectinstrumentscenario.h"

namespace kn::instrumentsscene {
class PartTreeItem : public AbstractLayoutPanelTreeItem, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT

    QML_ELEMENT;
    QML_UNCREATABLE("Must be created in C++ only")

    kfw::ContextInject<notation::ISelectInstrumentsScenario> selectInstrumentsScenario { this };
    kfw::ContextInject<kfw::IInteractive> interactive { this };

public:
    PartTreeItem(notation::IMasterNotationPtr masterNotation, notation::INotationPtr notation, QObject* parent);

    void init(const notation::Part* masterPart);

    const notation::Part* part() const;

    MoveParams buildMoveParams(int sourceRow, int count, AbstractLayoutPanelTreeItem* destinationParent, int destinationRow) const override;

    void moveChildren(int sourceRow, int count, AbstractLayoutPanelTreeItem* destinationParent, int destinationRow,
                      bool updateNotation) override;

    void moveChildrenOnScore(const MoveParams& params) override;

    void removeChildren(int row, int count, bool deleteChild) override;

    Q_INVOKABLE bool canAcceptDrop(const QVariant& item) const override;

    Q_INVOKABLE QString instrumentId() const;
    Q_INVOKABLE void replaceInstrument();
    Q_INVOKABLE void resetAllFormatting();

private:
    void onScoreChanged(const kn::engraving::ScoreChanges& changes) override;

    void listenVisibilityChanged();
    void createAndAddPart(const kfw::ID& masterPartId);

    size_t resolveNewPartIndex(const kfw::ID& partId) const;

    const notation::Part* m_part = nullptr;
    bool m_ignoreVisibilityChange = true;
    bool m_partExists = false;
};
}
