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

#include "inotationelements.h"
#include "igetscore.h"

namespace kn::notation {
class NotationElements : public INotationElements
{
public:
    NotationElements(IGetScore* getScore);

    kn::engraving::Score* msScore() const override;

    std::vector<EngravingItem*> search(const QString& searchText) const override;
    std::vector<EngravingItem*> elements(const FilterElementsOptions& elementsOptions) const override;

    Measure* measure(const int measureIndex) const override;
    const PageList& pages() const override;
    const Page* pageByPoint(const kfw::PointF& point) const override;

private:
    kn::engraving::Score* score() const;

    kn::engraving::RehearsalMark* rehearsalMark(const kfw::String& name) const;
    kn::engraving::Page* page(const int pageIndex) const;

    std::vector<EngravingItem*> allScoreElements() const;

    std::vector<EngravingItem*> filterElements(const FilterElementsOptions* elementsOptions) const;
    std::vector<EngravingItem*> filterNotes(const FilterNotesOptions* notesOptions) const;

    ElementPattern constructElementPattern(const FilterElementsOptions* elementsOptions) const;
    kn::engraving::NotePattern constructNotePattern(const FilterNotesOptions* notesOptions) const;

    IGetScore* m_getScore = nullptr;
};
}
