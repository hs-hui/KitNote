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
#include "applicationactioncontroller.h"

#include <QFile>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QQuickWindow>
#include <QTimer>
#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QFileInfo>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QFileOpenEvent>
#include <QWindow>
#include <QMimeData>
#include <QTimer>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>

#include "settings.h"
#include "project/inotationproject.h"
#include "async/async.h"
#include "audio/common/soundfonttypes.h"

#include "defer.h"
#include "translation.h"
#include "log.h"

using namespace kn::appshell;
using namespace kfw;
using namespace kfw::actions;

void ApplicationActionController::preInit()
{
    qApp->installEventFilter(this);
}

void ApplicationActionController::init()
{
    dispatcher()->reg(this, "quit", [this](const ActionData& args) {
        bool isAllInstances = args.count() > 0 ? args.arg<bool>(0) : true;
        kfw::io::path_t installatorPath = args.count() > 1 ? args.arg<kfw::io::path_t>(1) : "";

        QTimer::singleShot(0, [this, isAllInstances, installatorPath]() {
            quit(isAllInstances, installatorPath);
        });
    });

    dispatcher()->reg(this, "restart", [this]() {
        restart();
    });

    dispatcher()->reg(this, "fullscreen", this, &ApplicationActionController::toggleFullScreen);

    dispatcher()->reg(this, "about-musescore", this, &ApplicationActionController::openAboutDialog);
    dispatcher()->reg(this, "about-qt", this, &ApplicationActionController::openAboutQtDialog);
    dispatcher()->reg(this, "about-musicxml", this, &ApplicationActionController::openAboutMusicXMLDialog);
    dispatcher()->reg(this, "kitnote-guide", this, &ApplicationActionController::openKitNoteGuide);
    dispatcher()->reg(this, "kitnote-audio-to-score", [this]() {
        interactive()->open("kitnote://audio-to-score");     // KitNote: src/preferences/.../AudioToScoreDialog.qml
    });

    startKitNoteUiScript();     // KitNote: only with KITNOTE_TEST_UI set
    dispatcher()->reg(this, "preference-dialog", this, &ApplicationActionController::openPreferencesDialog);

    dispatcher()->reg(this, "revert-factory", this, &ApplicationActionController::revertToFactorySettings);

    // Global actions
    dispatcher()->reg(this, "action://copy", this, &ApplicationActionController::doGlobalCopy);
    dispatcher()->reg(this, "action://cut", this, &ApplicationActionController::doGlobalCut);
    dispatcher()->reg(this, "action://paste", this, &ApplicationActionController::doGlobalPaste);
    dispatcher()->reg(this, "action://undo", this, &ApplicationActionController::doGlobalUndo);
    dispatcher()->reg(this, "action://redo", this, &ApplicationActionController::doGlobalRedo);
    dispatcher()->reg(this, "action://delete", this, &ApplicationActionController::doGlobalDelete);
    dispatcher()->reg(this, "action://cancel", this, &ApplicationActionController::doGlobalCancel);
}

