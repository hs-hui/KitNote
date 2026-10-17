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
#include "aipreferencesmodel.h"

#include <QClipboard>
#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QSaveFile>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QUrl>

#include "translation.h"
#include "log.h"

using namespace kn::preferences;
using namespace kfw;

// --- the audio engine ------------------------------------------------------------------------------

AiEngine* AiEngine::instance()
{
    static AiEngine engine;
    return &engine;
}

QString AiEngine::root()
{
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/KitNote/ai";
}

QString AiEngine::python()
{
#ifdef Q_OS_WIN
    return root() + "/venv/Scripts/python.exe";
#else
    return root() + "/venv/bin/python";
#endif
}

//! held while the engine is installed or removed - by KitNote, or by an AI tool through kitnote-mcp
static QString engineLockPath()
{
    return QFileInfo(AiEngine::root()).absolutePath() + "/ai-engine.lock";
}

int AiEngine::version()
{
    QFile file(root() + "/engine.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return 0;
    }
    // (written by PowerShell 5.1 with a byte order mark)
    QByteArray text = file.readAll();
    if (text.startsWith("\xEF\xBB\xBF")) {
        text.remove(0, 3);
    }
    return QJsonDocument::fromJson(text).object().value("version").toInt(1);
}

bool AiEngine::installed()
{
    return QFileInfo::exists(python()) && QFileInfo::exists(root() + "/engine.json");
}

bool AiEngine::run(Job job, const QString& program, const QStringList& arguments, const QString& firstStatus)
{
    if (m_process) {
        return false;
    }
    if (job == Job::Install) {
        QDir().mkpath(QFileInfo(engineLockPath()).absolutePath());
        m_lock = std::make_unique<QLockFile>(engineLockPath());
        m_lock->setStaleLockTime(0);
        if (!m_lock->tryLock(200)) {
            m_lock.reset();
            m_status = kfw::qtrc("preferences/ai", "An AI tool is installing the audio engine just now. Try again when it has finished.");
            emit changed();
            return false;
        }
    }

    m_job = job;
    m_process = new QProcess(this);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
    m_progress = 0;
    m_status = firstStatus;
    m_output.clear();
    m_pending.clear();

    connect(m_process, &QProcess::readyReadStandardOutput, this, [this]() {
        const QByteArray chunk = m_process->readAllStandardOutput();
        m_output += QString::fromLocal8Bit(chunk);
        // whole lines only: a chunk may end in the middle of one
        m_pending += chunk;
        qsizetype lineEnd;
        while ((lineEnd = m_pending.indexOf('\n')) >= 0) {
            const QString line = QString::fromLocal8Bit(m_pending.left(lineEnd));
            m_pending.remove(0, lineEnd + 1);
            if (line.startsWith("PROGRESS ")) {
                m_progress = line.section(' ', 1, 1).toInt();
                // the words of the scripts are English; they are in the translations of this page
                m_status = kfw::qtrc("preferences/ai", line.section(' ', 2).trimmed().toUtf8().constData());
                emit changed();
            }
        }
    });
    auto end = [this](bool ok) {
        const QString output = m_output;
        const Job job = m_job;
        m_process->deleteLater();
        m_process = nullptr;
        m_job = Job::None;
        m_lock.reset();
        if (!ok) {
            const QStringList lines = output.trimmed().split('\n');
            m_status = lines.isEmpty() ? QString() : lines.constLast().trimmed();
        }
        emit changed();
        emit finished(ok, output, job);
    };
    connect(m_process, &QProcess::finished, this, [end](int exitCode, QProcess::ExitStatus exitStatus) {
        end(exitStatus == QProcess::NormalExit && exitCode == 0);
    });
    // a program that does not start sends no "finished"
    connect(m_process, &QProcess::errorOccurred, this, [this, end](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            m_output += "\n" + m_process->program() + ": " + m_process->errorString();
            end(false);
        }
    });

    m_process->start(program, arguments);
    emit changed();
    return true;
}

void AiEngine::stop()
{
    if (m_process) {
        m_process->kill();
    }
}

static QString shareFile(const kfw::IGlobalConfiguration* configuration, const QString& relative)
{
    return configuration->appDataPath().toQString() + relative;
}

