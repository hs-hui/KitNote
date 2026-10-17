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

#include "instrumentsscenemodule.h"

#include "modularity/ioc.h"

#include "internal/selectinstrumentscenario.h"
#include "internal/instrumentsuiactions.h"
#include "internal/instrumentsactionscontroller.h"

#include "ui/iinteractiveuriregister.h"
#include "ui/iuiactionsregister.h"

using namespace kn::instrumentsscene;
using namespace kfw;
using namespace kfw::modularity;
using namespace kfw::ui;

std::string InstrumentsSceneModule::moduleName() const
{
    return "instrumentsscene";
}

void InstrumentsSceneModule::registerExports()
{
    m_actionsController = std::make_shared<InstrumentsActionsController>(iocContext());

    ioc()->registerExport<notation::ISelectInstrumentsScenario>(moduleName(), new SelectInstrumentsScenario(iocContext()));
}

void InstrumentsSceneModule::resolveImports()
{
    auto ar = ioc()->resolve<IUiActionsRegister>(moduleName());
    if (ar) {
        ar->reg(std::make_shared<InstrumentsUiActions>(iocContext()));
    }

    auto ir = ioc()->resolve<IInteractiveUriRegister>(moduleName());
    if (ir) {
        ir->registerQmlUri(Uri("kitnote://instruments/select"), "KitNote.InstrumentsScene", "InstrumentsDialog");
    }
}

void InstrumentsSceneModule::onInit(const IApplication::RunMode&)
{
    m_actionsController->init();
}