bool ApplicationActionController::eventFilter(QObject* watched, QEvent* event)
{
    if ((event->type() == QEvent::Close && watched == qWindow())
        || event->type() == QEvent::Quit) {
        bool accepted = quit(false);
        event->setAccepted(accepted);

        return true;
    }

    if (watched == qApp) {
        if (event->type() == QEvent::FileOpen) {
            const QFileOpenEvent* openEvent = static_cast<const QFileOpenEvent*>(event);
            const QUrl url = openEvent->url();

            if (projectFilesController()->isUrlSupported(url)) {
                if (startupScenario()->startupCompleted()) {
                    dispatcher()->dispatch("file-open", ActionData::make_arg1<QUrl>(url));
                } else {
                    startupScenario()->setStartupScoreFile(project::ProjectFile { url });
                }

                return true;
            }
        }
    }

    if (watched == qWindow()) {
        if (event->type() == QEvent::DragEnter) {
            if (onDragEnterEvent(static_cast<QDragEnterEvent*>(event))) {
                return true;
            }
        } else if (event->type() == QEvent::DragMove) {
            if (onDragMoveEvent(static_cast<QDragMoveEvent*>(event))) {
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            if (onDropEvent(static_cast<QDropEvent*>(event))) {
                return true;
            }
        }
    }

    return QObject::eventFilter(watched, event);
}

QWindow* ApplicationActionController::qWindow() const
{
    return mainWindow() ? mainWindow()->qWindow() : nullptr;
}

ApplicationActionController::DragTarget ApplicationActionController::dragTarget(const QUrl& url) const
{
    if (projectFilesController()->isUrlSupported(url)) {
        return DragTarget::ProjectFile;
    } else if (url.isLocalFile()) {
        kfw::io::path_t filePath = url.toLocalFile();
        if (kfw::audio::synth::isSoundFont(filePath)) {
            return DragTarget::SoundFont;
        }

        static const QStringList RECORDINGS { "wav", "mp3", "flac", "ogg", "m4a", "aac", "aiff", "aif" };
        if (RECORDINGS.contains(QFileInfo(url.toLocalFile()).suffix().toLower())) {
            return DragTarget::Recording;
        }
    }
    return DragTarget::Unknown;
}

bool ApplicationActionController::onDragEnterEvent(QDragEnterEvent* event)
{
    return onDragMoveEvent(event);
}

bool ApplicationActionController::onDragMoveEvent(QDragMoveEvent* event)
{
    const QMimeData* mime = event->mimeData();
    QList<QUrl> urls = mime->urls();
    if (urls.count() > 0) {
        const QUrl& url = urls.front();
        DragTarget target = dragTarget(url);
        if (target != DragTarget::Unknown) {
            event->setDropAction(Qt::LinkAction);
            event->acceptProposedAction();
            return true;
        }
    }

    return false;
}

bool ApplicationActionController::onDropEvent(QDropEvent* event)
{
    const QMimeData* mime = event->mimeData();
    QList<QUrl> urls = mime->urls();
    if (urls.count() > 0) {
        const QUrl& url = urls.front();

        bool shouldBeHandled = true;
        DragTarget target = dragTarget(url);
        switch (target) {
        case DragTarget::ProjectFile: {
            async::Async::call(this, [this, url]() {
                    Ret ret = projectFilesController()->openProject(url);
                    if (!ret) {
                        LOGE() << ret.toString();
                    }
                });
        } break;
        case DragTarget::SoundFont: {
            kfw::io::path_t filePath = url.toLocalFile();
            async::Async::call(this, [this, filePath]() {
                    soundFontController()->addSoundFont(Uri::fromLocalFile(filePath));
                });
        } break;
        case DragTarget::Recording: {
            const QString filePath = url.toLocalFile();
            async::Async::call(this, [this, filePath]() {
                    UriQuery query("kitnote://audio-to-score");
                    query.addParam("audio", Val(filePath));
                    interactive()->open(query);
                });
        } break;
        case DragTarget::Unknown:
            shouldBeHandled = false;
            break;
        }

        if (shouldBeHandled) {
            event->accept();
        } else {
            event->ignore();
        }

        return shouldBeHandled;
    }

    return false;
}

bool ApplicationActionController::quit(bool isAllInstances, const kfw::io::path_t& installerPath)
{
    if (m_quiting) {
        return false;
    }

    m_quiting = true;
    DEFER {
        m_quiting = false;
    };

    if (!projectFilesController()->closeOpenedProject(false)) {
        return false;
    }

    if (isAllInstances) {
        multiwindowsProvider()->quitForAll();
    }

    if (multiwindowsProvider()->windowCount() == 1 && !installerPath.empty()) {
#if defined(Q_OS_LINUX)
        interactive()->revealInFileBrowser(installerPath);
#else
        interactive()->openUrl(QUrl::fromLocalFile(installerPath.toQString()));
#endif
    }

    if (multiwindowsProvider()->windowCount() > 1) {
        multiwindowsProvider()->notifyAboutWindowWasQuited();
    }

    QCoreApplication::exit();
    return true;
}

void ApplicationActionController::restart()
{
    if (projectFilesController()->closeOpenedProject(false)) {
        if (multiwindowsProvider()->windowCount() == 1) {
            application()->restart();
        } else {
            multiwindowsProvider()->quitAllAndRestartLast();

            QCoreApplication::exit();
        }
    }
}

void ApplicationActionController::toggleFullScreen()
{
    mainWindow()->toggleFullScreen();
}

void ApplicationActionController::openAboutDialog()
{
    interactive()->open("kitnote://about/musescore");
}

void ApplicationActionController::openAboutQtDialog()
{
    QApplication::aboutQt();
}

void ApplicationActionController::openAboutMusicXMLDialog()
{
    interactive()->open("kitnote://about/musicxml");
}

// KitNote: the guide is installed with the program, in Korean and in English.
//! KitNote: lets the interface be driven and looked at without a person at the screen (the checks of
//! tools/kitnote/ui_check.py). When the environment variable KITNOTE_TEST_UI names a text file, its
//! lines are run one after the other, a little while after the window has come up:
//!     do <action code>      dispatches an action ("kitnote-step-grid", "note-input" ...)
//!     wait <milliseconds>   lets the interface settle
//!     shot <file.png>       saves a picture of the front window: a dialog or menu when one is open (shotmain: the main window)
//!     click <x> <y>         a click in the front window (rclick: with the right button)
//!     drag <x> <y> <x> <y>  presses, moves and lets go
//!     wheel <x> <y> <steps> scrolls (down for steps > 0)
//!     key <sequence>        a key ("Ctrl+K", "Escape"); type <text> for text
//!     close                 closes the dialog or menu in front
//!     size <width> <height> gives the main window a size of its own (it is no longer maximized)
//!     quit                  closes KitNote without asking
//! Nothing happens when the variable is not set.
void ApplicationActionController::startKitNoteUiScript()
{
    const QString path = qEnvironmentVariable("KITNOTE_TEST_UI");
    if (path.isEmpty()) {
        return;
    }

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        LOGE() << "KITNOTE_TEST_UI: cannot read " << path;
        return;
    }

    m_uiScript = QString::fromUtf8(file.readAll()).split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    m_uiScriptStep = 0;

    QTimer::singleShot(9000, this, [this]() { runKitNoteUiScriptStep(); });
}