//! The engine is downloaded only after this question is answered with "Install".
static bool askForEngineConsent(kfw::IInteractive* interactive)
{
    constexpr int INSTALL = static_cast<int>(IInteractive::Button::CustomButton) + 1;
    const QString text = kfw::qtrc("preferences/ai",
                                    "To turn recordings into drum scores, KitNote needs an audio engine that is not part of it. "
                                    "If you agree, it downloads and installs, into %1 only:\n\n"
                                    "• uv, a Python installer (github.com/astral-sh/uv)\n"
                                    "• Python 3.11\n"
#ifdef Q_OS_WIN
                                    "• PyTorch for the processor (download.pytorch.org)\n"
#else
                                    "• PyTorch for the processor (pypi.org)\n"
#endif
                                    "• Demucs and its separation model htdemucs (by Meta, MIT licence)\n"
                                    "• Beat This!, a model that finds the beats and the measures (JKU Linz, MIT licence)\n\n"
                                    "This takes about 1.2 GB of disk space and some minutes. Nothing else on the computer is changed, "
                                    "and Preferences > AI removes it all again.")
                         .arg(QDir::toNativeSeparators(AiEngine::root()));

    const IInteractive::Result result = interactive->questionSync(
        kfw::trc("preferences/ai", "Install the audio engine?"), text.toStdString(), {
        IInteractive::ButtonData(INSTALL, kfw::trc("preferences/ai", "Install")),
        IInteractive::ButtonData(static_cast<int>(IInteractive::Button::Cancel), kfw::trc("global", "Cancel"))
    });
    return result.button() == INSTALL;
}

static void startEngineInstall(const kfw::IGlobalConfiguration* configuration)
{
#ifdef Q_OS_WIN
    AiEngine::instance()->run(AiEngine::Job::Install, "powershell", { "-NoProfile", "-ExecutionPolicy", "Bypass", "-File",
                                              QDir::toNativeSeparators(shareFile(configuration, "ai/install_engine.ps1")),
                                              "-Root", QDir::toNativeSeparators(AiEngine::root()) },
                              kfw::qtrc("preferences/ai", "starting"));
#else
    // a Mac: the same steps in sh (install_engine.sh), with the tools that come with macOS
    AiEngine::instance()->run(AiEngine::Job::Install, "/bin/sh", { shareFile(configuration, "ai/install_engine.sh"),
                                              "--root", AiEngine::root() },
                              kfw::qtrc("preferences/ai", "starting"));
#endif
}

// --- AI tools --------------------------------------------------------------------------------------

namespace {
struct Client {
    const char* id;
    const char* name;
    QString configFile;     // where the MCP server goes
    QString skillDir;       // where the skill goes; empty: the tool has no skills
    QString detectDir;      // the tool is there when this folder is (or its program is on the PATH)
    const char* program;
    bool toml = false;
};

//! The home folder (KITNOTE_AI_HOME stands in for it in tests, so that real settings are not touched)
QString home()
{
    return qEnvironmentVariable("KITNOTE_AI_HOME", QDir::homePath());
}

//! A program by its name: on the PATH, or (a Mac: a program started from the Dock has the PATH of the system only)
//! where its installers put it
QString findTool(const QString& name)
{
    const QString found = QStandardPaths::findExecutable(name);
    if (!found.isEmpty()) {
        return found;
    }
#ifdef Q_OS_MAC
    for (const QString& folder : { home() + "/.local/bin", QString("/opt/homebrew/bin"), QString("/usr/local/bin") }) {
        if (QFileInfo(folder + "/" + name).isFile()) {
            return folder + "/" + name;
        }
    }
#endif
    return QString();
}

std::vector<Client> allClients()
{
    // Claude Desktop keeps its settings in the roaming application data (%APPDATA%); on a Mac in
    // ~/Library/Application Support
#ifdef Q_OS_MAC
    const QString appDataDefault = home() + "/Library/Application Support";
#else
    const QString appDataDefault = qEnvironmentVariable("APPDATA", QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation));
#endif
    const QString appData = qEnvironmentVariableIsSet("KITNOTE_AI_HOME") ? home() + "/AppData/Roaming" : appDataDefault;
    return {
        { "claude-code", "Claude Code", home() + "/.claude.json", home() + "/.claude/skills/kitnote", home() + "/.claude", "claude" },
        { "codex", "Codex", home() + "/.codex/config.toml", home() + "/.codex/skills/kitnote", home() + "/.codex", "codex", true },
        { "claude-desktop", "Claude Desktop", appData + "/Claude/claude_desktop_config.json", QString(), appData + "/Claude", "" },
        { "cursor", "Cursor", home() + "/.cursor/mcp.json", QString(), home() + "/.cursor", "cursor" },
        // Gemini CLI: an extension of its own - the server and GEMINI.md (the skill) in one folder
        { "gemini", "Gemini CLI", home() + "/.gemini/extensions/kitnote/gemini-extension.json", home() + "/.gemini/extensions/kitnote",
          home() + "/.gemini", "gemini" },
    };
}

