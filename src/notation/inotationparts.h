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

#include "async/notification.h"

#include "notationtypes.h"

namespace kfw::async {
template<typename T>
class NotifyList;
}

namespace kn::notation {
class INotationParts
{
public:
    virtual ~INotationParts() = default;

    virtual kfw::async::NotifyList<const Part*> partList() const = 0;
    virtual kfw::async::NotifyList<const Staff*> staffList(const kfw::ID& partId) const = 0;

    virtual bool hasParts() const = 0;

    virtual const Part* part(const kfw::ID& partId) const = 0;
    virtual bool partExists(const kfw::ID& partId) const = 0;

    virtual const Staff* staff(const kfw::ID& staffId) const = 0;
    virtual bool staffExists(const kfw::ID& staffId) const = 0;

    virtual StaffConfig staffConfig(const kfw::ID& staffId) const = 0;
    virtual ScoreOrder scoreOrder() const = 0;

    virtual void setParts(const PartInstrumentList& instruments, const ScoreOrder& order) = 0;
    virtual void setScoreOrder(const ScoreOrder& order) = 0;
    virtual void setPartVisible(const kfw::ID& partId, bool visible) = 0;
    virtual bool setVoiceVisible(const kfw::ID& staffId, int voiceIndex, bool visible) = 0;
    virtual void setStaffVisible(const kfw::ID& staffId, bool visible) = 0;
    virtual void setPartSharpFlat(const kfw::ID& partId, const SharpFlat& sharpFlat) = 0;
    virtual void setInstrumentName(const InstrumentKey& instrumentKey, const QString& name) = 0;
    virtual void setInstrumentAbbreviature(const InstrumentKey& instrumentKey, const QString& abbreviature) = 0;
    virtual void setStaffType(const kfw::ID& staffId, StaffTypeId type) = 0;
    virtual void setStaffConfig(const kfw::ID& staffId, const StaffConfig& config) = 0;

    virtual void removeParts(const kfw::IDList& partsIds) = 0;
    virtual void removeStaves(const kfw::IDList& stavesIds) = 0;

    enum class InsertMode {
        Before,
        After
    };

    virtual void moveParts(const kfw::IDList& sourcePartsIds, const kfw::ID& destinationPartId, InsertMode mode = InsertMode::Before) = 0;
    virtual void moveStaves(const kfw::IDList& sourceStavesIds, const kfw::ID& destinationStaffId,
                            InsertMode mode = InsertMode::Before) = 0;

    virtual bool appendStaff(Staff* staff, const kfw::ID& destinationPartId) = 0;
    virtual bool appendStaffLinkedToMaster(Staff* staff, Staff* masterSourceStaff, const kfw::ID& destinationPartId) = 0;
    virtual bool appendLinkedStaff(Staff* staff, const kfw::ID& sourceStaffId, const kfw::ID& destinationPartId) = 0;

    virtual void insertPart(Part* part, size_t index) = 0;

    virtual void replacePart(const kfw::ID& partId, Part* newPart) = 0;
    virtual void replaceInstrument(const InstrumentKey& instrumentKey, const Instrument& newInstrument,
                                   const StaffType* newStaffType = nullptr) = 0;
    virtual void replaceDrumset(const InstrumentKey& instrumentKey, const Drumset& newDrumset, bool undoable = true) = 0;

    virtual const std::vector<Staff*>& systemObjectStaves() const = 0;
    virtual kfw::async::Notification systemObjectStavesChanged() const = 0;

    virtual void addSystemObjects(const kfw::IDList& stavesIds) = 0;
    virtual void removeSystemObjects(const kfw::IDList& stavesIds) = 0;
    virtual void moveSystemObjects(const kfw::ID& sourceStaffId, const kfw::ID& destinationStaffId) = 0;
    virtual void moveSystemObjectLayerBelowBottomStaff() = 0;
    virtual void moveSystemObjectLayerAboveBottomStaff() = 0;

    virtual kfw::async::Notification partsChanged() const = 0;
    virtual kfw::async::Notification scoreOrderChanged() const = 0;
};

using INotationPartsPtr = std::shared_ptr<INotationParts>;
}