//! The window a person would be looking at: the dialog, menu or popup opened last, or else the main window.
static QWindow* kitNoteFrontWindow(QWindow* main)
{
    QWindow* front = main;
    for (QWindow* window : QGuiApplication::topLevelWindows()) {
        if (window != main && window->isVisible() && qobject_cast<QQuickWindow*>(window) && window->width() > 1) {
            front = window;
        }
    }
    return front;
}

void ApplicationActionController::runKitNoteUiScriptStep()
{
    if (m_uiScriptStep >= m_uiScript.size()) {
        return;
    }

    const QString line = m_uiScript.at(m_uiScriptStep++).trimmed();
    const QString command = line.section(QLatin1Char(' '), 0, 0);
    const QString argument = line.section(QLatin1Char(' '), 1).trimmed();

    int delay = 350;

    if (command == QLatin1String("do")) {
        dispatcher()->dispatch(argument.toStdString());
    } else if (command == QLatin1String("wait")) {
        delay = argument.toInt();
    } else if (command == QLatin1String("shot") || command == QLatin1String("shotmain")) {
        QWindow* main = mainWindow() ? mainWindow()->qWindow() : nullptr;
        QQuickWindow* window = qobject_cast<QQuickWindow*>(command == QLatin1String("shot") ? kitNoteFrontWindow(main) : main);
        if (window) {
            LOGI() << "KITNOTE_TEST_UI: window " << window->width() << "x" << window->height() << " ratio " << window->devicePixelRatio()
                   << " exposed " << window->isExposed() << " visibility " << int(window->visibility());
        }
        if (!window || !window->grabWindow().save(argument)) {
            LOGE() << "KITNOTE_TEST_UI: no picture written to " << argument;
        }
    } else if (command == QLatin1String("click") || command == QLatin1String("rclick") || command == QLatin1String("drag")) {
        // positions are in the window, in the units of the interface (a picture is devicePixelRatio times as large)
        if (QWindow* window = kitNoteFrontWindow(mainWindow() ? mainWindow()->qWindow() : nullptr)) {
            const QStringList numbers = argument.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            const QPointF from(numbers.value(0).toDouble(), numbers.value(1).toDouble());
            const QPointF to = command == QLatin1String("drag") ? QPointF(numbers.value(2).toDouble(), numbers.value(3).toDouble()) : from;
            const Qt::MouseButton button = command == QLatin1String("rclick") ? Qt::RightButton : Qt::LeftButton;

            auto send = [window, button](QEvent::Type type, const QPointF& pos, Qt::MouseButton changed, Qt::MouseButtons down) {
                QMouseEvent event(type, pos, window->mapToGlobal(pos), changed, down, Qt::NoModifier);
                QCoreApplication::sendEvent(window, &event);
            };

            send(QEvent::MouseMove, from, Qt::NoButton, Qt::NoButton);
            send(QEvent::MouseButtonPress, from, button, button);
            if (to != from) {
                for (int i = 1; i <= 8; ++i) {
                    send(QEvent::MouseMove, from + (to - from) * i / 8.0, Qt::NoButton, button);
                }
            }
            send(QEvent::MouseButtonRelease, to, button, Qt::NoButton);
        }
    } else if (command == QLatin1String("wheel")) {
        // wheel <x> <y> <steps>: scrolls down (steps > 0) or up
        if (QWindow* window = kitNoteFrontWindow(mainWindow() ? mainWindow()->qWindow() : nullptr)) {
            const QStringList numbers = argument.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            const QPointF pos(numbers.value(0).toDouble(), numbers.value(1).toDouble());
            const QPoint delta(0, -120 * numbers.value(2).toInt());
            QWheelEvent event(pos, window->mapToGlobal(pos), QPoint(), delta, Qt::NoButton, Qt::NoModifier, Qt::NoScrollPhase, false);
            QCoreApplication::sendEvent(window, &event);
        }
    } else if (command == QLatin1String("key")) {
        // a key sequence as in the shortcut preferences: "Ctrl+K", "Escape", "Right"
        if (QWindow* window = kitNoteFrontWindow(mainWindow() ? mainWindow()->qWindow() : nullptr)) {
            const QKeyCombination combination = QKeySequence(argument)[0];
            for (QEvent::Type type : { QEvent::KeyPress, QEvent::KeyRelease }) {
                QKeyEvent event(type, combination.key(), combination.keyboardModifiers());
                QCoreApplication::sendEvent(window, &event);
            }
        }
    } else if (command == QLatin1String("type")) {
        if (QWindow* window = kitNoteFrontWindow(mainWindow() ? mainWindow()->qWindow() : nullptr)) {
            for (const QChar& character : argument) {
                for (QEvent::Type type : { QEvent::KeyPress, QEvent::KeyRelease }) {
                    QKeyEvent event(type, 0, Qt::NoModifier, QString(character));
                    QCoreApplication::sendEvent(window, &event);
                }
            }
        }
    } else if (command == QLatin1String("drop")) {
        // drop <file>: a file dropped on the middle of the window
        if (QWindow* window = mainWindow() ? mainWindow()->qWindow() : nullptr) {
            QMimeData mime;
            mime.setUrls({ QUrl::fromLocalFile(argument) });
            const QPointF pos(window->width() / 2, window->height() / 2);
            QDropEvent event(pos, Qt::CopyAction | Qt::LinkAction, &mime, Qt::LeftButton, Qt::NoModifier);
            QCoreApplication::sendEvent(window, &event);
        }
    } else if (command == QLatin1String("close")) {
        QWindow* main = mainWindow() ? mainWindow()->qWindow() : nullptr;
        if (QWindow* window = kitNoteFrontWindow(main); window && window != main) {
            window->close();
            if (window->isVisible()) {
                window->hide();     // a dialog that asks before it closes
            }
        }
    } else if (command == QLatin1String("size")) {
        if (QWindow* window = mainWindow() ? mainWindow()->qWindow() : nullptr) {
            window->showNormal();
            window->setGeometry(40, 40, argument.section(QLatin1Char(' '), 0, 0).toInt(), argument.section(QLatin1Char(' '), 1, 1).toInt());
        }
        delay = 1500;
    } else if (command == QLatin1String("quit")) {
        QCoreApplication::exit(0);
        return;
    } else if (!line.startsWith(QLatin1Char('#'))) {
        LOGE() << "KITNOTE_TEST_UI: unknown line " << line;
    }

    QTimer::singleShot(delay, this, [this]() { runKitNoteUiScriptStep(); });
}

