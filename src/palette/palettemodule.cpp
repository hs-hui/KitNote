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
#include "palettemodule.h"

#include <qqml.h>

#include "modularity/ioc.h"
#include "ui/iinteractiveuriregister.h"
#include "ui/iuiactionsregister.h"
#include "accessibility/iqaccessibleinterfaceregister.h"

#include "internal/paletteconfiguration.h"
#include "internal/paletteuiactions.h"
#include "internal/paletteactionscontroller.h"
#include "internal/paletteworkspacesetup.h"
#include "internal/paletteprovider.h"
#include "internal/palettecell.h"

#include "widgets/masterpalette.h"
#include "widgets/specialcharactersdialog.h"
#include "widgets/customizekitdialog.h"
#include "widgets/timesignaturepropertiesdialog.h"
#include "widgets/keyedit.h"
#include "widgets/timedialog.h"

using namespace kn::palette;
using namespace kfw;
using namespace kfw::modularity;
using namespace kfw::ui;
using namespace kfw::accessibility;

std::string PaletteModule::moduleName() const
{
    return "palette";
}

void PaletteModule::registerExports()
{
    m_paletteProvider = std::make_shared<PaletteProvider>(iocContext());
    m_actionsController = std::make_shared<PaletteActionsController>(iocContext());
    m_paletteUiActions = std::make_shared<PaletteUiActions>(m_actionsController, iocContext());
    m_configuration = std::make_shared<PaletteConfiguration>(iocContext());
    m_paletteWorkspaceSetup = std::make_shared<PaletteWorkspaceSetup>(iocContext());

    ioc()->registerExport<IPaletteProvider>(moduleName(), m_paletteProvider);
    ioc()->registerExport<IPaletteConfiguration>(moduleName(), m_configuration);
}

void PaletteModule::resolveImports()
{
    auto ar = ioc()->resolve<kfw::ui::IUiActionsRegister>(moduleName());
    if (ar) {
        ar->reg(m_paletteUiActions);
    }

    auto ir = ioc()->resolve<IInteractiveUriRegister>(moduleName());
    if (ir) {
        ir->registerWidgetUri<MasterPalette>(Uri("kitnote://palette/masterpalette"));
        ir->registerWidgetUri<SpecialCharactersDialog>(Uri("kitnote://palette/specialcharacters"));
        ir->registerWidgetUri<TimeSignaturePropertiesDialog>(Uri("kitnote://palette/timesignatureproperties"));
        ir->registerWidgetUri<CustomizeKitDialog>(Uri("kitnote://palette/customizekit"));
        ir->registerWidgetUri<KeyEditor>(Uri("kitnote://notation/keysignatures"));
        ir->registerWidgetUri<TimeDialog>(Uri("kitnote://notation/timesignatures"));

        ir->registerQmlUri(Uri("kitnote://palette/properties"), "KitNote.Palette", "PalettePropertiesDialog");
        ir->registerQmlUri(Uri("kitnote://palette/cellproperties"), "KitNote.Palette", "PaletteCellPropertiesDialog");
    }

    auto accr = ioc()->resolve<IQAccessibleInterfaceRegister>(moduleName());
    if (accr) {
        accr->registerInterfaceGetter("kn::palette::PaletteWidget", PaletteWidget::accessibleInterface);
        accr->registerInterfaceGetter("kn::palette::PaletteCell", PaletteCell::accessibleInterface);
    }
}

void PaletteModule::onInit(const IApplication::RunMode&)
{
    m_configuration->init();
    m_actionsController->init();
    m_paletteUiActions->init();
    m_paletteProvider->init();
}

void PaletteModule::onAllInited(const IApplication::RunMode&)
{
    //! NOTE We need to be sure that the workspaces are initialized.
    //! So, we loads these settings on onAllInited
    m_paletteWorkspaceSetup->setup();
}

void PaletteModule::onDeinit()
{
    m_paletteWorkspaceSetup.reset();
    m_configuration.reset();
    m_paletteUiActions.reset();

    ioc()->unregisterIfRegistered<IPaletteProvider>(moduleName(), m_paletteProvider);
    m_paletteProvider.reset();
}
