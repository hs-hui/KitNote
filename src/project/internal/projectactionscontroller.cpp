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
#include "projectactionscontroller.h"

#include <QBuffer>
#include <QEventLoop>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QUrl>
#include <QUrlQuery>

#include "async/async.h"
#include "defer.h"
#include "translation.h"

#include "engraving/infrastructure/mscio.h"
#include "engraving/engravingerrors.h"

#include "projecterrors.h"

#include "log.h"

using namespace kn;
using namespace kn::project;
using namespace kn::notation;
using namespace kfw;
using namespace kfw::actions;

static const kfw::Uri NOTATION_PAGE_URI("kitnote://notation");
static const kfw::Uri HOME_PAGE_URI("kitnote://home");
static const kfw::Uri NEW_SCORE_URI("kitnote://project/newscore");
static const kfw::Uri PROJECT_PROPERTIES_URI("kitnote://project/properties");

static constexpr int RETRY_SAVE_BTN_ID = int(IInteractive::Button::CustomButton);
static constexpr int SAVE_AS_BTN_ID    = RETRY_SAVE_BTN_ID + 1;

void ProjectActionsController::init()
{
    dispatcher()->reg(this, "file-new", this, &ProjectActionsController::newProject);
    dispatcher()->reg(this, "file-new-from-template", this, &ProjectActionsController::newProjectFromTemplate);
    dispatcher()->reg(this, "file-open", this, &ProjectActionsController::openProject);

    dispatcher()->reg(this, "file-close", [this]() {
        auto anyInstanceWithoutProject = multiwindowsProvider()->isHasWindowWithoutProject();
        bool ok = closeOpenedProject();
        if (ok && anyInstanceWithoutProject) {
            //! NOTE: we need to call `quit` in the next event loop due to controlling the lifecycle of this method
            async::Async::call(this, [this]() {
                dispatcher()->dispatch("quit", ActionData::make_arg1<bool>(false));
            });
            multiwindowsProvider()->activateWindowWithoutProject();
        }
    });

    dispatcher()->reg(this, "file-save", [this]() { saveProject(SaveMode::Save); });
    dispatcher()->reg(this, "file-save-as", [this]() { saveProject(SaveMode::SaveAs); });
    dispatcher()->reg(this, "file-save-a-copy", [this]() { saveProject(SaveMode::SaveCopy); });
    dispatcher()->reg(this, "file-save-selection", [this]() { saveProject(SaveMode::SaveSelection); });
    dispatcher()->reg(this, "file-save-at", [this](const ActionData& args) { saveProjectAt(args); });

    dispatcher()->reg(this, "file-export", this, &ProjectActionsController::exportScore);

    dispatcher()->reg(this, "print", this, &ProjectActionsController::printScore);

    dispatcher()->reg(this, "clear-recent", this, &ProjectActionsController::clearRecentScores);

    dispatcher()->reg(this, "continue-last-session", this, &ProjectActionsController::continueLastSession);

    dispatcher()->reg(this, "project-properties", this, &ProjectActionsController::openProjectProperties);
}

INotationProjectPtr ProjectActionsController::currentNotationProject() const
{
    return globalContext()->currentProject();
}

IMasterNotationPtr ProjectActionsController::currentMasterNotation() const
{
    return currentNotationProject() ? currentNotationProject()->masterNotation() : nullptr;
}

INotationPtr ProjectActionsController::currentNotation() const
{
    return currentMasterNotation() ? currentMasterNotation()->notation() : nullptr;
}

INotationInteractionPtr ProjectActionsController::currentInteraction() const
{
    return currentNotation() ? currentNotation()->interaction() : nullptr;
}

INotationSelectionPtr ProjectActionsController::currentNotationSelection() const
{
    return currentNotation() ? currentInteraction()->selection() : nullptr;
}

