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
#ifndef KN_NOTATION_ISELECTINSTRUMENTSSCENARIO_H
#define KN_NOTATION_ISELECTINSTRUMENTSSCENARIO_H

#include "modularity/imoduleinterface.h"
#include "notation/notationtypes.h"
#include "global/async/promise.h"

namespace kn::notation {
class ISelectInstrumentsScenario : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(ISelectInstrumentsScenario)

public:
    virtual ~ISelectInstrumentsScenario() = default;

    virtual kfw::async::Promise<PartInstrumentListScoreOrder> selectInstruments() const = 0;
    virtual kfw::async::Promise<InstrumentTemplate> selectInstrument(
        const InstrumentKey& currentInstrumentKey = InstrumentKey()) const = 0;
};
}

#endif // KN_NOTATION_ISELECTINSTRUMENTSSCENARIO_H
