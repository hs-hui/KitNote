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

#include "masternotationparts.h"

#include "engraving/dom/masterscore.h"
#include "engraving/dom/scoreorder.h"
#include "engraving/dom/excerpt.h"
#include "engraving/dom/utils.h"
#include "engraving/editing/editexcerpt.h"
#include "engraving/editing/transpose.h"

#include "log.h"

using namespace kfw;
using namespace kn::notation;

static NotationParts* get_impl(const INotationPartsPtr& parts)
{
    return static_cast<NotationParts*>(parts.get());
}

MasterNotationParts::MasterNotationParts(IGetScore* getScore, INotationInteractionPtr interaction, INotationUndoStackPtr undoStack)
    : NotationParts(getScore, interaction, undoStack)
{
}

void MasterNotationParts::setExcerpts(ExcerptNotationList excerpts)
{
    m_excerpts = excerpts;
}

void MasterNotationParts::startGlobalEdit(const kfw::TranslatableString& actionName)
{
    NotationParts::startEdit(actionName);
    undoStack()->lock();
}

void MasterNotationParts::endGlobalEdit()
{
    undoStack()->unlock();
    NotationParts::apply();
}

void MasterNotationParts::setParts(const PartInstrumentList& partList, const ScoreOrder& order)
{
    TRACEFUNC;

    kn::engraving::KeyList keyList = score()->keyList();

    endInteractionWithScore();
    startGlobalEdit(TranslatableString("undoableAction", "Add/remove instruments"));

    doSetScoreOrder(order);
    removeMissingParts(partList);
    insertNewParts(partList, keyList);
    updateSoloist(partList);
    sortParts(partList);
    setBracketsAndBarlines();

    for (const INotationPartsPtr& excerptParts : excerptsParts()) {
        auto impl = get_impl(excerptParts);

        impl->removeMissingParts(partList);

        PartInstrumentList excerptPartList;
        for (kn::engraving::Part* part: impl->score()->parts()) {
            PartInstrument pi;
            pi.isExistingPart = true;
            pi.partId = part->id();
            excerptPartList << pi;
        }

        impl->sortParts(excerptPartList);

        impl->setBracketsAndBarlines();
    }

    updatePartsAndSystemObjectStaves();
    endGlobalEdit();
}

void MasterNotationParts::removeParts(const IDList& partsIds)
{
    TRACEFUNC;

    startGlobalEdit(TranslatableString("undoableAction", "Remove instruments"));

    NotationParts::removeParts(partsIds);

    for (const INotationPartsPtr& parts : excerptsParts()) {
        parts->removeParts(partsIds);
    }

    endGlobalEdit();
}

void MasterNotationParts::removeStaves(const IDList& stavesIds)
{
    TRACEFUNC;

    startGlobalEdit(TranslatableString("undoableAction", "Remove staves"));

    NotationParts::removeStaves(stavesIds);

    for (const INotationPartsPtr& parts : excerptsParts()) {
        parts->removeStaves(stavesIds);
    }

    endGlobalEdit();
}

bool MasterNotationParts::appendStaff(Staff* staff, const ID& destinationPartId)
{
    TRACEFUNC;

    IF_ASSERT_FAILED(staff) {
        return false;
    }

    startGlobalEdit(TranslatableString("undoableAction", "Add staff"));

    //! NOTE: will be generated later after adding to the score
    staff->setId(kn::engraving::INVALID_ID);

    NotationParts::appendStaff(staff, destinationPartId);

    for (const INotationPartsPtr& parts : excerptsParts()) {
        Staff* excerptStaff = staff->clone();
        if (!parts->appendStaffLinkedToMaster(excerptStaff, staff, destinationPartId)) {
            excerptStaff->undoUnlink();
            delete excerptStaff;
        }
    }

    endGlobalEdit();
    return true;
}

bool MasterNotationParts::appendLinkedStaff(Staff* staff, const kfw::ID& sourceStaffId, const kfw::ID& destinationPartId)
{
    TRACEFUNC;

    IF_ASSERT_FAILED(staff) {
        return false;
    }

    startGlobalEdit(TranslatableString("undoableAction", "Add linked staff"));

    //! NOTE: will be generated later after adding to the score
    staff->setId(kn::engraving::INVALID_ID);

    NotationParts::appendLinkedStaff(staff, sourceStaffId, destinationPartId);

    for (const INotationPartsPtr& parts : excerptsParts()) {
        Staff* excerptStaff = staff->clone();
        if (!parts->appendLinkedStaff(excerptStaff, sourceStaffId, destinationPartId)) {
            excerptStaff->unlink();
            delete excerptStaff;
        }
    }

    endGlobalEdit();
    return true;
}