const Client* findClient(const QString& id)
{
    static const std::vector<Client> clients = allClients();
    for (const Client& client : clients) {
        if (id == client.id) {
            return &client;
        }
    }
    return nullptr;
}

QByteArray readFile(const QString& path)
{
    QFile file(path);
    return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}

//! A settings file as it is: false when it is there but cannot be read (it is then not to be rewritten)
bool readSettings(const QString& path, QByteArray& data)
{
    data.clear();
    if (!QFileInfo::exists(path)) {
        return true;
    }
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    data = file.readAll();
    return file.error() == QFileDevice::NoError;
}

//! Copies of a settings file before KitNote changes it: name.kitnote-backup is the user's own file as it was
//! the first time, name.kitnote-backup-last the file as it was before the last change. False when no copy
//! could be made.
bool backup(const QString& path)
{
    if (!QFileInfo::exists(path)) {
        return true;
    }
    if (!QFileInfo::exists(path + ".kitnote-backup")) {
        QFile::copy(path, path + ".kitnote-backup");
    }
    QFile::remove(path + ".kitnote-backup-last");
    return QFile::copy(path, path + ".kitnote-backup-last");
}

//! Written whole, or not at all (a tool that reads the file meanwhile never sees half of it)
bool writeFile(const QString& path, const QByteArray& data)
{
    QDir().mkpath(QFileInfo(path).absolutePath());
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    file.write(data);
    return file.commit();
}

const char* TOML_START = "# >>> KitNote MCP server (Preferences > AI in KitNote removes it)";
const char* TOML_END = "# <<< KitNote";

//! (bytes, not text: the rest of the file stays exactly as it is, whatever its encoding)
QByteArray tomlWithout(const QByteArray& text)
{
    const qsizetype start = text.indexOf(TOML_START);
    if (start < 0) {
        return text;
    }
    qsizetype end = text.indexOf(TOML_END, start);
    if (end < 0) {
        return text;        // a block without its end: better left alone than everything after it taken away
    }
    end = text.indexOf('\n', end) + 1;
    if (end <= 0) {
        end = text.size();
    }
    return text.left(start) + text.mid(end);
}

//! the file the skill is copied to: Gemini CLI reads the context of an extension from GEMINI.md
QString skillName(const Client& client)
{
    return QString(client.id) == "gemini" ? QStringLiteral("GEMINI.md") : QStringLiteral("SKILL.md");
}

bool hasServer(const Client& client)
{
    if (QString(client.id) == "gemini") {
        return QFileInfo::exists(client.configFile);
    }
    const QByteArray data = readFile(client.configFile);
    if (client.toml) {
        return data.contains(TOML_START);
    }
    return QJsonDocument::fromJson(data).object().value("mcpServers").toObject().contains("kitnote");
}

//! Claude Code rewrites ~/.claude.json all the time: its own command changes the file, when it is there.
bool setServerWithClaudeCli(const QString& serverPath, bool add)
{
    const QString claude = findTool("claude");
    if (claude.isEmpty() || qEnvironmentVariableIsSet("KITNOTE_AI_HOME")) {
        return false;
    }

    QProcess process;
    if (add) {
        QProcess remove;
        remove.start(claude, { "mcp", "remove", "--scope", "user", "kitnote" });
        remove.waitForFinished(30000);
        process.start(claude, { "mcp", "add", "--scope", "user", "kitnote", "--", QDir::toNativeSeparators(serverPath) });
    } else {
        process.start(claude, { "mcp", "remove", "--scope", "user", "kitnote" });
    }
    return process.waitForFinished(30000) && process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
}

