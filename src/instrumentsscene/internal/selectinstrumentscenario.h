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

#include "notationscene/iselectinstrumentscenario.h"

#include "global/modularity/ioc.h"
#include "global/iinteractive.h"
#include "notation/iinstrumentsrepository.h"

#include "global/async/asyncable.h"
#include "global/async/promise.h"

namespace kn::instrumentsscene {
class SelectInstrumentsScenario : public notation::ISelectInstrumentsScenario, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<notation::IInstrumentsRepository> instrumentsRepository = { this };

public:

    SelectInstrumentsScenario(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    kfw::async::Promise<notation::PartInstrumentListScoreOrder> selectInstruments() const override;
    kfw::async::Promise<notation::InstrumentTemplate> selectInstrument(
        const notation::InstrumentKey& currentInstrumentId = notation::InstrumentKey()) const override;

private:
    kfw::async::Promise<notation::PartInstrumentListScoreOrder> selectInstruments(const kfw::ValMap& params) const;
};
}