bool ProjectActionsController::canReceiveAction(const ActionCode& code) const
{
    if (!currentNotationProject()) {
        static const std::unordered_set<ActionCode> DONT_REQUIRE_OPEN_PROJECT {
            "file-new",
            "file-new-from-template", // KitNote
            "file-open",
            "continue-last-session",
            "clear-recent",
        };

        return kfw::contains(DONT_REQUIRE_OPEN_PROJECT, code);
    }

    return true;
}

bool ProjectActionsController::isUrlSupported(const QUrl& url) const
{
    if (url.isLocalFile()) {
        return isFileSupported(kfw::io::path_t(url));
    }

    return false;
}

bool ProjectActionsController::isFileSupported(const kfw::io::path_t& path) const
{
    std::string suffix = io::suffix(path);
    if (engraving::isKitNoteFile(suffix)) {
        return true;
    }

    if (readers()->reader(suffix)) {
        return true;
    }

    return false;
}

void ProjectActionsController::openProject(const ActionData& args)
{
    QUrl url = !args.empty() ? args.arg<QUrl>(0) : QUrl();
    QString displayNameOverride = args.count() >= 2 ? args.arg<QString>(1) : QString();

    Ret ret = openProject(ProjectFile(url, displayNameOverride));
    if (!ret) {
        LOGE() << ret.toString();

        // KitNote: a file that is not opened (one with pitched instruments, most often) must not
        // leave the window on an empty score page, as happens when the file was given at start-up.
        // (Not while another score is still on its way in: that one opens its own page.)
        if (!globalContext()->currentProject() && !m_isProjectProcessing) {
            openPageIfNeed(HOME_PAGE_URI);
        }
    }
}

Ret ProjectActionsController::openProject(const ProjectFile& file)
{
    LOGI() << "Try open project: url = " << file.url.toString() << ", displayNameOverride = " << file.displayNameOverride;

    if (file.isNull()) {
        auto promise = selectScoreOpeningFile();
        promise.onResolve(this, [this](const io::path_t& askedPath) {
            if (askedPath.empty()) {
                return;
            }

            configuration()->setLastOpenedProjectsPath(io::dirpath(askedPath));

            openProject(askedPath);
        });

        return kfw::make_ok();
    }

    if (file.url.isLocalFile()) {
        return openProject(file.path(), file.displayNameOverride);
    }

    return make_ret(Err::UnsupportedUrl);
}

Ret ProjectActionsController::openProject(const kfw::io::path_t& givenPath, const QString& displayNameOverride)
{
    //! NOTE This method is synchronous,
    //! but inside `multiwindowsProvider` there can be an event loop
    //! to wait for the responses from other instances, accordingly,
    //! the events (like user click) can be executed and this method can be called several times,
    //! before the end of the current call.
    //! So we ignore all subsequent calls until the current one completes.
    if (m_isProjectProcessing) {
        return make_ret(Ret::Code::InternalError);
    }
    m_isProjectProcessing = true;

    DEFER {
        m_isProjectProcessing = false;
    };

    //! Step 1. Take absolute path
    kfw::io::path_t actualPath = fileSystem()->absoluteFilePath(givenPath);
    if (actualPath.empty()) {
        // We assume that a valid path has been specified to this method
        return make_ret(Ret::Code::UnknownError);
    }

    //! Step 2. If the project is already open in the current window, then just switch to showing the notation
    if (isProjectOpened(actualPath)) {
        return doFinishOpenProject();
    }

    //! Step 3. Check, if the project already opened in another window, then activate the window with the project
    if (multiwindowsProvider()->isProjectAlreadyOpened(actualPath)) {
        multiwindowsProvider()->activateWindowWithProject(actualPath);
        return make_ret(Ret::Code::Ok);
    }

    //! Step 4. Check, if a any project is already open in the current window,
    //! then create a new instance
    if (globalContext()->currentProject()) {
        QStringList args;
        args << actualPath.toQString();

        if (!displayNameOverride.isEmpty()) {
            args << "--score-display-name-override" << displayNameOverride;
        }

        multiwindowsProvider()->openNewWindow(args);
        return make_ret(Ret::Code::Ok);
    }

    //! Step 5. Open project in the current window
    return doOpenProject(actualPath);
}

