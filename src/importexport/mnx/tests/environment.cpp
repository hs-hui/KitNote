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

#include "testing/environment.h"

#include "draw/drawmodule.h"
#include "engraving/engravingmodule.h"
#include "importexport/mnx/mnxmodule.h"

#include "engraving/tests/utils/scorerw.h"

#include "engraving/dom/instrtemplate.h"
#include "engraving/dom/engravingglobals.h"

#include "log.h"

using namespace kn;
using namespace kn::engraving;

static kfw::testing::SuiteEnvironment mnx_se(
{
    new kfw::draw::DrawModule(),
    new kn::engraving::EngravingModule(),
    new kn::iex::mnxio::MnxModule()
},
    nullptr,
    []() {
    LOGI() << "mnx tests suite post init";

    kn::engraving::ScoreRW::setRootPath(kfw::String::fromUtf8(iex_mnx_tests_DATA_ROOT));

    kn::engraving::EngravingGlobals::testMode = true;
    kn::engraving::EngravingGlobals::noGui = true;

    loadInstrumentTemplates(":/engraving/instruments/instruments.xml");
}
    );
