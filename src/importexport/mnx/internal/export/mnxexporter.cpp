/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2026 MuseScore Limited and others
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
#include "mnxexporter.h"

#include <stdexcept>

#include "engraving/dom/score.h"
#include "log.h"
#include "translation.h"

using namespace kn::engraving;

namespace kn::iex::mnxio {
//---------------------------------------------------------
//   getOrAssignEID
//---------------------------------------------------------

EID MnxExporter::getOrAssignEID(EngravingObject* item)
{
    EID eid = item->eid();
    if (!eid.isValid()) {
        eid = item->assignNewEID();
    }
    return eid;
}

//---------------------------------------------------------
//   mnxEventFromCR
//---------------------------------------------------------

std::optional<mnx::sequence::Event> MnxExporter::mnxEventFromCR(const engraving::ChordRest* cr)
{
    auto pointer = kfw::value(m_crToMnxEvent, cr);
    if (!pointer.empty()) {
        return mnx::sequence::Event(mnxDocument().root(), pointer);
    }
    return std::nullopt;
}

//---------------------------------------------------------
//   mnxMeasureIndexFromMeasure
//---------------------------------------------------------

size_t MnxExporter::mnxMeasureIndexFromMeasure(const engraving::Measure* measure) const
{
    IF_ASSERT_FAILED(measure) {
        throw std::logic_error("Measure is null while resolving MNX measure index.");
    }
    const size_t result = kfw::value(m_measToMnxMeas, measure, kfw::nidx);
    IF_ASSERT_FAILED(result != kfw::nidx) {
        throw std::logic_error("Measure is not mapped to an MNX measure index.");
    }
    return result;
}

//---------------------------------------------------------
//   mnxPartStaffFromStaffIdx
//---------------------------------------------------------

std::pair<size_t, int> MnxExporter::mnxPartStaffFromStaffIdx(engraving::staff_idx_t staffIdx) const
{
    const auto it = m_staffToPartStaff.find(staffIdx);
    IF_ASSERT_FAILED(it != m_staffToPartStaff.end()) {
        throw std::logic_error("Staff index is not mapped to an MNX part/staff.");
    }
    return it->second;
}

//---------------------------------------------------------
//   exportMnx
//---------------------------------------------------------

kfw::Ret MnxExporter::exportMnx()
{
    LOGI() << "MNX export started: schema version=" << m_mnxDocument.mnx().version();
    // Header
    if (m_exportBeams) {
        mnx::MnxMetaData::Support support = m_mnxDocument.mnx().ensure_support();
        support.set_useBeams(true);
    }

    createGlobal();
    if (!createParts()) {
        const String msg = kfw::TranslatableString(
            "importexport/mnx",
            "MNX export skipped because the score contains no exportable parts. (Tablature is not supported yet).").str;
        return kfw::make_ret(kfw::Ret::Code::NotSupported, msg);
    }
    createLayout(m_exportedStaves, "full-score");
    /// @todo Creation of all layouts and scores, including excerpts. (Deferred to a future dev cycle.)
    return kfw::make_ok();
}
} // namespace kn::iex::mnxio