RetVal<INotationProjectPtr> ProjectActionsController::loadProject(const kfw::io::path_t& filePath)
{
    TRACEFUNC;

    const auto project = projectCreator()->newProject(iocContext());
    IF_ASSERT_FAILED(project) {
        return make_ret(Ret::Code::InternalError);
    }

    const bool hasUnsavedChanges = projectAutoSaver()->projectHasUnsavedChanges(filePath);

    const kfw::io::path_t loadPath = hasUnsavedChanges ? projectAutoSaver()->projectAutoSavePath(filePath) : filePath;
    const std::string format = io::suffix(filePath);

    if (Ret result = loadWithFallback(project, loadPath, format); !result) {
        return result;
    }

    if (hasUnsavedChanges) {
        //! NOTE: redirect the project to the original file path
        project->setPath(filePath);

        project->markAsUnsaved();
    }

    if (projectAutoSaver()->isAutosaveOfNewlyCreatedProject(filePath)) {
        project->markAsNewlyCreated();
    }

    return RetVal<INotationProjectPtr>::make_ok(project);
}

Ret ProjectActionsController::loadWithFallback(const std::shared_ptr<INotationProject>& project,
                                               const kfw::io::path_t& loadPath,
                                               const std::string& format)
{
    Ret result = project->load(loadPath, OpenParams(), format);

    if (result || result.code() == static_cast<int>(Ret::Code::Cancel)) {
        return result;
    }

    bool forceLoad = shouldRetryLoadAfterError(result, loadPath);
    if (forceLoad) {
        OpenParams params;
        params.forceMode = forceLoad;
        result = project->load(loadPath, params, format);
    }

    return result;
}

Ret ProjectActionsController::doOpenProject(const kfw::io::path_t& filePath)
{
    TRACEFUNC;

    RetVal<INotationProjectPtr> rv = loadProject(filePath);
    if (!rv.ret) {
        return rv.ret;
    }

    INotationProjectPtr project = rv.val;

    bool isNewlyCreated = projectAutoSaver()->isAutosaveOfNewlyCreatedProject(filePath);
    if (!isNewlyCreated) {
        recentFilesController()->prependRecentFile(makeRecentFile(project));
    }

    globalContext()->setCurrentProject(project);

    return doFinishOpenProject();
}

Ret ProjectActionsController::doFinishOpenProject()
{

    return openPageIfNeed(NOTATION_PAGE_URI);
}

Ret ProjectActionsController::openPageIfNeed(Uri pageUri)
{
    if (!interactive()->isOpened(pageUri).val) {
        interactive()->open(pageUri);
    }
    return make_ret(Ret::Code::Ok);
}

bool ProjectActionsController::isProjectOpened(const kfw::io::path_t& scorePath) const
{
    auto project = globalContext()->currentProject();
    if (!project) {
        return false;
    }

    LOGD() << "project->path: " << project->path() << ", check path: " << scorePath;
    if (project->path() == scorePath) {
        return true;
    }

    return false;
}

bool ProjectActionsController::isAnyProjectOpened() const
{
    auto project = globalContext()->currentProject();
    if (project) {
        return true;
    }
    return false;
}

void ProjectActionsController::newProject()
{
    //! NOTE This method is synchronous,
    //! but inside `multiwindowsProvider` there can be an event loop
    //! to wait for the responses from other instances, accordingly,
    //! the events (like user click) can be executed and this method can be called several times,
    //! before the end of the current call.
    //! So we ignore all subsequent calls until the current one completes.
    if (m_isProjectProcessing) {
        return;
    }
    m_isProjectProcessing = true;

    DEFER {
        m_isProjectProcessing = false;
    };

    if (globalContext()->currentProject()) {
        if (multiwindowsProvider()->isHasWindowWithoutProject()) {
            multiwindowsProvider()->activateWindowWithoutProject({ "file-new" });
            return;
        }
        QStringList args;
        args << "--session-type" << "start-with-new";
        multiwindowsProvider()->openNewWindow(args);
        return;
    }

    auto promise = interactive()->open(NEW_SCORE_URI);
    promise.onResolve(this, [this](const Val&) {
        Ret ret = doFinishOpenProject();

        if (!ret) {
            LOGE() << ret.toString();
        }
    });
}

