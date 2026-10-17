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

#include "testing/environment.h"

#include "draw/drawmodule.h"
#include "engraving/engravingmodule.h"
#include "engraving/tests/utils/scorerw.h"

#include "importexport/guitarpro/guitarpromodule.h"

#include "engraving/dom/instrtemplate.h"
#include "engraving/dom/engravingglobals.h"

#include "log.h"

using namespace kn::iex::guitarpro;

static kfw::testing::SuiteEnvironment importexport_se(
{
    new kfw::draw::DrawModule(),         // needs for engraving
    new kn::engraving::EngravingModule(),
    new kn::iex::guitarpro::GuitarProModule()
},
    nullptr,
    []() {
    LOGI() << "guitarpro tests suite post init";

    kn::engraving::ScoreRW::setRootPath(kfw::String::fromUtf8(iex_guitarpro_tests_DATA_ROOT));

    kn::engraving::EngravingGlobals::testMode = true;
    kn::engraving::EngravingGlobals::testWriteStyleToScore = false;
    kn::engraving::EngravingGlobals::noGui = true;

    kn::engraving::loadInstrumentTemplates(":/engraving/instruments/instruments.xml");
}
    );
