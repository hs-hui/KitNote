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

#include "instrumentnamesettingsmodel.h"

#include "engraving/dom/instrumentname.h"

using namespace kn::inspector;
using namespace kfw::actions;

InstrumentNameSettingsModel::InstrumentNameSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                                         IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setTitle(kfw::qtrc("inspector", "Instrument names"));
    setModelType(InspectorModelType::TYPE_INSTRUMENT_NAME);
}

void InstrumentNameSettingsModel::openStyleSettings()
{
    const kn::engraving::InstrumentName* instrumentName = selectedInstrumentName();
    IF_ASSERT_FAILED(instrumentName) {
        return;
    }

    QString subPageCode = "instrument-name-long";

    if (instrumentName->instrumentNameType() == kn::engraving::InstrumentNameType::SHORT) {
        subPageCode = "instrument-name-short";
    }

    ActionData args = ActionData::make_arg2<QString, QString>("text-styles", subPageCode);
    dispatcher()->dispatch("edit-style", args);
}

void InstrumentNameSettingsModel::openStaffAndPartProperties()
{
    dispatcher()->dispatch("staff-properties");
}

const kn::engraving::InstrumentName* InstrumentNameSettingsModel::selectedInstrumentName() const
{
    for (kn::engraving::EngravingItem* element : selection()->elements()) {
        if (element->isInstrumentName()) {
            return kn::engraving::toInstrumentName(element);
        }
    }

    return nullptr;
}