// KitNote: a new score straight from a template, without the "New score" dialog.
// It gets what the dialog offers by default: 4/4 and 32 measures.
void ProjectActionsController::newProjectFromTemplate(const ActionData& args)
{
    const QString templatePath = args.count() > 0 ? args.arg<QString>(0) : QString();

    if (templatePath.isEmpty()) {
        newProject();
        return;
    }

    if (m_isProjectProcessing) {
        return;
    }

    // A score is open in this window: the new one gets a window of its own, as with "New score".
    if (globalContext()->currentProject()) {
        QStringList args;
        args << "--session-type" << (QStringLiteral("start-with-template:") + templatePath);
        multiwindowsProvider()->openNewWindow(args);
        return;
    }
    m_isProjectProcessing = true;

    DEFER {
        m_isProjectProcessing = false;
    };

    ProjectCreateOptions options;
    options.templatePath = templatePath;
    // the placeholders the dialog fills in when its fields are left empty
    options.title = kfw::qtrc("project", "Untitled score");
    options.composer = kfw::qtrc("project", "Composer / arranger");
    options.scoreOptions.globalTimesig = kn::engraving::Fraction(4, 4);
    options.scoreOptions.totalMeasures = 32;

    INotationProjectPtr project = projectCreator()->newProject(iocContext());
    Ret ret = project->createNew(options);
    if (!ret) {
        LOGE() << ret.toString();
        interactive()->error(kfw::trc("project", "The score could not be created"), ret.text());
        return;
    }

    globalContext()->setCurrentProject(project);

    ret = doFinishOpenProject();
    if (!ret) {
        LOGE() << ret.toString();
    }
}

bool ProjectActionsController::closeOpenedProject(bool goToHome)
{
    if (m_isProjectClosing) {
        return false;
    }

    if (m_isProjectSaving) {
        return false;
    }

    m_isProjectClosing = true;
    DEFER {
        m_isProjectClosing = false;
    };

    INotationProjectPtr project = currentNotationProject();
    if (!project) {
        return true;
    }

    if (playbackController()->isPlaying()) {
        playbackController()->reset();
    }

    bool result = true;

    if (project->needSave().val) {
        IInteractive::Button btn = askAboutSavingScore(project);

        if (btn == IInteractive::Button::Cancel) {
            result = false;
        } else if (btn == IInteractive::Button::Save) {
            result = saveProject();
        } else if (btn == IInteractive::Button::DontSave) {
            result = true;
        }
    }

    if (result) {
        interactive()->closeAllDialogsSync();
        globalContext()->setCurrentProject(nullptr);

        if (goToHome) {
            Ret ret = openPageIfNeed(HOME_PAGE_URI);
            if (!ret) {
                LOGE() << ret.toString();
            }
        }
    }

    return result;
}

IInteractive::Button ProjectActionsController::askAboutSavingScore(INotationProjectPtr project)
{
    std::string title = kfw::qtrc("project", "Do you want to save changes to the score “%1” before closing?")
                        .arg(project->displayName()).toStdString();

    std::string body = kfw::trc("project", "Your changes will be lost if you don’t save them.");

    IInteractive::Result result = interactive()->warningSync(title, body, {
        IInteractive::Button::DontSave,
        IInteractive::Button::Cancel,
        IInteractive::Button::Save
    }, IInteractive::Button::Save);

    return result.standardButton();
}

Ret ProjectActionsController::canSaveProject() const
{
    auto project = currentNotationProject();
    if (!project) {
        LOGW() << "no current project";
        return make_ret(Err::NoProjectError);
    }

    return project->canSave();
}

