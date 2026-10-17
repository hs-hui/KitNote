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

#ifndef KN_NOTATION_MASTERNOTATIONPARTS_H
#define KN_NOTATION_MASTERNOTATIONPARTS_H

#include "notationparts.h"
#include "imasternotation.h"

namespace kn::notation {
class MasterNotationParts : public NotationParts
{
public:
    MasterNotationParts(IGetScore* getScore, INotationInteractionPtr interaction, INotationUndoStackPtr undoStack);

    void setExcerpts(ExcerptNotationList excerpts);

    void setParts(const PartInstrumentList& parts, const ScoreOrder& order) override;

    void removeParts(const kfw::IDList& partsIds) override;
    void removeStaves(const kfw::IDList& stavesIds) override;

    bool appendStaff(Staff* staff, const kfw::ID& destinationPartId) override;
    bool appendStaffLinkedToMaster(Staff*, Staff*, const kfw::ID&) override { return false; }
    bool appendLinkedStaff(Staff* staff, const kfw::ID& sourceStaffId, const kfw::ID& destinationPartId) override;

    void replaceInstrument(const InstrumentKey& instrumentKey, const Instrument& newInstrument,
                           const StaffType* newStaffType = nullptr) override;
    void replaceDrumset(const InstrumentKey& instrumentKey, const Drumset& newDrumset, bool undoable = true) override;

    void addSystemObjects(const kfw::IDList& stavesIds) override;
    void removeSystemObjects(const kfw::IDList& stavesIds) override;
    void moveSystemObjects(const kfw::ID& sourceStaffId, const kfw::ID& destinationStaffId) override;
    void moveSystemObjectLayerBelowBottomStaff() override;
    void moveSystemObjectLayerAboveBottomStaff() override;

private:
    void startGlobalEdit(const kfw::TranslatableString& actionName);
    void endGlobalEdit();

    void onPartsRemoved(const std::vector<Part*>& parts) override;

    std::vector<INotationPartsPtr> excerptsParts() const;
    kn::engraving::Excerpt* findExcerpt(const kfw::ID& initialPartId) const;

    ExcerptNotationList m_excerpts;
};
}

#endif // KN_NOTATION_MASTERNOTATIONPARTS_H