bool setServer(const Client& client, const QString& serverPath, bool add)
{
    if (QString(client.id) == "claude-code" && setServerWithClaudeCli(serverPath, add)) {
        return true;
    }

    if (QString(client.id) == "gemini") {
        if (!add) {
            return QDir(QFileInfo(client.configFile).absolutePath()).removeRecursively();
        }
        const QJsonObject extension {
            { "name", "kitnote" }, { "version", "1.0.0" },
            { "description", "Drum scores with KitNote: write, change, read, export, and transcribe recordings" },
            { "contextFileName", "GEMINI.md" },
            { "mcpServers", QJsonObject { { "kitnote", QJsonObject { { "command", QDir::toNativeSeparators(serverPath) },
                                                                    { "args", QJsonArray() }, { "timeout", 3600000 } } } } },
        };
        return writeFile(client.configFile, QJsonDocument(extension).toJson(QJsonDocument::Indented));
    }

    QByteArray data;
    if (!readSettings(client.configFile, data)) {
        return false;       // a file KitNote cannot read is not rewritten
    }
    if (!add && data.isEmpty()) {
        return true;
    }

    if (client.toml) {
        QByteArray text = tomlWithout(data);
        if (add) {
            if (text.contains("[mcp_servers.kitnote]")) {
                return false;       // the user has a "kitnote" server of their own: not a second table
            }
            if (!text.isEmpty() && !text.endsWith('\n')) {
                text += '\n';
            }
            // a TOML basic string: backslashes and quotes escaped
            QString command = QDir::toNativeSeparators(serverPath);
            command.replace("\\", "\\\\").replace("\"", "\\\"");
            text += QString("\n%1\n[mcp_servers.kitnote]\ncommand = \"%2\"\nargs = []\nstartup_timeout_sec = 30\n"
                            "tool_timeout_sec = 3600\n%3\n").arg(TOML_START, command, TOML_END).toUtf8();
        }
        return backup(client.configFile) && writeFile(client.configFile, text);
    }

    // a file KitNote cannot read is not rewritten - nor an empty one that is there (another program may be
    // writing it just now)
    if (QFileInfo::exists(client.configFile) && data.trimmed().isEmpty()) {
        return false;
    }
    QJsonParseError error;
    const QJsonDocument document = data.isEmpty() ? QJsonDocument(QJsonObject()) : QJsonDocument::fromJson(data, &error);
    if (!data.isEmpty() && (error.error != QJsonParseError::NoError || !document.isObject())) {
        return false;
    }
    QJsonObject root = document.object();
    QJsonObject servers = root.value("mcpServers").toObject();
    if (add) {
        QJsonObject server { { "command", QDir::toNativeSeparators(serverPath) }, { "args", QJsonArray() } };
        if (QString(client.id) == "claude-code") {
            server["type"] = "stdio";
            server["env"] = QJsonObject();
        }
        if (QString(client.id) == "gemini") {
            server["timeout"] = 3600000;
        }
        servers["kitnote"] = server;
    } else {
        servers.remove("kitnote");
    }
    root["mcpServers"] = servers;
    return backup(client.configFile) && writeFile(client.configFile, QJsonDocument(root).toJson(QJsonDocument::Indented));
}
}

AiPreferencesModel::AiPreferencesModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
    connect(AiEngine::instance(), &AiEngine::changed, this, &AiPreferencesModel::engineChanged);
    connect(AiEngine::instance(), &AiEngine::finished, this, [this](bool ok, const QString&, AiEngine::Job job) {
        emit engineChanged();
        if (!ok && job == AiEngine::Job::Install) {
            interactive()->warning(kfw::trc("preferences/ai", "The audio engine could not be installed"),
                                   AiEngine::instance()->status().toStdString());
        }
    });
}

QString AiPreferencesModel::serverPath() const
{
#ifdef Q_OS_WIN
    return QDir::toNativeSeparators(QCoreApplication::applicationDirPath() + "/kitnote-mcp.exe");
#else
    return QCoreApplication::applicationDirPath() + "/kitnote-mcp";
#endif
}

QString AiPreferencesModel::skillFile() const
{
    return shareFile(globalConfiguration().get(), "ai/SKILL.md");
}

QVariantList AiPreferencesModel::clients() const
{
    QVariantList result;
    for (const Client& client : allClients()) {
        const bool found = QFileInfo::exists(client.detectDir)
                           || (client.program[0] && !findTool(client.program).isEmpty());
        const bool skillThere = client.skillDir.isEmpty() || QFileInfo::exists(client.skillDir + "/" + skillName(client));
        QVariantMap item;
        item["id"] = client.id;
        item["name"] = client.name;
        item["found"] = found;
        item["installed"] = hasServer(client) && skillThere;
        item["where"] = QDir::toNativeSeparators(client.configFile);
        item["skill"] = !client.skillDir.isEmpty();
        result << item;
    }
    return result;
}

