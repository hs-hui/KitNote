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

#include "iprojectfilescontroller.h"

#include <QObject>
#include <QString>

#include "modularity/ioc.h"
#include "iinteractive.h"
#include "context/iglobalcontext.h"
#include "actions/actionable.h"
#include "actions/iactionsdispatcher.h"
#include "multiwindows/imultiwindowsprovider.h"
#include "multiwindows/iprojectprovider.h"
#include "playback/iplaybackcontroller.h"
#include "print/iprintprovider.h"
#include "iexportprojectscenario.h"
#include "inotationreadersregister.h"
#include "iopensaveprojectscenario.h"
#include "imscmetareader.h"
#include "io/ifilesystem.h"
#include "notation/inotationconfiguration.h"

#include "async/asyncable.h"

#include "iprojectconfiguration.h"
#include "iprojectcreator.h"
#include "irecentfilescontroller.h"
#include "iprojectautosaver.h"

namespace kn::project {
class ProjectActionsController : public IProjectFilesController, public kfw::mi::IProjectProvider, public kfw::Contextable,
    public kfw::actions::Actionable, public kfw::async::Asyncable
{
    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::GlobalInject<kfw::mi::IMultiWindowsProvider> multiwindowsProvider;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;
    kfw::GlobalInject<IProjectCreator> projectCreator;
    kfw::ContextInject<INotationReadersRegister> readers = { this };
    kfw::ContextInject<IRecentFilesController> recentFilesController = { this };
    kfw::ContextInject<IProjectAutoSaver> projectAutoSaver = { this };
    kfw::ContextInject<IOpenSaveProjectScenario> openSaveProjectScenario = { this };
    kfw::ContextInject<IExportProjectScenario> exportProjectScenario = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<playback::IPlaybackController> playbackController = { this };
    kfw::ContextInject<print::IPrintProvider> printProvider = { this };

public:

    ProjectActionsController(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();

    bool canReceiveAction(const kfw::actions::ActionCode& code) const override;

    bool isUrlSupported(const QUrl& url) const override;
    bool isFileSupported(const kfw::io::path_t& path) const override;
    kfw::Ret openProject(const ProjectFile& file) override;
    bool closeOpenedProject(bool goToHome = true) override;
    bool saveProject(const kfw::io::path_t& path = kfw::io::path_t()) override;
    bool saveProjectLocally(
        const kfw::io::path_t& path = kfw::io::path_t(), SaveMode saveMode = SaveMode::Save, bool createBackup = true) override;

    // mi::IProjectProvider
    bool isProjectOpened(const kfw::io::path_t& scorePath) const override;
    bool isAnyProjectOpened() const override;

private:
    void setupConnections();

    project::INotationProjectPtr currentNotationProject() const;
    notation::IMasterNotationPtr currentMasterNotation() const;
    notation::INotationPtr currentNotation() const;
    notation::INotationInteractionPtr currentInteraction() const;
    notation::INotationSelectionPtr currentNotationSelection() const;

    void newProject();
    void newProjectFromTemplate(const kfw::actions::ActionData& args);

    void openProject(const kfw::actions::ActionData& args);
    kfw::Ret openProject(const kfw::io::path_t& path, const QString& displayNameOverride = QString());

    bool shouldRetryLoadAfterError(const kfw::Ret& ret, const kfw::io::path_t& filepath);
    bool askIfUserAgreesToOpenProjectWithIncompatibleVersion(const std::string& errorText);
    void warnFileTooNew(const kfw::io::path_t& filepath);
    bool askIfUserAgreesToOpenCorruptedProject(const kfw::String& projectName, const std::string& errorText);
    void warnProjectCriticallyCorrupted(const kfw::String& projectName, const std::string& errorText);
    void warnProjectCannotBeOpened(const kfw::Ret& ret, const kfw::io::path_t& filepath);

    kfw::IInteractive::Button askAboutSavingScore(INotationProjectPtr project);

    kfw::Ret canSaveProject() const;
    bool saveProject(SaveMode saveMode, bool force = false);
    void saveProjectAt(const kfw::actions::ActionData& args);
    bool saveProjectAt(const SaveLocation& saveLocation, SaveMode saveMode = SaveMode::Save, bool force = false);

    bool askIfUserAgreesToSaveProjectWithErrors(const kfw::Ret& ret, const SaveLocation& location);
    void warnScoreWithoutPartsCannotBeSaved();
    bool askIfUserAgreesToSaveCorruptedScore(const SaveLocation& location, const std::string& errorText, bool newlyCreated);
    bool askIfUserAgreesToSaveCorruptedScoreLocally(const std::string& errorText, bool canRevert);
    bool askIfUserAgreesToSaveCorruptedScoreUponOpenning(const SaveLocation& location, const std::string& errorText);

    void warnScoreCouldnotBeSaved(const kfw::Ret& ret);
    void warnScoreCouldnotBeSaved(const std::string& errorText);
    int warnScoreHasBecomeCorruptedAfterSave(const kfw::Ret& ret);

    void revertCorruptedScoreToLastSaved();

    RecentFile makeRecentFile(INotationProjectPtr project);

    void clearRecentScores();

    void continueLastSession();

    void openProjectProperties();

    kfw::async::Promise<kfw::io::path_t> selectScoreOpeningFile() const;
    kfw::io::path_t selectScoreSavingFile(const kfw::io::path_t& defaultFilePath, const QString& saveTitle);

    kfw::RetVal<INotationProjectPtr> loadProject(const kfw::io::path_t& filePath);
    kfw::Ret loadWithFallback(const std::shared_ptr<INotationProject>& project, const kfw::io::path_t& loadPath,
                               const std::string& format);
    kfw::Ret doOpenProject(const kfw::io::path_t& filePath);

    kfw::Ret doFinishOpenProject();
    kfw::Ret openPageIfNeed(kfw::Uri pageUri);

    void exportScore();
    void printScore();

    bool hasSelection() const;

    bool m_isProjectSaving = false;
    bool m_isProjectClosing = false;
    bool m_isProjectProcessing = false;

};
}
