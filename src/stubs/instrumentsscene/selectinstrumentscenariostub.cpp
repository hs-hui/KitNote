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
#include "selectinstrumentscenariostub.h"

using namespace kn::instrumentsscene;
using namespace kn::notation;
using namespace kfw;

async::Promise<PartInstrumentListScoreOrder> SelectInstrumentsScenarioStub::selectInstruments() const
{
    return async::make_promise<PartInstrumentListScoreOrder>([](auto, auto reject) {
        Ret ret = make_ret(Ret::Code::NotSupported);
        return reject(ret.code(), ret.text());
    });
}

async::Promise<InstrumentTemplate> SelectInstrumentsScenarioStub::selectInstrument(const notation::InstrumentKey&) const
{
    return async::make_promise<InstrumentTemplate>([](auto, auto reject) {
        Ret ret = make_ret(Ret::Code::NotSupported);
        return reject(ret.code(), ret.text());
    });
}
