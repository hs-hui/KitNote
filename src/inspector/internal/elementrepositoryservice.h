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

#include "ielementrepositoryservice.h"

namespace kn::inspector {
class ElementRepositoryService : public IElementRepositoryService
{
public:
    ElementRepositoryService() = default;

    bool needUpdateElementList(const QList<kn::engraving::EngravingItem*>& newRawElementList,
                               engraving::SelState selectionState) const override;
    void updateElementList(const QList<kn::engraving::EngravingItem*>& newRawElementList, engraving::SelState selectionState) override;

    QList<kn::engraving::EngravingItem*> findElementsByType(const kn::engraving::ElementType elementType) const override;
    QList<kn::engraving::EngravingItem*> findElementsByType(const kn::engraving::ElementType elementType,
                                                            std::function<bool(const kn::engraving::EngravingItem*)> filterFunc) const
    override;
    QList<kn::engraving::EngravingItem*> takeAllElements() const override;

    kfw::async::Channel<QList<kn::engraving::EngravingItem*> > elementsUpdated() const override;

private:
    QList<kn::engraving::EngravingItem*> exposeRawElements(const QList<kn::engraving::EngravingItem*>& rawElementList) const;

    QList<kn::engraving::EngravingItem*> findChords() const;
    QList<kn::engraving::EngravingItem*> findNotes() const;
    QList<kn::engraving::EngravingItem*> findElementsForNotes() const;
    QList<kn::engraving::EngravingItem*> findNoteHeads() const;
    QList<kn::engraving::EngravingItem*> findStems() const;
    QList<kn::engraving::EngravingItem*> findHooks() const;
    QList<kn::engraving::EngravingItem*> findBeams() const;
    QList<kn::engraving::EngravingItem*> findStaffs() const;
    QList<kn::engraving::EngravingItem*> findSectionBreaks() const;
    QList<kn::engraving::EngravingItem*> findTexts() const;
    std::vector<kn::engraving::EngravingItem*> findTextDelegates(kn::engraving::EngravingItem* element) const;
    QList<kn::engraving::EngravingItem*> findBrackets() const;
    QList<kn::engraving::EngravingItem*> findLines(kn::engraving::ElementType lineType) const;
    QList<kn::engraving::EngravingItem*> findRests() const;
    QList<kn::engraving::EngravingItem*> findOrnaments() const;
    QList<kn::engraving::EngravingItem*> findLyrics() const;

    QList<kn::engraving::EngravingItem*> m_exposedElementList;
    QList<kn::engraving::EngravingItem*> m_rawElementList;
    kn::engraving::SelState m_selectionState = kn::engraving::SelState::NONE;

    kfw::async::Channel<QList<kn::engraving::EngravingItem*> > m_elementsUpdated;
};
}