bool ProjectActionsController::saveProject(const kfw::io::path_t& path)
{
    if (!path.empty()) {
        if (m_isProjectSaving) {
            return false;
        }

        m_isProjectSaving = true;
        DEFER {
            m_isProjectSaving = false;
        };

        return saveProjectAt(SaveLocation(SaveLocationType::Local, path));
    }

    return saveProject(SaveMode::Save);
}

bool ProjectActionsController::saveProject(SaveMode saveMode, bool force)
{
    if (m_isProjectSaving) {
        return false;
    }

    m_isProjectSaving = true;
    DEFER {
        m_isProjectSaving = false;
    };

    INotationProjectPtr project = currentNotationProject();

    const bool isExistingSave = saveMode == SaveMode::Save && !project->isNewlyCreated();
    if (isExistingSave) {
        // Under these conditions, we can save without asking...
        return saveProjectAt(SaveLocation(SaveLocationType::Local), saveMode, force);
    }

    RetVal<SaveLocation> response = openSaveProjectScenario()->askSaveLocation(project, saveMode);
    if (!response.ret) {
        LOGE() << response.ret.toString();
        return false;
    }

    return saveProjectAt(response.val, saveMode, force);
}

void ProjectActionsController::saveProjectAt(const kfw::actions::ActionData& args)
{
    io::path_t path = !args.empty() ? args.arg<io::path_t>(0) : io::path_t();
    if (!path.empty()) {
        saveProjectAt(SaveLocation(path));
    }
}

bool ProjectActionsController::saveProjectAt(const SaveLocation& location, SaveMode saveMode, bool force)
{
    INotationInteractionPtr interaction = currentInteraction();
    if (interaction && interaction->isTextEditingStarted()) {
        interaction->endEditText();
    }

    if (!force) {
        Ret ret = canSaveProject();
        if (!ret) {
            ret = askIfUserAgreesToSaveProjectWithErrors(ret, location);
            if (!ret) {
                return ret;
            }
        }
    }

    if (location.isLocal()) {
        return saveProjectLocally(location.localPath(), saveMode);
    }

    return false;
}

bool ProjectActionsController::saveProjectLocally(const kfw::io::path_t& filePath, SaveMode saveMode, bool createBackup)
{
    INotationProjectPtr project = currentNotationProject();
    if (!project) {
        return false;
    }

    Ret ret = make_ok();

    if (ret) {
        ret = project->save(filePath, saveMode, createBackup);
    }

    if (!ret) {
        LOGE() << ret.toString();
        if (ret.code() != (int)Err::CorruptionUponSavingError) {
            warnScoreCouldnotBeSaved(ret);
        } else {
            switch (warnScoreHasBecomeCorruptedAfterSave(ret)) {
            case RETRY_SAVE_BTN_ID:
                async::Async::call(this, [this, filePath, saveMode]() {
                    // Retry the save. Do not create a backup this time because the target file has been corrupted
                    // already. Creating a backup file of a corrupted file now makes no sense and will corrupt
                    // the healthy backup file created on the first save attempt.
                    saveProjectLocally(filePath, saveMode, false /*createBackup*/);
                });
                break;

            case SAVE_AS_BTN_ID:
                async::Async::call(this, [this]() {
                    saveProject(SaveMode::SaveAs);
                });
                break;
            }
        }
        return false;
    }

    recentFilesController()->prependRecentFile(makeRecentFile(project));
    return true;
}

bool ProjectActionsController::askIfUserAgreesToSaveProjectWithErrors(const Ret& ret, const SaveLocation& location)
{
    switch (static_cast<Err>(ret.code())) {
    case Err::NoPartsError:
        warnScoreCouldnotBeSaved(kfw::trc("project/save", "Please add at least one instrument to enable saving."));
        return false;
    case Err::CorruptionUponOpenningError:
        return askIfUserAgreesToSaveCorruptedScoreUponOpenning(location, ret.text());
    case Err::CorruptionError: {
        auto project = currentNotationProject();
        return askIfUserAgreesToSaveCorruptedScore(location, ret.text(), project->isNewlyCreated());
    }
    default:
        return false;
    }
}