void AiPreferencesModel::install(const QString& id)
{
    const Client* client = findClient(id);
    if (!client) {
        return;
    }

    bool ok = setServer(*client, serverPath(), true);
    if (ok && !client->skillDir.isEmpty()) {
        QDir().mkpath(client->skillDir);
        QFile::remove(client->skillDir + "/" + skillName(*client));
        ok = QFile::copy(skillFile(), client->skillDir + "/" + skillName(*client));
    }

    emit clientsChanged();
    interactive()->info(message(ok, client->name).toStdString(), "");
}

void AiPreferencesModel::remove(const QString& id)
{
    const Client* client = findClient(id);
    if (!client) {
        return;
    }

    setServer(*client, serverPath(), false);
    if (!client->skillDir.isEmpty()) {
        QDir(client->skillDir).removeRecursively();
    }
    emit clientsChanged();
}

QString AiPreferencesModel::message(bool ok, const QString& what) const
{
    return ok ? kfw::qtrc("preferences/ai", "KitNote is set up in %1. Restart %1 to use it.").arg(what)
           : kfw::qtrc("preferences/ai", "KitNote could not be set up in %1: its settings file could not be read or written.").arg(what);
}

void AiPreferencesModel::copyServerCommand()
{
    QGuiApplication::clipboard()->setText(serverPath());
}

void AiPreferencesModel::openSkillFolder()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(skillFile()).absolutePath()));
}

bool AiPreferencesModel::engineInstalled() const
{
    return AiEngine::installed();
}

bool AiPreferencesModel::engineBusy() const
{
    return AiEngine::instance()->busy();
}

int AiPreferencesModel::engineProgress() const
{
    return AiEngine::instance()->progress();
}

QString AiPreferencesModel::engineStatus() const
{
    return AiEngine::instance()->status();
}

QString AiPreferencesModel::engineFolder() const
{
    return QDir::toNativeSeparators(AiEngine::root());
}

//! which drum model engine 2 finds the strokes with: KitNote's own, shipped with KitNote (ai/kitnote_drums.pt)
QString AiPreferencesModel::drumModel() const
{
    const QFileInfo weights(shareFile(globalConfiguration().get(), "ai/kitnote_drums.pt"));
    if (!weights.exists()) {
        return kfw::qtrc("preferences/ai", "KitNote's drum model is missing: the strokes are found by analysing frequency bands.");
    }
    return kfw::qtrc("preferences/ai", "Strokes: KitNote's own drum model (%1, %2 MB). Beats and measures: Beat This!")
           .arg(QLocale().toString(weights.lastModified().date(), QLocale::ShortFormat))
           .arg(QString::number(weights.size() / 1048576.0, 'f', 1));
}

void AiPreferencesModel::installEngine()
{
    if (!engineBusy() && askForEngineConsent(interactive().get())) {
        startEngineInstall(globalConfiguration().get());
    }
}

void AiPreferencesModel::removeEngine()
{
    if (engineBusy()) {
        return;
    }
    QLockFile lock(engineLockPath());
    lock.setStaleLockTime(0);
    if (!lock.tryLock(200)) {
        AiEngine::instance()->setStatus(kfw::qtrc("preferences/ai", "An AI tool is installing the audio engine just now. Try again when it has finished."));
        emit engineChanged();
        return;
    }
    QDir(AiEngine::root()).removeRecursively();
    emit engineChanged();
}

// --- a recording to a score ------------------------------------------------------------------------

AudioToScoreModel::AudioToScoreModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
    connect(AiEngine::instance(), &AiEngine::changed, this, &AudioToScoreModel::changed);
    connect(AiEngine::instance(), &AiEngine::finished, this, [this](bool ok, const QString& output, AiEngine::Job job) {
        if (job != AiEngine::Job::Transcribe || !m_waitingForTranscription) {
            emit changed();       // the engine was being installed
            return;
        }
        m_waitingForTranscription = false;
        if (!ok) {
            m_status = kfw::qtrc("preferences/ai", "The recording could not be transcribed: %1")
                       .arg(output.trimmed().split('\n').constLast());
            emit changed();
            return;
        }
        writeScore(m_temp->filePath("audio.json"));
    });
}