void ApplicationActionController::openKitNoteGuide()
{
    const bool korean = languagesService()->currentLanguage().code.startsWith(QLatin1String("ko"));
    const QString folder = QDir::cleanPath(globalConfiguration()->appDataPath().toQString() + QStringLiteral("/manual/kitnote"));

    // the guide in the interface language, or else the other one
    for (const QString& language : { korean ? QStringLiteral("ko") : QStringLiteral("en"),
                                     korean ? QStringLiteral("en") : QStringLiteral("ko") }) {
        const QString path = folder + QStringLiteral("/user-guide_%1.html").arg(language);
        if (QFileInfo::exists(path)) {
            interactive()->openUrl(QUrl::fromLocalFile(path));
            return;
        }
    }

    LOGE() << "KitNote user guide not found in " << folder;
}

void ApplicationActionController::openPreferencesDialog()
{
    const context::IPlaybackStatePtr state = globalContext()->playbackState();
    if (state->playbackStatus() == audio::PlaybackStatus::Running) {
        dispatcher()->dispatch("stop");

        async::Channel<audio::PlaybackStatus> statusChanged = state->playbackStatusChanged();
        statusChanged.onReceive(this, [statusChanged, this](audio::PlaybackStatus) {
            auto statusChangedMut = statusChanged;
            statusChangedMut.disconnect(this);
            doOpenPreferencesDialog();
        });

        return;
    }

    doOpenPreferencesDialog();
}