bool ProjectActionsController::askIfUserAgreesToSaveCorruptedScore(const SaveLocation& location, const std::string& errorText,
                                                                   bool newlyCreated)
{
    switch (location.type) {
    case SaveLocationType::Local:
        return askIfUserAgreesToSaveCorruptedScoreLocally(errorText, !newlyCreated);
    case SaveLocationType::Undefined: // fallthrough
    default:
        return false;
    }
}

bool ProjectActionsController::askIfUserAgreesToSaveCorruptedScoreLocally(const std::string& errorText,
                                                                          bool canRevert)
{
    std::string title = kfw::trc("project", "This score has become corrupted and contains errors");

    IInteractive::Text text;
    text.text = !canRevert
                ? kfw::trc("project", "You can continue saving it locally, although the file may become unusable. "
                                       "You can try to fix the errors manually.")
                : kfw::trc("project", "You can continue saving it locally, although the file may become unusable. "
                                       "To preserve your score, revert to the last saved version, or fix the errors manually.");
    text.detailedText = errorText;

    IInteractive::ButtonDatas buttons;
    buttons.push_back(interactive()->buttonData(IInteractive::Button::Cancel));

    IInteractive::ButtonData saveAnywayBtn(IInteractive::Button::CustomButton, kfw::trc("project", "Save anyway"), !canRevert /*accent*/);
    buttons.push_back(saveAnywayBtn);

    int defaultBtn = saveAnywayBtn.btn;

    IInteractive::ButtonData revertToLastSavedBtn(saveAnywayBtn.btn + 1, kfw::trc("project", "Revert to last saved"),
                                                  true /*accent*/);
    if (canRevert) {
        buttons.push_back(revertToLastSavedBtn);
        defaultBtn = revertToLastSavedBtn.btn;
    }

    int btn = interactive()->errorSync(title, text, buttons, defaultBtn).button();

    if (btn == revertToLastSavedBtn.btn) {
        revertCorruptedScoreToLastSaved();
    }

    return btn == saveAnywayBtn.btn;
}

bool ProjectActionsController::askIfUserAgreesToSaveCorruptedScoreUponOpenning(const SaveLocation& location, const std::string& errorText)
{
    switch (location.type) {
    case SaveLocationType::Local:
        return askIfUserAgreesToSaveCorruptedScoreLocally(errorText, false /*canRevert*/);
    case SaveLocationType::Undefined: // fallthrough
    default:
        return false;
    }
}

void ProjectActionsController::warnScoreCouldnotBeSaved(const Ret& ret)
{
    std::string message = ret.text();
    if (message.empty()) {
        message = kfw::trc("project/save", "An unknown error occurred while saving this file.");
    }

    warnScoreCouldnotBeSaved(message);
}

void ProjectActionsController::warnScoreCouldnotBeSaved(const std::string& errorText)
{
    interactive()->warning(kfw::trc("project/save", "Your score could not be saved"), errorText);
}

int ProjectActionsController::warnScoreHasBecomeCorruptedAfterSave(const Ret& ret)
{
    const QString errDetailsMessage = QString::fromStdString(ret.toString()).toHtmlEscaped();

    const std::string title = kfw::trc("project/save", "An error occurred while saving your score");

    const std::string body = kfw::qtrc("project/save",
                                        "To preserve your score, try saving it again. "
                                        "If this message still appears, please save your score as new copy.<br/><br/>"
                                        "Error details (please cite when asking for support): %1")
                             .arg(errDetailsMessage)
                             .toStdString();

    IInteractive::ButtonDatas buttons;

    IInteractive::ButtonData saveAsBtn(SAVE_AS_BTN_ID, kfw::trc("project/save", "Save as…"));
    saveAsBtn.role = IInteractive::ButtonRole::ContinueRole;
    buttons.push_back(saveAsBtn);

    IInteractive::ButtonData retryBtn(RETRY_SAVE_BTN_ID, kfw::trc("project", "Try again"), true /*accent*/);
    retryBtn.role = IInteractive::ButtonRole::ContinueRole;
    buttons.push_back(retryBtn);

    IInteractive::ButtonData cancelBtn = interactive()->buttonData(IInteractive::Button::Cancel);
    buttons.push_back(cancelBtn);

    return interactive()->errorSync(title, IInteractive::Text(body, IInteractive::TextFormat::RichText),
                                    buttons, retryBtn.btn).button();
}

