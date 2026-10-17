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
#ifndef KN_APPSHELL_APPLICATIONCONTROLLER_H
#define KN_APPSHELL_APPLICATIONCONTROLLER_H

#include <QObject>

#include "modularity/ioc.h"
#include "actions/actionable.h"
#include "actions/iactionsdispatcher.h"
#include "ui/iuiactionsregister.h"
#include "async/asyncable.h"
#include "ui/imainwindow.h"
#include "languages/ilanguagesservice.h"
#include "iinteractive.h"
#include "iappshellconfiguration.h"
#include "multiwindows/imultiwindowsprovider.h"
#include "project/iprojectfilescontroller.h"
#include "audio/main/isoundfontcontroller.h"
#include "istartupscenario.h"
#include "iapplication.h"
#include "context/iglobalcontext.h"
#include "context/iuicontextresolver.h"
#include "global/iglobalconfiguration.h"

class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;

namespace kn::appshell {
class ApplicationActionController : public QObject, public kfw::Contextable, public kfw::actions::Actionable,
    public kfw::async::Asyncable
{
    kfw::GlobalInject<kfw::mi::IMultiWindowsProvider> multiwindowsProvider;
    kfw::GlobalInject<IAppShellConfiguration> configuration;
    kfw::GlobalInject<kfw::languages::ILanguagesService> languagesService;
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::ContextInject<kfw::ui::IUiActionsRegister> actionsRegister = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::ui::IMainWindow> mainWindow = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<project::IProjectFilesController> projectFilesController = { this };
    kfw::ContextInject<kfw::audio::ISoundFontController> soundFontController = { this };
    kfw::ContextInject<IStartupScenario> startupScenario = { this };
    kfw::ContextInject<kfw::IApplication> application = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<context::IUiContextResolver> uiContextResolver = { this };

public:
    ApplicationActionController(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void preInit();
    void init();

    kfw::ValCh<bool> isFullScreen() const;

private:

    enum DragTarget {
        Unknown = 0,
        ProjectFile,
        SoundFont,
        Recording       // KitNote: a recording, for a drum score made from it
    };

    bool eventFilter(QObject* watched, QEvent* event) override;

    QWindow* qWindow() const;

    DragTarget dragTarget(const QUrl& url) const;
    bool onDragEnterEvent(QDragEnterEvent* event);
    bool onDragMoveEvent(QDragMoveEvent* event);
    bool onDropEvent(QDropEvent* event);

    void setupConnections();

    bool quit(bool isAllInstances, const kfw::io::path_t& installerPath = kfw::io::path_t());
    void restart();

    void toggleFullScreen();
    void openAboutDialog();
    void openAboutQtDialog();
    void openAboutMusicXMLDialog();

    void openKitNoteGuide();

    void startKitNoteUiScript();
    void runKitNoteUiScriptStep();

    QStringList m_uiScript;     // KitNote: see startKitNoteUiScript
    int m_uiScriptStep = 0;
    void openPreferencesDialog();
    void doOpenPreferencesDialog();

    void revertToFactorySettings();

    bool hasProjectAndIsFocused() const;
    void doGlobalCopy();
    void doGlobalCut();
    void doGlobalPaste();
    void doGlobalUndo();
    void doGlobalRedo();
    void doGlobalDelete();
    void doGlobalCancel();

    bool m_quiting = false;

    kfw::async::Channel<kfw::actions::ActionCodeList> m_actionsReceiveAvailableChanged;
};
}

#endif // KN_APPSHELL_APPLICATIONCONTROLLER_H
