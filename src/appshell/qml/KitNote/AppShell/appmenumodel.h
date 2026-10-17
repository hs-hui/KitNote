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
#pragma once

#include <qqmlintegration.h>

#include "uicomponents/qml/KitNote/UiComponents/abstractmenumodel.h"

#include "kfw_framework_config.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"
#include "context/iglobalcontext.h"
#include "global/iglobalconfiguration.h"
#include "project/iprojectconfiguration.h"
#include "project/irecentfilescontroller.h"
#include "ui/imainwindow.h"
#include "ui/inavigationcontroller.h"
#include "ui/iuiactionsregister.h"
#include "ui/iuiconfiguration.h"
#include "framework/stubs/update/iupdateconfiguration.h"
#include "workspace/iworkspacemanager.h"

#include "iappshellconfiguration.h"
#include "internal/iappmenumodelhook.h"

namespace kn::appshell {
class AppMenuModel : public kfw::uicomponents::AbstractMenuModel
{
    Q_OBJECT

#ifdef Q_OS_WIN
    QML_ANONYMOUS;
#else
    QML_NAMED_ELEMENT(PlatformAppMenuModel);
#endif

public:
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::GlobalInject<kfw::update::IUpdateConfiguration> updateConfiguration;
    kfw::GlobalInject<IAppShellConfiguration> configuration;
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::GlobalInject<project::IProjectConfiguration> projectConfiguration;
    kfw::ContextInject<kfw::ui::IUiActionsRegister> uiActionsRegister = { this };
    kfw::ContextInject<IAppMenuModelHook> appMenuModelHook = { this };
    kfw::ContextInject<kn::context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> actionsDispatcher = { this };
    kfw::ContextInject<kfw::ui::IMainWindow> mainWindow = { this };
    kfw::ContextInject<kfw::ui::INavigationController> navigationController = { this };
    kfw::ContextInject<kfw::workspace::IWorkspaceManager> workspacesManager = { this };
    kfw::ContextInject<project::IRecentFilesController> recentFilesController = { this };

public:
    explicit AppMenuModel(QObject* parent = nullptr);

    Q_INVOKABLE void load() override;
    Q_INVOKABLE bool isGlobalMenuAvailable();

private:
    void setupConnections();

    void onActionsStateChanges(const kfw::actions::ActionCodeList& codes) override;

    using kfw::uicomponents::AbstractMenuModel::makeMenuItem;
    kfw::uicomponents::MenuItem* makeMenuItem(const kfw::actions::ActionCode& actionCode, kfw::uicomponents::MenuItemRole role);

    kfw::uicomponents::MenuItem* makeFileMenu();
    kfw::uicomponents::MenuItemList makeSaveOtherSubItems();

    kfw::uicomponents::MenuItem* makeEditMenu();
    kfw::uicomponents::MenuItem* makeViewMenu();
    kfw::uicomponents::MenuItem* makeAddMenu();
    kfw::uicomponents::MenuItem* makeFormatMenu();
    kfw::uicomponents::MenuItem* makeToolsMenu();
    kfw::uicomponents::MenuItem* makeHelpMenu(bool addDiagnosticsSubMenu);
    kfw::uicomponents::MenuItem* makeDiagnosticsMenu();

    kfw::uicomponents::MenuItemList makeRecentScoresItems();
    kfw::uicomponents::MenuItemList appendClearRecentSection(const kfw::uicomponents::MenuItemList& recentScores);

    kfw::uicomponents::MenuItemList makeNotesItems();
    kfw::uicomponents::MenuItemList makeTupletsItems();
    kfw::uicomponents::MenuItemList makeMeasuresItems();
    kfw::uicomponents::MenuItemList makeDrumPatternItems();
    kfw::uicomponents::MenuItemList makeDrumRudimentItems();
    kfw::uicomponents::MenuItemList makeFramesItems();
    kfw::uicomponents::MenuItemList makeFramesAppendItems();
    kfw::uicomponents::MenuItemList makeTextItems();
    kfw::uicomponents::MenuItemList makeLinesItems();
    kfw::uicomponents::MenuItemList makeToolbarsItems();
    kfw::uicomponents::MenuItemList makeWorkspacesItems();
    kfw::uicomponents::MenuItemList makeShowItems();

    kn::notation::INotationUndoStackPtr undoStack() const;
    void updateUndoRedoItems();

    std::shared_ptr<kfw::uicomponents::AbstractMenuModel> m_workspacesMenuModel;
};
}