void ProjectActionsController::revertCorruptedScoreToLastSaved()
{
    TRACEFUNC;

    std::string title = kfw::trc("project", "Revert to last saved?");
    std::string body = kfw::trc("project", "Your changes will be lost. This action cannot be undone.");

    auto promise = interactive()->warning(title, body, {
        { IInteractive::Button::No, IInteractive::Button::Yes }
    }, IInteractive::Button::Yes, IInteractive::Option::WithIcon);

    promise.onResolve(this, [this](const IInteractive::Result& res) {
        if (res.isButton(IInteractive::Button::No)) {
            return;
        }

        auto currentProject = currentNotationProject();
        kfw::io::path_t filePath = currentProject->path();

        bool hasUnsavedChanges = projectAutoSaver()->projectHasUnsavedChanges(filePath);
        if (hasUnsavedChanges) {
            kfw::io::path_t autoSavePath = projectAutoSaver()->projectAutoSavePath(filePath);
            fileSystem()->remove(autoSavePath);
        }

        Ret ret = doOpenProject(filePath);
        if (!ret) {
            LOGE() << ret.toString();
        }
    });
}

RecentFile ProjectActionsController::makeRecentFile(INotationProjectPtr project)
{
    RecentFile file;
    file.path = project->path();

    return file;
}

bool ProjectActionsController::shouldRetryLoadAfterError(const Ret& ret, const kfw::io::path_t& filepath)
{
    if (ret) {
        return true;
    }

    switch (static_cast<engraving::Err>(ret.code())) {
    case engraving::Err::FileTooOld:
    case engraving::Err::FileOld300Format:
        return askIfUserAgreesToOpenProjectWithIncompatibleVersion(ret.text());
    case engraving::Err::FileTooNew:
        warnFileTooNew(filepath);
        return configuration()->disableVersionChecking();
    case engraving::Err::FileCorrupted:
        return askIfUserAgreesToOpenCorruptedProject(io::filename(filepath).toString(), ret.text());
    case engraving::Err::FileCriticallyCorrupted:
        warnProjectCriticallyCorrupted(io::filename(filepath).toString(), ret.text());
        return false;
    default:
        warnProjectCannotBeOpened(ret, filepath);
        break;
    }

    return false;
}

bool ProjectActionsController::askIfUserAgreesToOpenProjectWithIncompatibleVersion(const std::string& errorText)
{
    IInteractive::ButtonData openAnywayBtn(IInteractive::Button::CustomButton, kfw::trc("project", "Open anyway"), true /*accent*/);

    int btn = interactive()->warningSync(errorText, "", {
        interactive()->buttonData(IInteractive::Button::Cancel),
        openAnywayBtn
    }, openAnywayBtn.btn).button();

    return btn == openAnywayBtn.btn;
}

void ProjectActionsController::warnFileTooNew(const kfw::io::path_t& filepath)
{
    interactive()->error(kfw::qtrc("project", "Cannot read file %1").arg(io::toNativeSeparators(filepath).toQString()).toStdString(),
                         // KitNote: see engravingerrors.h
                         kfw::mtrc("project",
                                    "This file was saved with a newer version of MuseScore than the one KitNote is built on (%1). KitNote cannot open it.")
                         .arg(kfw::String::fromUtf8(KITNOTE_ENGINE_VERSION)).toStdString());
}