void MasterNotationParts::replaceInstrument(const InstrumentKey& instrumentKey, const Instrument& newInstrument,
                                            const StaffType* newStaffType)
{
    TRACEFUNC;

    startGlobalEdit(TranslatableString("undoableAction", "Replace instrument"));

    Part* part = partModifiable(instrumentKey.partId);
    bool isMainInstrument = part && isMainInstrumentForPart(instrumentKey, part);

    kn::engraving::Interval oldTranspose = part ? part->instrument()->transpose() : kn::engraving::Interval(0, 0);

    NotationParts::replaceInstrument(instrumentKey, newInstrument, newStaffType);

    for (const INotationPartsPtr& parts : excerptsParts()) {
        parts->replaceInstrument(instrumentKey, newInstrument, newStaffType);
    }

    // this also transposes all linked parts
    engraving::Transpose::transpositionChanged(score(), part, Part::MAIN_INSTRUMENT_TICK, oldTranspose);

    if (isMainInstrument) {
        if (kn::engraving::Excerpt* excerpt = findExcerpt(part->id())) {
            StringList allExcerptLowerNames;
            for (const kn::engraving::Excerpt* excerpt2 : score()->masterScore()->excerpts()) {
                allExcerptLowerNames.push_back(excerpt2->name().toLower());
            }

            String newName = kn::engraving::formatUniqueExcerptName(part->partName(), allExcerptLowerNames);
            excerpt->excerptScore()->undo(new kn::engraving::ChangeExcerptTitle(excerpt, newName));
        }
    }

    endGlobalEdit();
}

void MasterNotationParts::replaceDrumset(const InstrumentKey& instrumentKey, const Drumset& newDrumset, bool undoable)
{
    TRACEFUNC;

    startGlobalEdit(TranslatableString("undoableAction", "Edit drumset"));

    NotationParts::replaceDrumset(instrumentKey, newDrumset, undoable);

    for (const INotationPartsPtr& parts : excerptsParts()) {
        parts->replaceDrumset(instrumentKey, newDrumset, undoable);
    }

    endGlobalEdit();
}

void MasterNotationParts::onPartsRemoved(const std::vector<Part*>& parts)
{
    kn::engraving::MasterScore* master = score()->masterScore();
    std::vector<kn::engraving::Excerpt*> excerpts = master->excerpts();

    for (kn::engraving::Excerpt* excerpt : excerpts) {
        const ID& initialPartId = excerpt->initialPartId();
        if (!initialPartId.isValid()) {
            continue;
        }

        bool deleteExcerpt = std::find_if(parts.cbegin(), parts.cend(), [initialPartId](const Part* part) {
            return part->id() == initialPartId;
        }) != parts.cend();

        if (deleteExcerpt) {
            master->deleteExcerpt(excerpt);
        }
    }
}

void MasterNotationParts::addSystemObjects(const kfw::IDList& stavesIds)
{
    if (stavesIds.empty()) {
        return;
    }

    startGlobalEdit(TranslatableString("undoableAction", "Add system markings"));

    NotationParts::addSystemObjects(stavesIds);

    endGlobalEdit();
}

void MasterNotationParts::removeSystemObjects(const kfw::IDList& stavesIds)
{
    if (stavesIds.empty()) {
        return;
    }

    startGlobalEdit(TranslatableString("undoableAction", "Remove system markings"));

    NotationParts::removeSystemObjects(stavesIds);

    endGlobalEdit();
}

void MasterNotationParts::moveSystemObjects(const kfw::ID& sourceStaffId, const kfw::ID& destinationStaffId)
{
    startGlobalEdit(TranslatableString("undoableAction", "Move system markings"));

    NotationParts::moveSystemObjects(sourceStaffId, destinationStaffId);

    for (const INotationPartsPtr& parts : excerptsParts()) {
        parts->moveSystemObjects(sourceStaffId, destinationStaffId);
    }

    endGlobalEdit();
}

void MasterNotationParts::moveSystemObjectLayerBelowBottomStaff()
{
    IF_ASSERT_FAILED(score()->nstaves() > 1) {
        return;
    }

    startGlobalEdit(TranslatableString("undoableAction", "Add system object layer below the bottom staff"));

    NotationParts::moveSystemObjectLayerBelowBottomStaff();

    endGlobalEdit();
}

void MasterNotationParts::moveSystemObjectLayerAboveBottomStaff()
{
    IF_ASSERT_FAILED(score()->nstaves() > 1) {
        return;
    }

    startGlobalEdit(TranslatableString("undoableAction", "Remove system object layer below the bottom staff"));

    NotationParts::moveSystemObjectLayerAboveBottomStaff();

    endGlobalEdit();
}

std::vector<INotationPartsPtr> MasterNotationParts::excerptsParts() const
{
    std::vector<INotationPartsPtr> result;

    for (const IExcerptNotationPtr& excerpt : m_excerpts) {
        result.push_back(excerpt->notation()->parts());
    }

    return result;
}

kn::engraving::Excerpt* MasterNotationParts::findExcerpt(const ID& initialPartId) const
{
    const std::vector<kn::engraving::Excerpt*>& excerpts = score()->masterScore()->excerpts();
    for (kn::engraving::Excerpt* excerpt : excerpts) {
        if (excerpt->initialPartId() == initialPartId) {
            return excerpt;
        }
    }

    return nullptr;
}
