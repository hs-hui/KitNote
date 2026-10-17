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

#include "async/asyncable.h"
#include "async/notifylist.h"

#include "inotationparts.h"
#include "inotationundostack.h"
#include "inotationinteraction.h"

namespace kn::notation {
class IGetScore;
class NotationParts : public INotationParts, public kfw::async::Asyncable
{
public:
    NotationParts(IGetScore* getScore, INotationInteractionPtr interaction, INotationUndoStackPtr undoStack);

    kfw::async::NotifyList<const Part*> partList() const override;
    kfw::async::NotifyList<const Staff*> staffList(const kfw::ID& partId) const override;

    bool hasParts() const override;

    const Part* part(const kfw::ID& partId) const override;
    bool partExists(const kfw::ID& partId) const override;

    const Staff* staff(const kfw::ID& staffId) const override;
    bool staffExists(const kfw::ID& staffId) const override;

    StaffConfig staffConfig(const kfw::ID& staffId) const override;
    ScoreOrder scoreOrder() const override;

    void setParts(const PartInstrumentList& parts, const ScoreOrder& order) override;
    void setScoreOrder(const ScoreOrder& order) override;
    void setPartVisible(const kfw::ID& partId, bool visible) override;
    bool setVoiceVisible(const kfw::ID& staffId, int voiceIndex, bool visible) override;
    void setStaffVisible(const kfw::ID& staffId, bool visible) override;
    void setPartSharpFlat(const kfw::ID& partId, const SharpFlat& sharpFlat) override;
    void setInstrumentName(const InstrumentKey& instrumentKey, const QString& name) override;
    void setInstrumentAbbreviature(const InstrumentKey& instrumentKey, const QString& abbreviature) override;
    void setStaffType(const kfw::ID& staffId, StaffTypeId type) override;
    void setStaffConfig(const kfw::ID& staffId, const StaffConfig& config) override;

    void removeParts(const kfw::IDList& partsIds) override;
    void removeStaves(const kfw::IDList& stavesIds) override;

    void moveParts(const kfw::IDList& sourcePartsIds, const kfw::ID& destinationPartId, InsertMode mode = InsertMode::Before) override;
    void moveStaves(const kfw::IDList& sourceStavesIds, const kfw::ID& destinationStaffId, InsertMode mode = InsertMode::Before) override;

    bool appendStaff(Staff* staff, const kfw::ID& destinationPartId) override;
    bool appendStaffLinkedToMaster(Staff* staff, Staff* masterSourceStaff, const kfw::ID& destinationPartId) override;
    bool appendLinkedStaff(Staff* staff, const kfw::ID& sourceStaffId, const kfw::ID& destinationPartId) override;

    void insertPart(Part* part, size_t index) override;

    void replacePart(const kfw::ID& partId, Part* newPart) override;
    void replaceInstrument(const InstrumentKey& instrumentKey, const Instrument& newInstrument,
                           const StaffType* newStaffType = nullptr) override;
    void replaceDrumset(const InstrumentKey& instrumentKey, const Drumset& newDrumset, bool undoable = true) override;

    const std::vector<Staff*>& systemObjectStaves() const override;
    kfw::async::Notification systemObjectStavesChanged() const override;

    void addSystemObjects(const kfw::IDList& stavesIds) override;
    void removeSystemObjects(const kfw::IDList& stavesIds) override;
    void moveSystemObjects(const kfw::ID& sourceStaffId, const kfw::ID& destinationStaffId) override;
    void moveSystemObjectLayerBelowBottomStaff() override;
    void moveSystemObjectLayerAboveBottomStaff() override;

    kfw::async::Notification partsChanged() const override;
    kfw::async::Notification scoreOrderChanged() const override;

protected:
    kn::engraving::Score* score() const;
    INotationUndoStackPtr undoStack() const;

    Part* partModifiable(const kfw::ID& partId) const;

    void startEdit(const kfw::TranslatableString& actionName);
    void apply();
    void rollback();

    virtual void onPartsRemoved(const std::vector<Part*>& parts);

private:
    friend class MasterNotationParts;

    void listenUndoStackChanges();
    void updatePartsAndSystemObjectStaves(const kn::engraving::ScoreChanges& changes = {});

    void doSetScoreOrder(const ScoreOrder& order);
    void doRemoveParts(const std::vector<Part*>& parts);
    void doAppendStaff(Staff* staff, Part* destinationPart, bool createRests=true);
    void doSetStaffConfig(Staff* staff, const StaffConfig& config);
    void doInsertPart(Part* part, size_t index);

    Staff* staffModifiable(const kfw::ID& staffId) const;

    std::vector<Staff*> staves(const kfw::IDList& stavesIds) const;
    std::vector<Part*> parts(const kfw::IDList& partsIds) const;

    void appendStaves(Part* part, const InstrumentTemplate& templ, const kn::engraving::KeyList& keyList);
    void insertStaff(Staff* staff, engraving::staff_idx_t destinationStaffIndex, bool createRests=true);
    void initStaff(Staff* staff, const InstrumentTemplate& templ, const kn::engraving::StaffType* staffType, size_t cleffIndex);

    void removeMissingParts(const PartInstrumentList& newParts);
    void insertNewParts(const PartInstrumentList& parts, const kn::engraving::KeyList& keyList);
    void insertNewParts(const PartInstrumentList& parts);
    void updateSoloist(const PartInstrumentList& parts);
    void sortParts(const PartInstrumentList& parts);

    int resolveNewInstrumentNumber(const InstrumentTemplate& instrument, const PartInstrumentList& allNewInstruments) const;

    void setBracketsAndBarlines();

    void endInteractionWithScore();

    void notifyAboutPartChanged(const Part* part) const;
    void notifyAboutPartAdded(const Part* part) const;
    void notifyAboutPartRemoved(const Part* part) const;
    void notifyAboutPartReplaced(const Part* oldPart, const Part* newPart) const;
    void notifyAboutStaffChanged(const Staff* staff) const;
    void notifyAboutStaffAdded(const Staff* staff) const;
    void notifyAboutStaffRemoved(const Staff* staff) const;

    IGetScore* m_getScore = nullptr;
    INotationUndoStackPtr m_undoStack;
    INotationInteractionPtr m_interaction;
    kfw::async::Notification m_partsChanged;
    kfw::async::Notification m_scoreOrderChanged;

    std::vector<Part*> m_parts;
    std::vector<Staff*> m_systemObjectStaves;
    kfw::async::Notification m_systemObjectStavesChanged;

    bool m_ignoreUndoStackChanges = false;

    mutable kfw::async::ChangedNotifier<const Part*> m_partChangedNotifier;
    mutable std::map<kfw::ID, kfw::async::ChangedNotifier<const Staff*> > m_staffChangedNotifierMap;
};
}
