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

#include "engraving/editing/scoreview.h"

namespace kn::notation {
class INotationInteraction;
class ScoreCallbacks : public kn::engraving::ScoreView
{
public:
    ScoreCallbacks() = default;

    void dataChanged(const kfw::RectF&) override;
    void updateAll() override;
    void drawBackground(kfw::draw::Painter*, const kfw::RectF&) const override;
    qreal selectionProximity() const override;
    void setDropTarget(kn::engraving::EngravingItem* dropTarget) override;
    void setDropRectangle(const kfw::RectF& rect) override;
    void changeEditElement(kn::engraving::EngravingItem* newElement) override;
    void adjustCanvasPosition(const kn::engraving::EngravingItem*, int staffIdx = -1) override;

    void setSelectionProximity(qreal proximity);
    void setNotationInteraction(INotationInteraction* interaction);

private:
    qreal m_selectionProximity = 0.0f;

    INotationInteraction* m_interaction = nullptr;
};
}
