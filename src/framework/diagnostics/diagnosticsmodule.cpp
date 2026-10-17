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
#include "diagnosticsmodule.h"

#include "modularity/ioc.h"
#include "ui/iinteractiveuriregister.h"
#include "ui/iuiactionsregister.h"

#include "internal/diagnosticsconfiguration.h"
#include "internal/diagnosticsactions.h"
#include "internal/diagnosticsactionscontroller.h"
#include "internal/diagnosticspathsregister.h"
#include "internal/savediagnosticfilesscenario.h"

#include "log.h"

using namespace kfw::diagnostics;
using namespace kfw;
using namespace kfw::modularity;

std::string DiagnosticsModule::moduleName() const
{
    return "diagnostics";
}

void DiagnosticsModule::registerExports()
{
    m_configuration = std::make_shared<DiagnosticsConfiguration>(iocContext());
    m_actionsController = std::make_shared<DiagnosticsActionsController>(iocContext());

    ioc()->registerExport<IDiagnosticsPathsRegister>(moduleName(), new DiagnosticsPathsRegister());
    ioc()->registerExport<IDiagnosticsConfiguration>(moduleName(), m_configuration);
    ioc()->registerExport<ISaveDiagnosticFilesScenario>(moduleName(), new SaveDiagnosticFilesScenario(iocContext()));
}

void DiagnosticsModule::resolveImports()
{
    auto ir = ioc()->resolve<kfw::ui::IInteractiveUriRegister>(moduleName());
    if (ir) {
        ir->registerQmlUri(Uri("kitnote://diagnostics/system/paths"), "KitNote.Diagnostics", "DiagnosticPathsDialog");
        ir->registerQmlUri(Uri("kitnote://diagnostics/system/graphicsinfo"), "KitNote.Diagnostics", "DiagnosticGraphicsInfoDialog");
        ir->registerQmlUri(Uri("kitnote://diagnostics/system/profiler"), "KitNote.Diagnostics", "DiagnosticProfilerDialog");
        ir->registerQmlUri(Uri("kitnote://diagnostics/navigation/tree"), "KitNote.Diagnostics", "DiagnosticNavigationDialog");
        ir->registerQmlUri(Uri("kitnote://diagnostics/accessible/tree"), "KitNote.Diagnostics", "DiagnosticAccessibleDialog");
        ir->registerQmlUri(Uri("kitnote://diagnostics/actions/list"), "KitNote.Diagnostics", "DiagnosticActionsDialog");
    }

    auto ar = ioc()->resolve<kfw::ui::IUiActionsRegister>(moduleName());
    if (ar) {
        ar->reg(std::make_shared<DiagnosticsActions>());
    }
}

void DiagnosticsModule::onInit(const IApplication::RunMode&)
{
    m_configuration->init();
    m_actionsController->init();

}