AudioToScoreModel::~AudioToScoreModel()
{
    // The dialog is closed: the files of the transcription go with it, so the transcription is not left
    // running for nothing (and the engine free for the next one).
    if (m_waitingForTranscription) {
        AiEngine::instance()->stop();
    }
    // a score being written is finished first, not left half written next to the recording
    for (QProcess* process : findChildren<QProcess*>()) {
        process->disconnect(this);
        process->waitForFinished(30000);
    }
}

void AudioToScoreModel::stop()
{
    if (!m_waitingForTranscription) {
        return;
    }
    m_waitingForTranscription = false;
    AiEngine::instance()->stop();
    m_status = kfw::qtrc("preferences/ai", "The transcription was stopped.");
    emit changed();
}

bool AudioToScoreModel::busy() const
{
    return AiEngine::instance()->busy() || m_writing;
}

int AudioToScoreModel::progress() const
{
    return m_writing ? 98 : AiEngine::instance()->progress();
}

QString AudioToScoreModel::status() const
{
    if (busy() || m_status.isEmpty()) {
        return m_writing ? kfw::qtrc("preferences/ai", "writing the score") : AiEngine::instance()->status();
    }
    return m_status;
}

void AudioToScoreModel::chooseAudio()
{
    // (tools/kitnote checks the dialog with a recording of its own: the file dialog of the system cannot be driven)
    if (qEnvironmentVariableIsSet("KITNOTE_TEST_AUDIO")) {
        m_audioPath = qEnvironmentVariable("KITNOTE_TEST_AUDIO");
        emit changed();
        return;
    }

    const kfw::io::path_t path = interactive()->selectOpeningFileSync(
        kfw::trc("preferences/ai", "Choose a recording"), kfw::io::path_t(QDir::homePath()),
        { kfw::trc("preferences/ai", "Recordings") + " (*.wav *.mp3 *.flac *.ogg *.m4a *.aac *.aiff *.aif)" });
    if (!path.empty()) {
        m_audioPath = path.toQString();
        m_status.clear();
        emit changed();
    }
}

void AudioToScoreModel::setAudio(const QString& pathOrUrl)
{
    // a path, or the file: URL of a file dropped on the dialog
    const QUrl url(pathOrUrl);
    const QString path = url.isLocalFile() ? url.toLocalFile() : pathOrUrl;
    if (!busy() && !path.isEmpty() && QFileInfo::exists(path)) {
        m_audioPath = QFileInfo(path).absoluteFilePath();
        m_status.clear();
        emit changed();
    }
}

void AudioToScoreModel::installEngine()
{
    if (!busy() && askForEngineConsent(interactive().get())) {
        startEngineInstall(globalConfiguration().get());
    }
}

void AudioToScoreModel::start(bool separate, int tempo, bool keepStems, const QString& meter, bool accurate, bool repeatSigns,
                              bool sections, bool ghostNotes)
{
    m_repeatSigns = repeatSigns;
    m_sections = sections;
    if (busy() || m_audioPath.isEmpty() || !AiEngine::installed()) {
        return;
    }

    m_temp = std::make_unique<QTemporaryDir>();
    QStringList arguments { QDir::toNativeSeparators(shareFile(globalConfiguration().get(), "ai/kitnote_audio.py")),
                            QDir::toNativeSeparators(m_audioPath),
                            "--json", QDir::toNativeSeparators(m_temp->filePath("audio.json")) };
    if (!separate) {
        arguments << "--no-separation";
    }
    if (tempo > 0) {
        arguments << "--tempo" << QString::number(tempo);
    }
    arguments << "--meter" << (meter.isEmpty() ? QStringLiteral("auto") : meter);
    if (separate && accurate) {
        arguments << "--accurate";
    }
    if (ghostNotes) {
        arguments << "--ghost-notes";
    }
    if (separate && keepStems) {
        // the separated instruments as wav files: "<song> stems/drums.wav", bass, vocals, other
        const QFileInfo audio(m_audioPath);
        arguments << "--stems" << QDir::toNativeSeparators(audio.absolutePath() + "/" + audio.completeBaseName() + " stems");
    }

    m_status.clear();
    m_waitingForTranscription = AiEngine::instance()->run(AiEngine::Job::Transcribe, AiEngine::python(), arguments,
                                                          kfw::qtrc("preferences/ai", "reading the recording"));
    emit changed();
}

