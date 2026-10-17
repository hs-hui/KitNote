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
#ifndef KN_NOTATION_NOTATIONSELECTION_H
#define KN_NOTATION_NOTATIONSELECTION_H

#include "../inotationselection.h"

#include "igetscore.h"

namespace kn::engraving {
class MeasureBase;
class Score;
class System;
}

namespace kn::notation {
class NotationSelection : public INotationSelection
{
public:
    NotationSelection(IGetScore* getScore);

    bool isNone() const override;
    bool isRange() const override;
    SelectionState state() const override;

    kfw::Ret canCopy() const override;
    kfw::ByteArray mimeData() const override;
    QMimeData* qMimeData() const override;

    EngravingItem* element() const override;
    const std::vector<EngravingItem*>& elements() const override;

    std::vector<Note*> notes(NoteFilter filter) const override;

    kfw::RectF canvasBoundingRect() const override;

    INotationSelectionRangePtr range() const override;

    EngravingItem* lastElementHit() const override;

    void onElementHit(EngravingItem*);

    kn::engraving::MeasureBase* startMeasureBase() const override;
    kn::engraving::MeasureBase* endMeasureBase() const override;
    std::vector<kn::engraving::System*> selectedSystems() const override;

    bool elementsSelected(const kn::engraving::ElementTypeSet& types) const override;

private:
    kn::engraving::Score* score() const;

    EngravingItem* m_lastElementHit = nullptr;
    IGetScore* m_getScore = nullptr;
    INotationSelectionRangePtr m_range;
};
}

#endif // KN_NOTATION_NOTATIONSELECTION_H
