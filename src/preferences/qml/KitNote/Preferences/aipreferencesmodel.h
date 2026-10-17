/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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

#include <QLockFile>
#include <QObject>
#include <QProcess>
#include <QTemporaryDir>
#include <QVariantList>

#include <memory>
#include <qqmlintegration.h>

#include "modularity/ioc.h"
#include "global/iglobalconfiguration.h"
#include "global/iinteractive.h"
#include "actions/iactionsdispatcher.h"

namespace kn::preferences {
//! KitNote: the audio engine - a Python of KitNote's own with PyTorch and the Demucs separation model,
//! installed after the user agrees (share/ai/install_engine.ps1 on Windows, install_engine.sh on a Mac) into
//! <local app data>/KitNote/ai (~/Library/Application Support/KitNote/ai on a Mac). It turns
//! a recording into a drum tab (share/ai/kitnote_audio.py). Shared by the AI page and the audio dialog.
class AiEngine : public QObject
{
    Q_OBJECT

public:
    static AiEngine* instance();

    static QString root();
    static QString python();
    static bool installed();
    //! 1: the band analysis; 2: with the beat model (Beat This!) and KitNote's drum model
    static int version();

    enum class Job {
        None,
        Install,
        Transcribe
    };

    bool busy() const { return m_process != nullptr; }
    Job job() const { return m_job; }
    int progress() const { return m_progress; }
    QString status() const { return m_status; }
    void setStatus(const QString& status) { m_status = status; emit changed(); }

    //! Runs a script of the engine; "PROGRESS <percent> <what>" lines of its output become progress.
    bool run(Job job, const QString& program, const QStringList& arguments, const QString& firstStatus);
    //! Ends the script that is running ("finished" follows, not ok).
    void stop();

signals:
    void changed();
    void finished(bool ok, const QString& output, Job job);

private:
    QProcess* m_process = nullptr;
    Job m_job = Job::None;
    std::unique_ptr<QLockFile> m_lock;     // while installing: kitnote-mcp's audio_engine waits for it
    int m_progress = 0;
    QString m_status;
    QString m_output;
    QByteArray m_pending;       // the output after the last complete line
};

//! KitNote: Preferences > AI. KitNote's MCP server (kitnote-mcp) and its skill (SKILL.md) put into the
//! settings of AI tools - Claude Code, Codex, Claude Desktop, Cursor, Gemini CLI - and taken out again;
//! and the audio engine installed or removed.
class AiPreferencesModel : public QObject, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(QVariantList clients READ clients NOTIFY clientsChanged)
    Q_PROPERTY(QString serverPath READ serverPath CONSTANT)
    Q_PROPERTY(bool engineInstalled READ engineInstalled NOTIFY engineChanged)
    Q_PROPERTY(bool engineUpgradable READ engineUpgradable NOTIFY engineChanged)
    Q_PROPERTY(bool engineBusy READ engineBusy NOTIFY engineChanged)
    Q_PROPERTY(int engineProgress READ engineProgress NOTIFY engineChanged)
    Q_PROPERTY(QString engineStatus READ engineStatus NOTIFY engineChanged)
    Q_PROPERTY(QString engineFolder READ engineFolder CONSTANT)
    Q_PROPERTY(QString drumModel READ drumModel CONSTANT)

    QML_ELEMENT

    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    explicit AiPreferencesModel(QObject* parent = nullptr);

    //! { id, name, found, installed, where, skill }
    QVariantList clients() const;
    QString serverPath() const;

    bool engineInstalled() const;
    bool engineUpgradable() const { return engineInstalled() && AiEngine::version() < 2; }
    bool engineBusy() const;
    int engineProgress() const;
    QString engineStatus() const;
    QString engineFolder() const;
    QString drumModel() const;

    Q_INVOKABLE void install(const QString& id);
    Q_INVOKABLE void remove(const QString& id);
    Q_INVOKABLE void copyServerCommand();
    Q_INVOKABLE void openSkillFolder();

    Q_INVOKABLE void installEngine();
    Q_INVOKABLE void removeEngine();

signals:
    void clientsChanged();
    void engineChanged();

private:
    QString skillFile() const;
    QString message(bool ok, const QString& what) const;
};

//! KitNote: File > Drum score from a recording: the recording is turned into a drum tab by the audio
//! engine, the tab into a score next to the recording, and the score is opened.
class AudioToScoreModel : public QObject, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(QString audioPath READ audioPath NOTIFY changed)
    Q_PROPERTY(bool engineInstalled READ engineInstalled NOTIFY changed)
    Q_PROPERTY(bool busy READ busy NOTIFY changed)
    Q_PROPERTY(bool transcribing READ transcribing NOTIFY changed)
    Q_PROPERTY(int progress READ progress NOTIFY changed)
    Q_PROPERTY(QString status READ status NOTIFY changed)

    QML_ELEMENT

    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };

public:
    explicit AudioToScoreModel(QObject* parent = nullptr);
    ~AudioToScoreModel() override;

    QString audioPath() const { return m_audioPath; }
    bool engineInstalled() const { return AiEngine::installed(); }
    bool busy() const;
    //! the recording of this dialog is being transcribed (not: the engine installed, the score written)
    bool transcribing() const { return m_waitingForTranscription; }
    int progress() const;
    QString status() const;

    Q_INVOKABLE void chooseAudio();
    //! the recording dropped on the window, or given another way
    Q_INVOKABLE void setAudio(const QString& path);
    Q_INVOKABLE void installEngine();
    Q_INVOKABLE void start(bool separate, int tempo, bool keepStems, const QString& meter, bool accurate, bool repeatSigns,
                           bool sections, bool ghostNotes = false);
    Q_INVOKABLE void stop();

signals:
    void changed();
    void done();

private:
    void writeScore(const QString& resultJson);

    QString m_audioPath;
    QString m_status;
    std::unique_ptr<QTemporaryDir> m_temp;      // the files of one transcription
    bool m_writing = false;
    bool m_repeatSigns = false;
    bool m_sections = true;
    bool m_waitingForTranscription = false;
};
}