//! "@<measure> section:<name>" for every section the transcription found (A, B ...), or nothing
static QByteArray sectionCommands(const QJsonObject& result, bool wanted)
{
    QByteArray commands;
    if (!wanted) {
        return commands;
    }
    for (const QJsonValue& value : result.value("sections").toArray()) {
        const QJsonObject section = value.toObject();
        commands += QString("\n@%1 section:%2").arg(section.value("measure").toInt()).arg(section.value("name").toString()).toUtf8();
    }
    return commands;
}

//! The tab into a new score from the drum kit template, next to the recording; then it is opened.
void AudioToScoreModel::writeScore(const QString& resultJson)
{
    const QJsonObject result = QJsonDocument::fromJson(readFile(resultJson)).object();
    const QString tab = result.value("tab").toString();
    if (tab.isEmpty() || result.value("strokes").toInt() == 0) {
        m_status = kfw::qtrc("preferences/ai", "No drums were found in the recording.");
        emit changed();
        return;
    }

    const QFileInfo audio(m_audioPath);
    QString output = audio.absolutePath() + "/" + audio.completeBaseName() + ".kitnote";
    for (int n = 2; QFileInfo::exists(output); ++n) {
        output = audio.absolutePath() + "/" + audio.completeBaseName() + QString(" (%1).kitnote").arg(n);
    }

    const QString tabFile = m_temp->filePath("tab.txt");
    writeFile(tabFile, tab.toUtf8());
    // one command to a line: a line break in the file name would make a command of the rest
    QString title = audio.completeBaseName();
    title.replace('\r', ' ').replace('\n', ' ');
    // where each measure begins in the recording: the score's playback follows them (an engine installed
    // before does not tell them)
    QByteArray timesCommand;
    if (result.value("measure_times").toArray().size() >= 2) {
        const QString timesFile = m_temp->filePath("measure_times.json");
        writeFile(timesFile, QJsonDocument(result.value("measure_times").toArray()).toJson(QJsonDocument::Compact));
        timesCommand = "\nrecordingtimes:" + QDir::toNativeSeparators(timesFile).toUtf8();
    }
    const QString commandFile = m_temp->filePath("commands.txt");
    writeFile(commandFile, QStringList {
        "title:" + title,
        "timesig:" + result.value("time_signature").toString("4/4"),
        QString("measures:%1").arg(std::max(1, result.value("measures").toInt())),
        QString("tempo:%1").arg(qRound(result.value("tempo").toDouble())),
        "tabpaste:" + tabFile,
    }.join('\n').toUtf8() + (m_repeatSigns ? QByteArray("\nrepeats") : QByteArray()) + sectionCommands(result, m_sections)
              // the song plays along with its score (the Mixer's "Recording"), in time with it
              + QByteArray("\nrecording:") + QDir::toNativeSeparators(m_audioPath).toUtf8() + timesCommand);

    m_writing = true;
    emit changed();

    auto* process = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("KITNOTE_APPLY_FILE", QDir::toNativeSeparators(commandFile));
    process->setProcessEnvironment(env);
    connect(process, &QProcess::errorOccurred, this, [this, process](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            process->deleteLater();
            m_writing = false;
            m_status = kfw::qtrc("preferences/ai", "The score could not be written.");
            emit changed();
        }
    });
    connect(process, &QProcess::finished, this, [this, process, output](int exitCode, QProcess::ExitStatus) {
        process->deleteLater();
        m_writing = false;
        if (exitCode != 0 || !QFileInfo::exists(output)) {
            m_status = kfw::qtrc("preferences/ai", "The score could not be written.");
            emit changed();
            return;
        }
        m_status = kfw::qtrc("preferences/ai", "Written: %1").arg(QDir::toNativeSeparators(output));
        emit changed();
        dispatcher()->dispatch("file-open", kfw::actions::ActionData::make_arg1<QUrl>(QUrl::fromLocalFile(output)));
        emit done();
    });
    process->start(QCoreApplication::applicationFilePath(),
                   { "-o", QDir::toNativeSeparators(output),
                     QDir::toNativeSeparators(shareFile(globalConfiguration().get(), "templates/01-Drums/01-Drum_Kit/01-Drum_Kit.mscx")) });
}
