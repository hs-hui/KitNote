/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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

#include "braillemodule.h"

#include "modularity/ioc.h"
#include "project/inotationwritersregister.h"

#include "internal/brailleconfiguration.h"
#include "internal/brailleconverter.h"
#include "internal/notationbraille.h"
#include "internal/braillewriter.h"

#include "inotationbraille.h"

using namespace kfw;
using namespace kn::braille;
using namespace kn::engraving;
using namespace kn::notation;
using namespace kn::project;

std::string BrailleModule::moduleName() const
{
    return "braille";
}

void BrailleModule::resolveImports()
{
    auto writers = ioc()->resolve<INotationWritersRegister>(moduleName());
    if (writers) {
        writers->reg({ "brf" }, std::make_shared<BrailleWriter>());
    }
}

void BrailleModule::registerExports()
{
    m_brailleConfiguration = std::make_shared<BrailleConfiguration>();
    m_brailleConverter = std::make_shared<BrailleConverter>();
    m_notationBraille = std::make_shared<NotationBraille>(iocContext());

    ioc()->registerExport<IBrailleConfiguration>(moduleName(), m_brailleConfiguration);
    ioc()->registerExport<IBrailleConverter>(moduleName(), m_brailleConverter);
    ioc()->registerExport<INotationBraille>(moduleName(), m_notationBraille);
}

void BrailleModule::onInit(const IApplication::RunMode&)
{
    m_brailleConfiguration->init();
    m_notationBraille->init();
}
