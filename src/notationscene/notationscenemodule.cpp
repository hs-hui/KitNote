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
#include "notationscenemodule.h"

#include "modularity/ioc.h"
#include "ui/iinteractiveuriregister.h"
#include "ui/iuiactionsregister.h"

#include "internal/notationsceneconfiguration.h"
#include "internal/notationactioncontroller.h"
#include "internal/midiinputoutputcontroller.h"
#include "internal/notationuiactions.h"
#include "internal/notationactionsshortcutsmigrator.h"

#include "widgets/breaksdialog.h"
#include "widgets/editstaff.h"
#include "widgets/editstringdata.h"
#include "widgets/editstyle.h"
#include "widgets/measureproperties.h"
#include "widgets/pagesettings.h"
#include "widgets/realizeharmonydialog.h"
#include "widgets/selectdialog.h"
#include "widgets/selectnotedialog.h"
#include "widgets/stafftextpropertiesdialog.h"
#include "widgets/transposedialog.h"
#include "widgets/tupletdialog.h"

using namespace kn::notation;
using namespace kfw;
using namespace kfw::modularity;
using namespace kfw::ui;
using namespace kfw::actions;
using namespace kfw::uicomponents;

std::string NotationSceneModule::moduleName() const
{
    return "notationscene";
}

void NotationSceneModule::registerExports()
{
    m_configuration = std::make_shared<NotationSceneConfiguration>(iocContext());
    m_actionController = std::make_shared<NotationActionController>(iocContext());
    m_notationUiActions = std::make_shared<NotationUiActions>(m_actionController, iocContext());
    m_midiInputOutputController = std::make_shared<MidiInputOutputController>(iocContext());

    ioc()->registerExport<INotationSceneConfiguration>(moduleName(), m_configuration);
}

void NotationSceneModule::resolveImports()
{
    auto ar = ioc()->resolve<IUiActionsRegister>(moduleName());
    if (ar) {
        ar->reg(m_notationUiActions);
    }

    auto ir = ioc()->resolve<IInteractiveUriRegister>(moduleName());
    if (ir) {
        ir->registerWidgetUri<EditStyle>(Uri("kitnote://notation/style"));
        ir->registerWidgetUri<PageSettings>(Uri("kitnote://notation/pagesettings"));
        ir->registerWidgetUri<MeasurePropertiesDialog>(Uri("kitnote://notation/measureproperties"));
        ir->registerWidgetUri<BreaksDialog>(Uri("kitnote://notation/breaks"));
        ir->registerWidgetUri<EditStaff>(Uri("kitnote://notation/staffproperties"));
        ir->registerWidgetUri<EditStringData>(Uri("kitnote://notation/editstrings"));
        ir->registerWidgetUri<TransposeDialog>(Uri("kitnote://notation/transpose"));
        ir->registerWidgetUri<SelectNoteDialog>(Uri("kitnote://notation/selectnote"));
        ir->registerWidgetUri<SelectDialog>(Uri("kitnote://notation/selectelement"));
        ir->registerWidgetUri<TupletDialog>(Uri("kitnote://notation/othertupletdialog"));
        ir->registerWidgetUri<StaffTextPropertiesDialog>(Uri("kitnote://notation/stafftextproperties"));
        ir->registerWidgetUri<RealizeHarmonyDialog>(Uri("kitnote://notation/realizechordsymbols"));

        ir->registerQmlUri(Uri("kitnote://notation/parts"), "KitNote.NotationScene", "PartsDialog");
        ir->registerQmlUri(Uri("kitnote://notation/selectmeasurescount"), "KitNote.NotationScene", "SelectMeasuresCountDialog");
        ir->registerQmlUri(Uri("kitnote://notation/editgridsize"), "KitNote.NotationScene", "EditGridSizeDialog");
        ir->registerQmlUri(Uri("kitnote://notation/percussionpanelpadswap"), "KitNote.NotationScene", "PercussionPanelPadSwapDialog");
        ir->registerQmlUri(Uri("kitnote://notation/editpercussionshortcut"), "KitNote.NotationScene", "EditPercussionShortcutDialog");
    }
}

void NotationSceneModule::onInit(const IApplication::RunMode& mode)
{
    m_configuration->init();
    m_actionController->init();
    m_notationUiActions->init();

    if (mode == IApplication::RunMode::GuiApp) {
        m_midiInputOutputController->init();
    }
}

void NotationSceneModule::onAllInited(const IApplication::RunMode& mode)
{
    if (mode == IApplication::RunMode::GuiApp) {
        NotationActionsShortcutsMigrator::migrate();
    }
}
