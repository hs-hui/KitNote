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
#include "languagesmodule.h"

#include "modularity/ioc.h"

#include "internal/languagesconfiguration.h"
#include "internal/languagesservice.h"

#include "kfw_framework_config.h"

#ifdef KFW_MODULE_DIAGNOSTICS
#include "diagnostics/idiagnosticspathsregister.h"
#endif

using namespace kfw::languages;
using namespace kfw::modularity;

std::string LanguagesModule::moduleName() const
{
    return "languages";
}

void LanguagesModule::registerExports()
{
    m_languagesConfiguration = std::make_shared<LanguagesConfiguration>(iocContext());
    m_languagesService = std::make_shared<LanguagesService>(iocContext());

    ioc()->registerExport<ILanguagesConfiguration>(moduleName(), m_languagesConfiguration);
    ioc()->registerExport<ILanguagesService>(moduleName(), m_languagesService);
}

void LanguagesModule::onPreInit(const IApplication::RunMode&)
{
    //! NOTE: configurator must be initialized before any service that uses it
    m_languagesConfiguration->init();
    m_languagesService->init();

#ifdef KFW_MODULE_DIAGNOSTICS
    auto pr = ioc()->resolve<kfw::diagnostics::IDiagnosticsPathsRegister>(moduleName());
    if (pr) {
        pr->reg("languagesAppDataPath", m_languagesConfiguration->languagesAppDataPath());
        pr->reg("languagesUserAppDataPath", m_languagesConfiguration->languagesUserAppDataPath());
    }
#endif
}
