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

#include "engraving/dom/instrtemplate.h"
#include "engraving/dom/engravingglobals.h"

#include "mocks/engravingconfigurationmock.h"

#include "utils/scorerw.h"

#include "log.h"

static const kn::engraving::IEngravingConfiguration::DebuggingOptions debugOpt {};

static kfw::testing::SuiteEnvironment engraving_se(
{
    new kfw::draw::DrawModule(),
    new kn::engraving::EngravingModule()
},
    nullptr,
    []() {
    LOGI() << "engraving tests suite post init";

    kn::engraving::ScoreRW::setRootPath(kfw::String::fromUtf8(engraving_tests_DATA_ROOT));

    kn::engraving::EngravingGlobals::testMode = true;
    kn::engraving::EngravingGlobals::noGui = true;

    kn::engraving::loadInstrumentTemplates(":/engraving/instruments/instruments.xml");

    using ECMock = ::testing::NiceMock<kn::engraving::EngravingConfigurationMock>;

    std::shared_ptr<ECMock> configurator(new ECMock(), [](ECMock*) {}); // no delete
    ON_CALL(*configurator, isAccessibleEnabled()).WillByDefault(::testing::Return(false));
    ON_CALL(*configurator, defaultColor()).WillByDefault(::testing::Return(kfw::draw::Color::BLACK));
    ON_CALL(*configurator, debuggingOptions()).WillByDefault(::testing::ReturnRef(debugOpt));
    ON_CALL(*configurator, allowReadingImagesFromOutsideMscz()).WillByDefault(::testing::Return(true));

    kfw::modularity::globalIoc()->unregister<kn::engraving::IEngravingConfiguration>("utests");
    kfw::modularity::globalIoc()->registerExport<kn::engraving::IEngravingConfiguration>("utests", configurator);
},

    []() {
    std::shared_ptr<kn::engraving::IEngravingConfiguration> mock
        = kfw::modularity::globalIoc()->resolve<kn::engraving::IEngravingConfiguration>("utests");
    kfw::modularity::globalIoc()->unregister<kn::engraving::IEngravingConfiguration>("utests");

    //! HACK
    //! There are still live pointers to the mock
    //! because of this, the mock generates an error stating that it has not been deleted
    //! This is a hack to remove it manually to get rid of this error
    //! The problem itself is deeper, it seems that some score objects are not deleted
    kn::engraving::IEngravingConfiguration* ecptr = mock.get();
    delete ecptr;
}
    );
