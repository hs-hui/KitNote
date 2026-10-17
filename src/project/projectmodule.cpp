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
#include "projectmodule.h"

#include "modularity/ioc.h"
#include "internal/projectcreator.h"
#include "internal/projectautosaver.h"
#include "internal/projectactionscontroller.h"
#include "internal/projectuiactions.h"
#include "internal/projectconfiguration.h"
#include "internal/opensaveprojectscenario.h"
#include "internal/exportprojectscenario.h"
#include "internal/mscmetareader.h"
#include "internal/templatesrepository.h"
#include "internal/projectmigrator.h"
#include "internal/projectautosaver.h"

#include "internal/notationreadersregister.h"
#include "internal/notationwritersregister.h"
#include "internal/projectrwregister.h"

#ifdef Q_OS_MAC
#include "internal/platform/macos/macosrecentfilescontroller.h"
#elif defined (Q_OS_WIN)
#include "internal/platform/windows/windowsrecentfilescontroller.h"
#else
#include "internal/recentfilescontroller.h"
#endif

#include "ui/iuiactionsregister.h"
#include "ui/iinteractiveuriregister.h"

using namespace kn::project;
using namespace kfw;
using namespace kfw::modularity;

std::string ProjectModule::moduleName() const
{
    return "project";
}

void ProjectModule::registerExports()
{
    m_configuration = std::make_shared<ProjectConfiguration>(iocContext());
    m_actionsController = std::make_shared<ProjectActionsController>(iocContext());
    m_projectAutoSaver = std::make_shared<ProjectAutoSaver>(iocContext());

#ifdef Q_OS_MAC
    m_recentFilesController = std::make_shared<MacOSRecentFilesController>();
#elif defined(Q_OS_WIN)
    m_recentFilesController = std::make_shared<WindowsRecentFilesController>();
#else
    m_recentFilesController = std::make_shared<RecentFilesController>();
#endif

    ioc()->registerExport<IProjectConfiguration>(moduleName(), m_configuration);
    ioc()->registerExport<IProjectCreator>(moduleName(), new ProjectCreator());
    ioc()->registerExport<IProjectFilesController>(moduleName(), m_actionsController);
    ioc()->registerExport<mi::IProjectProvider>(moduleName(), m_actionsController);
    ioc()->registerExport<IOpenSaveProjectScenario>(moduleName(), new OpenSaveProjectScenario(iocContext()));
    ioc()->registerExport<IExportProjectScenario>(moduleName(), new ExportProjectScenario(iocContext()));
    ioc()->registerExport<IRecentFilesController>(moduleName(), m_recentFilesController);
    ioc()->registerExport<IMscMetaReader>(moduleName(), new MscMetaReader());
    ioc()->registerExport<ITemplatesRepository>(moduleName(), new TemplatesRepository());
    ioc()->registerExport<IProjectMigrator>(moduleName(), new ProjectMigrator(iocContext()));
    ioc()->registerExport<IProjectAutoSaver>(moduleName(), m_projectAutoSaver);

    //! TODO Should be replace INotationReaders/WritersRegister with IProjectRWRegister
    ioc()->registerExport<INotationReadersRegister>(moduleName(), new NotationReadersRegister());
    ioc()->registerExport<INotationWritersRegister>(moduleName(), new NotationWritersRegister());
    ioc()->registerExport<IProjectRWRegister>(moduleName(), new ProjectRWRegister());
}

void ProjectModule::resolveImports()
{
    auto ar = ioc()->resolve<kfw::ui::IUiActionsRegister>(moduleName());
    if (ar) {
        ar->reg(std::make_shared<ProjectUiActions>(m_actionsController, iocContext()));
    }

    auto ir = ioc()->resolve<kfw::ui::IInteractiveUriRegister>(moduleName());
    if (ir) {
        ir->registerQmlUri(Uri("kitnote://project/newscore"), "KitNote.Project", "NewScoreDialog");
        ir->registerQmlUri(Uri("kitnote://project/export"), "KitNote.Project", "ExportDialog");
        ir->registerQmlUri(Uri("kitnote://project/migration"), "KitNote.Project", "MigrationDialog");
        ir->registerQmlUri(Uri("kitnote://project/properties"), "KitNote.Project", "ProjectPropertiesDialog");
    }

}

void ProjectModule::onInit(const IApplication::RunMode& mode)
{
    if (IApplication::RunMode::GuiApp != mode) {
        return;
    }

    m_configuration->init();
    m_actionsController->init();
    m_recentFilesController->init();
    m_projectAutoSaver->init();
}

void ProjectModule::onDelayedInit()
{
    //! NOTE: resuming polling can show dialogs (errors, review prompts), so it must wait
    //! until the main window is up rather than running during onInit()
    // m_convertFileToScoreService->resumeConvert();
}