void ApplicationActionController::doOpenPreferencesDialog()
{
    if (multiwindowsProvider()->isPreferencesAlreadyOpened()) {
        multiwindowsProvider()->activateWindowWithOpenedPreferences();
        return;
    }

    interactive()->open("kitnote://preferences");
}

void ApplicationActionController::revertToFactorySettings()
{
    std::string title = kfw::trc("appshell", "Are you sure you want to revert to factory settings?");
    std::string question = kfw::trc("appshell", "This action will reset all your app preferences and delete all custom palettes and custom shortcuts. "
                                                 "The list of recent scores will also be cleared.\n\n"
                                                 "This action will not delete any of your scores.");

    IInteractive::ButtonData cancelBtn = interactive()->buttonData(IInteractive::Button::Cancel);
    cancelBtn.accent = true;

    int revertBtn = int(IInteractive::Button::Apply);
    auto promise = interactive()->warning(title, question,
                                          { cancelBtn,
                                            IInteractive::ButtonData(revertBtn, kfw::trc("appshell", "Revert")) },
                                          cancelBtn.btn, { kfw::IInteractive::Option::WithIcon },
                                          kfw::trc("appshell", "Revert to factory settings"));

    promise.onResolve(this, [this](const IInteractive::Result& res) {
        if (res.isButton(IInteractive::Button::Cancel)) {
            return;
        }

        static constexpr bool KEEP_DEFAULT_SETTINGS = false;
        static constexpr bool NOTIFY_ABOUT_CHANGES = false;
        static constexpr bool NOTIFY_OTHER_INSTANCES = false;
        configuration()->revertToFactorySettings(KEEP_DEFAULT_SETTINGS, NOTIFY_ABOUT_CHANGES, NOTIFY_OTHER_INSTANCES);

        std::string title = kfw::trc("appshell", "Would you like to restart KitNote now?");
        std::string question = kfw::trc("appshell", "KitNote needs to be restarted for these changes to take effect.");

        int restartBtn = int(IInteractive::Button::Apply);
        auto promise = interactive()->question(title, question,
                                               { interactive()->buttonData(IInteractive::Button::Cancel),
                                                 IInteractive::ButtonData(restartBtn, kfw::trc("appshell", "Restart"), true) },
                                               restartBtn);

        promise.onResolve(this, [this](const IInteractive::Result& res) {
            if (!res.isButton(IInteractive::Button::Cancel)) {
                restart();
            }
        });
    });
}

bool ApplicationActionController::hasProjectAndIsFocused() const
{
    bool hasProject = globalContext()->currentProject() != nullptr;
    bool isFocused = uiContextResolver()->currentUiContext() == context::UiCtxProjectFocused;
    return hasProject && isFocused;
}

void ApplicationActionController::doGlobalCopy()
{
    if (hasProjectAndIsFocused()) {
        dispatcher()->dispatch("action://notation/copy");
    } else {
        // resolve other actions
    }
}

void ApplicationActionController::doGlobalCut()
{
    if (hasProjectAndIsFocused()) {
        dispatcher()->dispatch("action://notation/cut");
    } else {
        // resolve other actions
    }
}

void ApplicationActionController::doGlobalPaste()
{
    if (hasProjectAndIsFocused()) {
        dispatcher()->dispatch("action://notation/paste");
    } else {
        // resolve other actions
    }
}

void ApplicationActionController::doGlobalUndo()
{
    if (hasProjectAndIsFocused()) {
        dispatcher()->dispatch("action://notation/undo");
    } else {
        // resolve other actions
    }
}

void ApplicationActionController::doGlobalRedo()
{
    if (hasProjectAndIsFocused()) {
        dispatcher()->dispatch("action://notation/redo");
    } else {
        // resolve other actions
    }
}

void ApplicationActionController::doGlobalDelete()
{
    if (hasProjectAndIsFocused()) {
        dispatcher()->dispatch("action://notation/delete");
    } else {
        // resolve other actions
    }
}

void ApplicationActionController::doGlobalCancel()
{
    if (hasProjectAndIsFocused()) {
        dispatcher()->dispatch("action://notation/cancel");
    } else {
        dispatcher()->dispatch("nav-escape");
    }
}