bool ProjectActionsController::askIfUserAgreesToOpenCorruptedProject(const String& projectName, const std::string& errorText)
{
    std::string title = kfw::mtrc("project", "File “%1” is corrupted").arg(projectName).toStdString();
    IInteractive::Text text;
    text.text = kfw::trc("project", "This file contains errors that could cause KitNote to malfunction.");
    text.detailedText = errorText;

    IInteractive::ButtonData openAnywayBtn(IInteractive::Button::CustomButton, kfw::trc("project", "Open anyway"), true /*accent*/);

    int btn = interactive()->warningSync(title, text, {
        interactive()->buttonData(IInteractive::Button::Cancel),
        openAnywayBtn
    }, openAnywayBtn.btn).button();

    return btn == openAnywayBtn.btn;
}

void ProjectActionsController::warnProjectCriticallyCorrupted(const String& projectName, const std::string& errorText)
{
    std::string title = kfw::mtrc("project", "File “%1” is corrupted and cannot be opened").arg(projectName).toStdString();
    interactive()->error(title, errorText);
}

void ProjectActionsController::warnProjectCannotBeOpened(const Ret& ret, const kfw::io::path_t& filepath)
{
    std::string title = kfw::mtrc("project", "Cannot read file %1").arg(io::toNativeSeparators(filepath).toString()).toStdString();
    std::string body;

    switch (ret.code()) {
    case int(engraving::Err::FileNotFound):
        body = kfw::trc("project", "This file does not exist or cannot be accessed at the moment.");
        break;
    case int(engraving::Err::FileOpenError):
        body = kfw::trc("project",
                         "This file could not be opened. Please make sure that KitNote has permission to read this file.");
        break;
    default:
        if (!ret.text().empty()) {
            body = ret.text();
        } else {
            body = kfw::trc("project", "An error occurred while reading this file.");
        }
    }

    interactive()->error(title, body);
}

void ProjectActionsController::clearRecentScores()
{
    recentFilesController()->clearRecentFiles();
}

void ProjectActionsController::continueLastSession()
{
    const RecentFilesList& recentScorePaths = recentFilesController()->recentFilesList();

    if (recentScorePaths.empty()) {
        Ret ret = openPageIfNeed(HOME_PAGE_URI);
        if (!ret) {
            LOGE() << ret.toString();
        }
        return;
    }

    kfw::io::path_t lastScorePath = recentScorePaths.front().path;
    openProject(lastScorePath);
}

void ProjectActionsController::exportScore()
{
    static const Uri EXPORT_URI("kitnote://project/export");
    if (!interactive()->isOpened(EXPORT_URI).val) {
        interactive()->open(EXPORT_URI);
    }
}

void ProjectActionsController::printScore()
{
    INotationPtr notation = globalContext()->currentNotation();
    if (!notation) {
        return;
    }

    printProvider()->printNotation(notation);
}

async::Promise<io::path_t> ProjectActionsController::selectScoreOpeningFile() const
{
    // KitNote: only the formats that are still built in.
    std::string allExt = "*.kitnote *.mscz *.mxl *.musicxml *.xml *.mid *.midi *.kar *.mscx *.mscs *.kitnote~ *.mscz~";

    std::vector<std::string> filter { kfw::trc("project", "All supported files") + " (" + allExt + ")",
                                      kfw::trc("project", "KitNote files") + " (*.kitnote)",
                                      kfw::trc("project", "MuseScore files") + " (*.mscz)",
                                      kfw::trc("project", "MusicXML files") + " (*.mxl *.musicxml *.xml)",
                                      kfw::trc("project", "MIDI files") + " (*.mid *.midi *.kar)",
                                      kfw::trc("project", "Uncompressed MuseScore folders (experimental)") + " (*.mscx)",
                                      kfw::trc("project", "MuseScore developer files") + " (*.mscs)",
                                      kfw::trc("project", "KitNote backup files") + " (*.kitnote~ *.mscz~)" };

    return interactive()->selectOpeningFile(kfw::trc("project", "Open"), configuration()->defaultOpenProjectsPath(), filter);
}

bool ProjectActionsController::hasSelection() const
{
    return currentNotationSelection() ? !currentNotationSelection()->isNone() : false;
}

void ProjectActionsController::openProjectProperties()
{
    interactive()->open(PROJECT_PROPERTIES_URI);
}
