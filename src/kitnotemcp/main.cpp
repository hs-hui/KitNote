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

//! kitnote-mcp: KitNote as a Model Context Protocol server, so that AI tools (Claude Code, Codex, Claude
//! Desktop, Cursor, Gemini CLI ...) can write drum scores. It speaks JSON-RPC over stdin and stdout, a
//! message to a line, and does the work with KitNote itself: "KitNote -o out in" with the commands of
//! KITNOTE_APPLY_FILE (see applyKitNoteTestCommands in src/converter/internal/convertercontroller.cpp).
//! Scores are .kitnote files; the drum tab is the text form an AI reads and writes.

#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QLockFile>
#include <QMap>
#include <QProcess>
#include <QRegularExpression>
#include <QSaveFile>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QUrl>

#include <cstdio>
#include <iostream>
#include <string>

#ifdef Q_OS_WIN
#include <fcntl.h>
#include <io.h>
#endif

namespace {
const char* VERSION = "1.0.0";

QString appDir()
{
    return QCoreApplication::applicationDirPath();
}

QString kitNoteExe()
{
#ifdef Q_OS_WIN
    return appDir() + "/KitNote.exe";
#else
    return appDir() + "/KitNote";
#endif
}

//! The installed share folder: next to bin/ (templates/, ai/) - or, in a build tree, the source share/.
QString shareDir(const QString& sub)
{
    // (on a Mac the server is in KitNote.app/Contents/MacOS, and the share folder is KitNote.app/Contents/Resources)
    for (const QString& candidate : { appDir() + "/../" + sub, appDir() + "/../share/" + sub,
                                      appDir() + "/../Resources/" + sub }) {
        if (QFileInfo::exists(candidate)) {
            return QDir::cleanPath(candidate);
        }
    }
    return QDir::cleanPath(appDir() + "/../" + sub);
}

//! Where relative paths go: the working folder of the AI tool (Claude Code, Codex: the user's project) -
//! but Documents/KitNote when the server was started in a folder of programs or of the system (a chat app
//! such as Claude Desktop starts it in its own), or in one it cannot write to.
QString baseFolder()
{
    static const QString folder = []() {
        const QString cwd = QDir::cleanPath(QDir::currentPath());
        QStringList programs { QCoreApplication::applicationDirPath() };
        for (const char* variable : { "WINDIR", "ProgramFiles", "ProgramFiles(x86)", "ProgramW6432", "LOCALAPPDATA", "APPDATA",
                                      "ProgramData" }) {
            if (qEnvironmentVariableIsSet(variable)) {
                programs << QDir::cleanPath(QDir::fromNativeSeparators(qEnvironmentVariable(variable)));
            }
        }
#ifdef Q_OS_MAC
        // the system's and the programs' folders (the application data is the user's own, as on Windows)
        programs << "/Applications" << "/System" << "/Library" << "/usr" << "/bin" << "/sbin" << "/opt" << "/Volumes";
#endif
        bool usable = QFileInfo(cwd).isWritable() && !QDir(cwd).isRoot();
        for (const QString& program : programs) {
            usable = usable && !(cwd.compare(program, Qt::CaseInsensitive) == 0
                                 || cwd.startsWith(program + "/", Qt::CaseInsensitive));
        }
        // (the temporary folder is in the local application data, but a project may well be there)
        const QString temp = QDir::cleanPath(QDir::tempPath());
        if (usable || (QFileInfo(cwd).isWritable()
                       && (cwd.compare(temp, Qt::CaseInsensitive) == 0 || cwd.startsWith(temp + "/", Qt::CaseInsensitive)))) {
            return cwd;
        }
        const QString documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/KitNote";
        QDir().mkpath(documents);
        return documents;
    }();
    return folder;
}

QString absolute(const QString& path)
{
    return QDir::cleanPath(QDir(baseFolder()).absoluteFilePath(path));
}

struct Result {
    bool ok = true;
    QString text;
    QByteArray png;     // a picture to send with the text (view_score)
};

Result fail(const QString& text)
{
    return { false, text };
}

//! Runs KitNote on a score: reads "in", applies the commands, writes "out" (the same file is fine).
Result runKitNote(const QString& in, const QString& out, const QStringList& commands, const QStringList& extraArguments = {})
{
    // one command to a line: a line break in a title or a name would make a second command of the rest
    for (const QString& command : commands) {
        if (command.contains('\n') || command.contains('\r')) {
            return fail("a value may not contain a line break: " + command.left(80));
        }
    }

    QTemporaryDir temp;
    if (!temp.isValid()) {
        return fail("no temporary folder");
    }

    const QString commandFile = temp.filePath("commands.txt");
    QFile file(commandFile);
    if (!file.open(QIODevice::WriteOnly)) {
        return fail("cannot write " + commandFile);
    }
    file.write(commands.join('\n').toUtf8());
    file.close();

    // written next to itself first: the score is replaced only when KitNote has finished
    const QString target = absolute(out);

    // One change of a score at a time: two calls at once (two AI tools, or one that works in parallel) would
    // each read the score, change it and write it back, and the later one would undo the earlier.
    // (the score read and changed - not a template a new score is made from; one file under one name)
    const QString source = absolute(in);
    QString locked = source.startsWith(shareDir("templates"), Qt::CaseInsensitive) ? target : source;
    if (QFileInfo::exists(locked)) {
        locked = QFileInfo(locked).canonicalFilePath();
    }
    const QString lockName = QDir::tempPath() + "/kitnote-"
                             + QCryptographicHash::hash(locked.toLower().toUtf8(), QCryptographicHash::Sha1).toHex().left(16) + ".lock";
    QLockFile lock(lockName);
    lock.setStaleLockTime(15 * 60 * 1000);
    if (!lock.tryLock(10 * 60 * 1000)) {
        return fail("the score is being changed by another call; try again");
    }
    const QString staging = temp.filePath("out." + QFileInfo(target).suffix());

    QProcess process;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("KITNOTE_APPLY_FILE", QDir::toNativeSeparators(commandFile));
    const QString report = temp.filePath("report.txt");
    env.insert("KITNOTE_APPLY_REPORT", QDir::toNativeSeparators(report));
    // where the score will be (it is written into the temporary folder first): a recording next to it is kept by its name
    env.insert("KITNOTE_APPLY_SCORE", QDir::toNativeSeparators(target));
    env.remove("KITNOTE_APPLY");
    env.remove("KITNOTE_TEST_APPLY");
#ifdef Q_OS_MAC
    // a Mac: the conversion runs without a window or a Dock icon (the app bundle ships the offscreen platform)
    if (!env.contains("QT_QPA_PLATFORM")) {
        env.insert("QT_QPA_PLATFORM", "offscreen");
    }
#endif
    process.setProcessEnvironment(env);
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(kitNoteExe(), QStringList { "-o", QDir::toNativeSeparators(staging), QDir::toNativeSeparators(absolute(in)) }
                  + extraArguments);
    if (!process.waitForStarted(30000)) {
        return fail("KitNote could not be started: " + kitNoteExe());
    }
    if (!process.waitForFinished(10 * 60 * 1000)) {
        process.kill();
        return fail("KitNote took too long");
    }

    // pictures come page by page (out-1.png ...), everything else as out.<suffix>
    const QFileInfoList staged = QDir(temp.path()).entryInfoList({ "out.*", "out-*" }, QDir::Files);
    if (process.exitCode() != 0 || staged.isEmpty()) {
        return fail(QString("KitNote failed (exit code %1). %2").arg(process.exitCode())
                    .arg(QString::fromLocal8Bit(process.readAll()).right(800)));
    }

    QFile reportFile(report);
    if (reportFile.open(QIODevice::ReadOnly)) {
        const QString problems = QString::fromUtf8(reportFile.readAll()).trimmed();
        if (!problems.isEmpty()) {
            return fail("Nothing was written. " + problems);
        }
    }

    // several pages of a picture come as name-1.png, name-2.png ...
    QDir().mkpath(QFileInfo(target).absolutePath());
    QStringList written;
    for (const QFileInfo& info : staged) {
        const QString destination = QFileInfo(target).absolutePath() + "/" + QFileInfo(target).completeBaseName()
                                    + info.completeBaseName().mid(3) + "." + info.suffix();
        // replaced whole or not at all: a full disk does not leave half a score
        QFile source(info.absoluteFilePath());
        QSaveFile destinationFile(destination);
        if (!source.open(QIODevice::ReadOnly) || !destinationFile.open(QIODevice::WriteOnly)
            || destinationFile.write(source.readAll()) < 0 || !destinationFile.commit()) {
            return fail("cannot write " + destination);
        }
        written << QDir::toNativeSeparators(destination);
    }

    return { true, written.join('\n') };
}

//! The summary of a score (title, tempo, measures, sections, drum tab) as JSON text.
Result readScore(const QString& path)
{
    if (!QFileInfo::exists(absolute(path))) {
        return fail("no such score: " + path);
    }

    QTemporaryDir temp;
    const QString info = temp.filePath("info.json");
    const Result run = runKitNote(path, temp.filePath("copy.kitnote"), { "info:" + info });
    if (!run.ok) {
        if (run.text.startsWith("KitNote failed")) {
            return fail(path + " could not be read as a score. KitNote reads drum scores as .kitnote, .mscz, .mscx, "
                        "MusicXML (.musicxml, .mxl, .xml) and MIDI (.mid).");
        }
        return run;
    }
    QFile file(info);
    if (!file.open(QIODevice::ReadOnly)) {
        return fail("KitNote wrote no summary");
    }
    return { true, QString::fromUtf8(file.readAll()) };
}

QString templateFor(const QString& kit)
{
    static const QMap<QString, QString> TEMPLATES {
        { "standard", "01-Drum_Kit/01-Drum_Kit.mscx" },
        { "4-piece", "03-Drum_Kit_4-piece/03-Drum_Kit_4-piece.mscx" },
        { "large", "04-Drum_Kit_large/04-Drum_Kit_large.mscx" },
        { "marching", "02-Battery_Percussion/06-Battery_Percussion.mscx" },
    };
    const QString dir = shareDir("templates") + "/01-Drums/";
    QString file = dir + TEMPLATES.value(kit, TEMPLATES.value("standard"));
    if (!QFileInfo::exists(file)) {
        // the file name of a template may differ: the first .mscx of its folder
        const QString folder = QFileInfo(file).absolutePath();
        const QStringList found = QDir(folder).entryList({ "*.mscx" }, QDir::Files);
        if (!found.isEmpty()) {
            file = folder + "/" + found.first();
        }
    }
    return file;
}

//! A whole number of a call: a JSON number or a numeric string. "ok" is false for anything else.
int number(const QJsonObject& args, const char* key, int fallback, bool* ok = nullptr)
{
    const QJsonValue value = args.value(key);
    bool good = true;
    int result = fallback;
    if (value.isDouble()) {
        result = qRound(value.toDouble());
    } else if (value.isString()) {
        result = value.toString().trimmed().toInt(&good);
    } else if (!value.isUndefined() && !value.isNull()) {
        good = false;
    }
    if (ok) {
        *ok = *ok && good;
    }
    return good ? result : fallback;
}

//! "title", "tempo" ... of a call as KitNote commands; "ok" false and "problem" said for a value that is not right
QStringList propertyCommands(const QJsonObject& args, bool* ok, QString* problem)
{
    QStringList commands;
    if (args.contains("title")) {
        commands << "title:" + args.value("title").toString();
    }
    if (args.contains("composer")) {
        commands << "composer:" + args.value("composer").toString();
    }
    if (args.contains("time_signature")) {
        static const QRegularExpression SIGNATURE(QStringLiteral("^(\\d{1,2})/(1|2|4|8|16|32)$"));
        const QString signature = args.value("time_signature").toString().trimmed();
        const QRegularExpressionMatch match = SIGNATURE.match(signature);
        if (!match.hasMatch() || match.captured(1).toInt() < 1 || match.captured(1).toInt() > 32) {
            *ok = false;
            *problem = "time_signature must be like 4/4, 3/4, 6/8, 7/8 or 12/8";
        }
        commands << "timesig:" + signature;
    }
    if (args.contains("measures")) {
        const int measures = number(args, "measures", 0, ok);
        if (measures < 1 || measures > 2000) {
            *ok = false;
            *problem = "measures must be a number from 1 to 2000";
        }
        commands << QString("measures:%1").arg(measures);
    }
    if (args.contains("tempo")) {
        const int tempo = number(args, "tempo", 0, ok);
        if (tempo < 20 || tempo > 400) {
            *ok = false;
            *problem = "tempo must be a number from 20 to 400 (quarter notes a minute)";
        }
        commands << QString("tempo:%1").arg(tempo);
    }
    if (args.contains("swing")) {
        const QString swing = args.value("swing").toString();
        if (!QStringList { "off", "8", "16", "eighth", "sixteenth" }.contains(swing)) {
            *ok = false;
            *problem = "swing must be off, 8 or 16";
        }
        commands << (swing == "8" || swing == "eighth" ? "swing-8" : swing == "16" || swing == "sixteenth" ? "swing-16" : "swing-off");
    }
    // the recording played along with the score (in KitNote's Mixer, "Recording"); "" takes it away
    if (args.contains("recording")) {
        const QString file = args.value("recording").toString().trimmed();
        if (!file.isEmpty() && !QFileInfo(absolute(file)).isFile()) {
            *ok = false;
            *problem = "recording: no such file " + QDir::toNativeSeparators(absolute(file));
        }
        commands << (file.isEmpty() ? QString("recording-remove") : "recording:" + QDir::toNativeSeparators(absolute(file)));
    }
    if (args.contains("recording_offset")) {
        if (!args.value("recording_offset").isDouble()) {
            *ok = false;
            *problem = "recording_offset is a number of seconds";
        }
        commands << QString("recordingoffset:%1").arg(args.value("recording_offset").toDouble(), 0, 'f', 3);
    }
    // the timing of the recording (a transcription's): false takes its hidden tempo changes away
    if (args.contains("recording_timing")) {
        if (args.value("recording_timing").toBool(true)) {
            *ok = false;
            *problem = "recording_timing can only be false (a transcription gives a score the timing of its recording)";
        }
        commands << QString("recordingtempo-remove");
    }
    return commands;
}

//! "@first-last " of a call; empty when first_measure is missing or not a number of a measure
QString range(const QJsonObject& args, const char* first = "first_measure", const char* last = "last_measure")
{
    bool ok = true;
    const int from = number(args, first, 0, &ok);
    const int to = number(args, last, from, &ok);
    if (!ok || from <= 0) {
        return QString();
    }
    return QString("@%1-%2 ").arg(from).arg(std::max(from, to));
}

//! A score that the tools change in place: KitNote's own formats only (a MIDI or MusicXML file written
//! back would lose what it cannot hold).
bool editable(const QString& path)
{
    const QString suffix = QFileInfo(path).suffix().toLower();
    return suffix == "kitnote" || suffix == "mscz" || suffix == "mscx";
}

//! Commands edit_score passes on: the ones its description names - none that reads or writes other files,
//! or is meant for tests. Read as KitNote reads them: trimmed, after the range of measures.
bool allowedCommand(QString command)
{
    command = command.trimmed();
    if (command.startsWith('@')) {
        command = command.section(' ', 1).trimmed();
    }
    static const QStringList COMMANDS { "clear", "sticking-r", "sticking-l", "counting", "repeats", "write-out", "key",
                                        "key-remove", "swing-8", "swing-16", "swing-off", "repeat", "repeat-remove",
                                        "recordingtempo-remove" };
    static const QStringList WITH_VALUE { "pattern:", "section:", "fillsection:", "change:", "title:", "composer:", "tempo:",
                                          "timesig:", "measures:", "repeat:", "volta:" };
    if (COMMANDS.contains(command)) {
        return true;
    }
    for (const QString& prefix : WITH_VALUE) {
        if (command.startsWith(prefix)) {
            return true;
        }
    }
    return false;
}

Result withSummary(const QString& path, const Result& run, const QString& done)
{
    if (!run.ok) {
        return run;
    }
    const Result summary = readScore(path);
    if (!summary.ok) {
        return { true, done };
    }
    // music that ends before the score does: said plainly, so that the length asked for is kept
    QString note;
    const QJsonObject info = QJsonDocument::fromJson(summary.text.mid(summary.text.indexOf('{')).toUtf8()).object();
    const int measures = info.value("measures").toInt();
    const int empty = info.value("empty_measures_at_end").toInt();
    if (empty > 0 && empty < measures) {
        note = QString("\n\nNote: the last %1 measure(s), %2-%3, are empty. If the piece is %4 measures long, "
                       "set_properties with measures: %4 removes them; otherwise fill them.")
               .arg(empty).arg(measures - empty + 1).arg(measures).arg(measures - empty);
    }
    return { true, done + note + "\n\n" + summary.text };
}

//! where the audio engine is (the folder KitNote's Preferences > AI installs it into as well)
QString engineRoot()
{
    // (tools/kitnote tries an installation elsewhere with KITNOTE_ENGINE_ROOT, so that the real engine stays)
    if (qEnvironmentVariableIsSet("KITNOTE_ENGINE_ROOT")) {
        return QDir::fromNativeSeparators(qEnvironmentVariable("KITNOTE_ENGINE_ROOT"));
    }
    return QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + "/KitNote/ai";
}

QString pythonForAudio()
{
    // the engine KitNote installs (Preferences > AI) - a Python of its own with the separation model
    const QString root = engineRoot();
#ifdef Q_OS_WIN
    const QString python = root + "/venv/Scripts/python.exe";
#else
    const QString python = root + "/venv/bin/python";
#endif
    // engine.json is written last by the installer: without it the engine is half installed
    return QFileInfo::exists(python) && QFileInfo::exists(root + "/engine.json") ? python : QString();
}

//! the progress token of the tool call being answered (params._meta.progressToken), if the client sent one
QJsonValue g_progressToken;

void send(const QJsonObject& message);

//! MCP notifications/progress: a client that asked for them shows how far a long call is
void sendProgress(int percent, const QString& message)
{
    if (g_progressToken.isUndefined() || g_progressToken.isNull()) {
        return;
    }
    send({ { "jsonrpc", "2.0" }, { "method", "notifications/progress" },
           { "params", QJsonObject { { "progressToken", g_progressToken }, { "progress", percent }, { "total", 100 },
                                     { "message", message } } } });
}

//! audio_engine: status, install (only with confirm: true - the user has agreed), remove
Result audioEngine(const QJsonObject& args)
{
    const QString action = args.value("action").toString("status");
    const QString root = engineRoot();
    const bool installed = !pythonForAudio().isEmpty();
    // 1: band analysis; 2: the beat model (Beat This!) and KitNote's drum model - an engine 1 can be updated
    int version = 0;
    QFile engineFile(root + "/engine.json");
    if (engineFile.open(QIODevice::ReadOnly)) {
        QByteArray text = engineFile.readAll();
        if (text.startsWith("\xEF\xBB\xBF")) {
            text.remove(0, 3);
        }
        version = QJsonDocument::fromJson(text).object().value("version").toInt(1);
    }
    const auto models = [&root]() {
        QStringList names;
        for (const QFileInfo& file : QDir(root + "/torch/hub/checkpoints").entryInfoList({ "*.th" }, QDir::Files)) {
            names << file.fileName();
        }
        return names;
    };

    if (action == "status") {
        return { true, QString(QJsonDocument(QJsonObject {
                { "installed", installed }, { "folder", QDir::toNativeSeparators(root) }, { "version", version },
                { "update_available", installed && version < 2 },
                // (htdemucs_ft, for accurate: true - four models, downloaded the first time they are used)
                { "accurate_model", models().filter(QRegularExpression("^(f7e0c4bc|d12395a8|92cfc3b6|04573f0d)-")).size() == 4 },
                { "note", installed && version < 2 ? "transcribe_audio can be used (engine 1). Engine 2 transcribes far better: "
                  "audio_engine install with confirm true updates it (about 100 MB) - ask the user first."
                  : installed ? "transcribe_audio can be used."
                  : "Not installed: audio_engine with action install and confirm true installs it (about 1 GB) - ask the user first." } })
                                     .toJson(QJsonDocument::Indented)) };
    }
    if (action != "install" && action != "remove") {
        return fail("action must be status, install or remove");
    }
    if (!args.value("confirm").toBool()) {
        return fail(action == "install"
                    ? "Ask the user first: this downloads and installs uv, Python 3.11, PyTorch (CPU), Demucs with its separation "
                    "model (by Meta, MIT licence) and Beat This! (beats and measures, MIT licence) - about 1.2 GB, some minutes - into "
                    + QDir::toNativeSeparators(root) + " only; nothing else on the computer changes. If they agree, call again with confirm: true."
                    : "Ask the user first: this deletes " + QDir::toNativeSeparators(root)
                    + " (the audio engine, about 1 GB). If they agree, call again with confirm: true.");
    }
    if (action == "install" && installed && version >= 2) {
        return { true, "The audio engine (engine 2) is installed already: transcribe_audio can be used." };
    }
    if (action == "remove" && !QFileInfo::exists(root)) {
        return { true, "The audio engine is not installed: nothing to remove." };
    }
    // one at a time (another AI tool, or a second call)
    QDir().mkpath(QFileInfo(root).absolutePath());
    QLockFile lock(QFileInfo(root).absolutePath() + "/ai-engine.lock");
    lock.setStaleLockTime(0);       // held for minutes: stale only when its process has gone
    if (!lock.tryLock(1000)) {
        return fail("the audio engine is being installed or removed already - try again in some minutes");
    }

    // Windows: PowerShell runs install_engine.ps1; a Mac: sh runs install_engine.sh (the same steps)
#ifdef Q_OS_WIN
    const QString installerProgram = "powershell";
    QStringList scriptArgs { "-NoProfile", "-ExecutionPolicy", "Bypass", "-File",
                             QDir::toNativeSeparators(shareDir("ai") + "/install_engine.ps1"), "-Root", QDir::toNativeSeparators(root) };
    if (action == "remove") {
        scriptArgs << "-Remove";
    }
#else
    const QString installerProgram = "/bin/sh";
    QStringList scriptArgs { shareDir("ai") + "/install_engine.sh", "--root", root };
    if (action == "remove") {
        scriptArgs << "--remove";
    }
#endif
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(installerProgram, scriptArgs);
    if (!process.waitForStarted(30000)) {
        return fail("the audio engine installer could not be started");
    }
    QByteArray log;
    QByteArray pending;
    QElapsedTimer clock;
    clock.start();
    while (process.state() != QProcess::NotRunning) {
        process.waitForReadyRead(500);
        const QByteArray chunk = process.readAll();
        log += chunk;
        pending += chunk;
        qsizetype end;
        while ((end = pending.indexOf('\n')) >= 0) {
            const QByteArray line = pending.left(end);
            pending.remove(0, end + 1);
            if (line.startsWith("PROGRESS ")) {
                const QString text = QString::fromLocal8Bit(line).trimmed();
                sendProgress(text.section(' ', 1, 1).toInt(), text.section(' ', 2));
            }
        }
        if (clock.elapsed() > 60LL * 60 * 1000) {
            process.kill();
            return fail("the installation did not finish within an hour");
        }
    }
    log += process.readAll();

    if (action == "remove") {
        return QFileInfo::exists(root) ? fail("the audio engine could not be removed completely: " + QDir::toNativeSeparators(root))
               : Result { true, "The audio engine is removed." };
    }
    if (pythonForAudio().isEmpty()) {
        QString why;
        // (PowerShell 5.1 writes in the code page of the system, not in UTF-8)
        for (const QString& line : QString::fromLocal8Bit(log).split('\n')) {
            if (line.startsWith("ERROR")) {
                why = line.mid(6).trimmed();
            }
        }
        return fail("the audio engine could not be installed" + (why.isEmpty() ? QString() : ": " + why)
                    + " (is the computer online?)");
    }
    return { true, "The audio engine is installed (" + QDir::toNativeSeparators(root) + "). transcribe_audio can be used now." };
}

//! the time signatures whose beat is a dotted quarter
const QStringList METERS_COMPOUND { "6/8", "12/8" };

Result transcribe(const QJsonObject& args)
{
    const QString audio = absolute(args.value("audio_path").toString());
    const QString output = absolute(args.value("output_path").toString());
    // (an empty path would be the base folder itself)
    if (args.value("audio_path").toString().trimmed().isEmpty() || args.value("output_path").toString().trimmed().isEmpty()) {
        return fail("audio_path and output_path are needed");
    }
    if (!QFileInfo(audio).isFile()) {
        return fail("no such audio file: " + audio);
    }
    if (QFileInfo(output).isDir()) {
        return fail(output + " is a folder, not a score file");
    }
    if (QFileInfo::exists(output) && !args.value("overwrite").toBool()) {
        return fail(output + " exists already (overwrite: true replaces it)");
    }
    const QString python = pythonForAudio();
    if (python.isEmpty()) {
        return fail("The audio engine is not installed. Tell the user what audio_engine with action \"install\" downloads "
                    "(Python, PyTorch and the Demucs separation model, about 1 GB, into " + QDir::toNativeSeparators(engineRoot())
                    + " only) and, if they agree, call it with confirm: true; then call transcribe_audio again.");
    }

    QTemporaryDir temp;
    const QString json = temp.filePath("drums.json");
    QStringList scriptArgs { shareDir("ai") + "/kitnote_audio.py", audio, "--json", json };
    if (args.value("separate").toBool(true) == false) {
        scriptArgs << "--no-separation";
    }
    if (args.contains("tempo")) {
        const double tempo = args.value("tempo").toVariant().toDouble();
        const bool compound = QStringList { "6/8", "12/8" }.contains(args.value("time_signature").toString());
        if (tempo < 30 || tempo > (compound ? 266 : 300)) {
            return fail(compound ? "tempo (dotted quarters a minute) must be between 30 and 266" : "tempo must be between 30 and 300");
        }
        scriptArgs << "--tempo" << QString::number(tempo);
    }
    const QString meter = args.value("time_signature").toString("auto");
    if (!QStringList { "auto", "4/4", "3/4", "2/4", "6/8", "12/8" }.contains(meter)) {
        return fail("time_signature must be auto, 4/4, 3/4, 2/4, 6/8 or 12/8");
    }
    scriptArgs << "--meter" << meter;
    if (args.value("accurate").toBool()) {
        scriptArgs << "--accurate";
    }
    if (args.value("ghost_notes").toBool()) {
        scriptArgs << "--ghost-notes";
    }
    if (args.contains("stems_folder") && args.value("separate").toBool(true)) {
        scriptArgs << "--stems" << QDir::toNativeSeparators(absolute(args.value("stems_folder").toString()));
    }

    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    QProcessEnvironment pythonEnv = QProcessEnvironment::systemEnvironment();
    pythonEnv.insert("PYTHONIOENCODING", "utf-8");
    process.setProcessEnvironment(pythonEnv);
    process.start(python, scriptArgs);
    if (!process.waitForStarted(30000)) {
        return fail("the audio engine could not be started");
    }

    // "PROGRESS <percent> <what>" lines of the script become progress notifications
    QByteArray log;
    QByteArray pending;     // a line not yet complete
    QElapsedTimer clock;
    clock.start();
    while (process.state() != QProcess::NotRunning) {
        process.waitForReadyRead(500);
        const QByteArray chunk = process.readAll();
        log += chunk;
        pending += chunk;
        qsizetype end;
        while ((end = pending.indexOf('\n')) >= 0) {
            const QByteArray line = pending.left(end);
            pending.remove(0, end + 1);
            if (line.startsWith("PROGRESS ")) {
                const QString text = QString::fromUtf8(line).trimmed();
                sendProgress(text.section(' ', 1, 1).toInt(), text.section(' ', 2));
            }
        }
        if (clock.elapsed() > 60LL * 60 * 1000) {
            process.kill();
            return fail("the audio engine did not finish within an hour");
        }
    }
    log += process.readAll();

    QFile file(json);
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0 || !file.open(QIODevice::ReadOnly)) {
        // the last line says what went wrong; a Python traceback before it says nothing to the user
        const QStringList lines = QString::fromUtf8(log).trimmed().split('\n');
        return fail("transcription failed: " + (lines.isEmpty() ? QString() : lines.constLast().trimmed()));
    }
    const QJsonObject result = QJsonDocument::fromJson(file.readAll()).object();
    file.close();
    if (result.value("strokes").toInt() == 0) {
        return fail("No drums were found in the recording.");
    }

    const QString tab = temp.filePath("tab.txt");
    QFile tabFile(tab);
    tabFile.open(QIODevice::WriteOnly);
    tabFile.write(result.value("tab").toString().toUtf8());
    tabFile.close();

    QStringList commands {
        "title:" + args.value("title").toString(QFileInfo(audio).completeBaseName()),
        "timesig:" + result.value("time_signature").toString("4/4"),
        QString("measures:%1").arg(std::max(1, result.value("measures").toInt())),
        QString("tempo:%1").arg(qRound(result.value("tempo").toDouble())),
        "tabpaste:" + tab,
    };
    if (args.value("repeat_signs").toBool()) {
        commands << "repeats";      // a measure (or 2- or 4-measure phrase) the same as the one before as a repeat sign
    }
    if (args.value("sections").toBool(true)) {
        // where the groove changes and stays changed: A, B, A ...
        for (const QJsonValue& value : result.value("sections").toArray()) {
            const QJsonObject section = value.toObject();
            commands << QString("@%1 section:%2").arg(section.value("measure").toInt()).arg(section.value("name").toString());
        }
    }
    // the song plays along with its score in KitNote, from where the notes say measure 1 is (last: after the notes)
    commands << "recording:" + QDir::toNativeSeparators(audio);
    // ... in time with it: every measure where it is in the recording (hidden tempo changes; an engine installed
    // before does not tell where the measures are)
    const QJsonArray times = result.value("measure_times").toArray();
    QString timing;
    if (times.size() >= 2) {
        const QString timesFile = temp.filePath("measure_times.json");
        QFile timesOut(timesFile);
        timesOut.open(QIODevice::WriteOnly);
        timesOut.write(QJsonDocument(times).toJson(QJsonDocument::Compact));
        timesOut.close();
        commands << "recordingtimes:" + QDir::toNativeSeparators(timesFile);
        timing = " The playback follows the recording's own timing measure by measure (hidden tempo changes: "
                 "read_score's hidden_tempo_changes), while the chart shows the tempo marks; changing the first tempo "
                 "mark scales them all.";
    }
    return withSummary(output, runKitNote(templateFor("standard"), output, commands),
                       QString("Transcribed %1 in %2 at %3 into %4 (%5 strokes found).")
                       .arg(QFileInfo(audio).fileName(), result.value("time_signature").toString("4/4"),
                            METERS_COMPOUND.contains(result.value("time_signature").toString())
                            ? QString("dotted quarter = %1").arg(qRound(result.value("beat_tempo").toDouble()))
                            : QString("%1 BPM").arg(qRound(result.value("tempo").toDouble())))
                       .arg(QDir::toNativeSeparators(output)).arg(result.value("strokes").toInt())
                       + (result.value("drum_model").isString() ? QString(" The strokes were found by KitNote's own drum model.")
                          : QString(" The strokes were found by analysing frequency bands (KitNote's drum model was not used)."))
                       + timing);
}

// --- the tools -------------------------------------------------------------------------------------

const char* TAB_HELP
    = "A drum tab: a line for every drum, a column for every step, '|' between measures. Labels: CC crash, RD ride, "
      "HH hi-hat, T1 high tom, T2 mid tom, SD snare, FT floor tom, BD bass drum, HF hi-hat pedal. Signs: x or o a "
      "stroke (on HH 'o' is the open hi-hat; on SD 'x' is a cross-stick), a capital (X, O) an accent, g a ghost note, "
      "- nothing. An accent belongs to the step: a capital on one hand line accents every hand stroke on that step (they "
      "are one chord), and read_score shows them all as capitals. 16 columns to a 4/4 measure are sixteenth notes, 8 eighth "
      "notes, 12 triplet eighths. Example of a measure of rock beat, one line per drum:\nHH|x-x-x-x-x-x-x-x-|\n"
      "SD|----o-------o---|\nBD|o-------o-------|";

QJsonObject schema(const QJsonObject& properties, const QStringList& required)
{
    return QJsonObject { { "type", "object" }, { "properties", properties }, { "required", QJsonArray::fromStringList(required) } };
}

QJsonObject prop(const char* type, const QString& description)
{
    return QJsonObject { { "type", type }, { "description", description } };
}

QJsonArray toolList()
{
    const QJsonObject path = prop("string", "The .kitnote score file: an absolute path is best (a relative one is taken from the "
                                          "folder the AI tool started KitNote's server in).");
    QJsonArray tools;

    auto add = [&tools](const char* name, const QString& description, const QJsonObject& input) {
        tools.append(QJsonObject { { "name", name }, { "description", description }, { "inputSchema", input } });
    };

    add("create_score", "Creates a new drum score (.kitnote) from a KitNote template: empty measures, ready for tabs and patterns. "
        "Returns the summary of the score.",
        schema({ { "path", path },
                   { "title", prop("string", "Title") }, { "composer", prop("string", "Composer or arranger") },
                   { "tempo", prop("integer", "Quarter notes a minute (in 6/8 a dotted quarter is one and a half: dotted quarter = 60 is 90)") },
                   { "time_signature", prop("string", "For example 4/4, 3/4, 6/8, 12/8") },
                   { "measures", prop("integer", "Number of measures: the length the user asked for (default 16)") },
                   { "kit", QJsonObject { { "type", "string" }, { "enum", QJsonArray { "standard", "4-piece", "large", "marching" } },
                         { "description", "The kit: standard drum kit (default), 4-piece, large, or marching battery" } } },
                   { "overwrite", prop("boolean", "Replace an existing file") } },
               { "path" }));
    add("read_score", "Reads a drum score: title, tempo, time signature, number of measures, sections, and the whole score as a "
        "drum tab, in which measure-repeat signs are written out as the measures they stand for (repeat_signs says where the "
        "signs are: first_measure, measure_count 1, 2 or 4). " + QString(TAB_HELP),
        schema({ { "path", path } }, { "path" }));
    add("write_drum_tab", "Writes drum tab text into a score, measure after measure, from start_measure on (measures are added at "
        "the end when the tab is longer). The measures it covers are replaced. Every line of a row covers the same measures "
        "(a silent measure as dashes, never left out), and an empty line goes only between rows: a tab that breaks this is "
        "refused, and nothing is written. " + QString(TAB_HELP),
        schema({ { "path", path }, { "tab", prop("string", "The drum tab text") },
                   { "start_measure", prop("integer", "The first measure to write (default 1)") } }, { "path", "tab" }));
    add("fill_pattern", "Fills measures with a ready-made groove, fill or rudiment (see list_patterns).",
        schema({ { "path", path }, { "pattern", prop("string", "The name of the pattern, e.g. rock-8, disco, tom-fill, paradiddle") },
                   { "first_measure", prop("integer", "First measure") }, { "last_measure", prop("integer", "Last measure") } },
               { "path", "pattern", "first_measure" }));
    add("list_patterns", "Lists the grooves, fills and rudiments fill_pattern knows.", schema({}, {}));
    add("clear_measures", "Empties measures (rests only).",
        schema({ { "path", path }, { "first_measure", prop("integer", "First measure") }, { "last_measure", prop("integer", "Last measure") } },
               { "path", "first_measure" }));
    add("add_section", "Starts a section of the chart (Intro, Verse, Chorus, Bridge ...) at a measure; the name is shown above it.",
        schema({ { "path", path }, { "measure", prop("integer", "The first measure of the section") }, { "name", prop("string", "Its name") } },
               { "path", "measure", "name" }));
    add("set_properties", "Changes title, composer, tempo, time signature (from the first measure), the number of measures, swing, or the recording "
        "KitNote plays along with the score. "
        "The tempo is the one at the start (measure 1); a tempo change later is edit_score \"@9 tempo:140\". "
        "A new time signature re-bars the music already written: set it before writing.",
        schema({ { "path", path }, { "title", prop("string", "Title") }, { "composer", prop("string", "Composer") },
                   { "tempo", prop("integer", "Quarter notes a minute at the start (in 6/8 a dotted quarter is one and a half: dotted quarter = 60 is 90)") }, { "time_signature", prop("string", "e.g. 3/4") },
                   { "measures", prop("integer", "Number of measures: added or taken away at the end") },
                   { "swing", QJsonObject { { "type", "string" }, { "enum", QJsonArray { "off", "8", "16" } }, { "description", "Swing of playback" } } },
                   { "recording", prop("string", "A recording (wav, flac, mp3, ogg) KitNote plays along with the score, to check it by ear; "
                                                 "where measure 1 begins in it is found from the notes. \"\" takes it away. A transcription has one already.") },
                   { "recording_offset", prop("number", "Seconds into the recording where measure 1 begins (read_score shows it as recording.offset_seconds)") },
                   { "recording_timing", prop("boolean", "false: the score no longer follows the timing of its recording (read_score's "
                                                         "tempo_follows_recording): its hidden tempo changes go, and it plays at its tempo marks") } },
               { "path" }));
    add("edit_score", QString::fromUtf8(
        "Applies KitNote editing commands in order (for what the other tools do not cover). A command is a string: the "
        "measures first as '@<first>-<last> ' (or '@<measure> '), then the command - \"@5-8 change:hihat-to-ride\"; without "
        "'@' it acts on the whole score. Commands: pattern:<name>, clear, section:<name>, fillsection:<pattern>, sticking-r "
        "(R L R L under the notes, starting with the right hand) or sticking-l (L R L R, starting with the left) - use one, "
        "not both: the second replaces the first; counting (1 e & a), repeats (a measure equal to the one before becomes a % "
        "sign, a phrase of 2 or 4 measures played again the 2- or 4-measure repeat sign), write-out (the signs back to notes), key (adds a drum key/legend), key-remove, change:hihat-to-ride, change:ride-to-hihat, "
        "change:swap-sticking, change:accent-beats, change:accent-offbeats, change:accent-clear, timesig:<n>/<d>. "
        "Tempo: tempo:<bpm> sets the tempo at the start; \"@9 tempo:140\" is a tempo change at measure 9 (one measure; it "
        "holds from there on). Repeat barlines (\xEB\x8F\x84\xEB\x8F\x8C\xEC\x9D\xB4\xED\x91\x9C): \"@5-8 repeat\" puts |: at 5 and :| at 8, "
        "\"@5-8 repeat:3\" plays them three times; endings: \"@8 volta:1\" (1st ending over measure 8), \"@9-10 volta:2\" "
        "(2nd ending) - write the repeat before its endings; \"@5-10 repeat-remove\" takes repeats and endings away. Whole "
        "score only (no '@'): title:<text>, composer:<text>, measures:<count>, swing-8, swing-16, swing-off. Example: "
        "[\"@1-4 pattern:rock-8\", \"@1-4 sticking-r\", \"@5-8 repeat\", \"@9 tempo:140\"]."),
        schema({ { "path", path }, { "commands", QJsonObject { { "type", "array" }, { "items", QJsonObject { { "type", "string" } } },
                                                               { "description", "The commands, e.g. [\"@1-4 pattern:rock-8\", \"@5 section:Chorus\", \"@5-8 repeat\"]" } } } },
               { "path", "commands" }));
    add("export_score", "Exports a score; the format follows the extension of output_path: pdf, png, svg, mp3, wav, ogg, flac, mid, "
        "musicxml, mxl, txt (drum tab), kitnote. Pictures of several pages are written as name-1.png, name-2.png ... "
        "An existing file is replaced only with overwrite: true.",
        schema({ { "path", path }, { "output_path", prop("string", "The file to write") },
                   { "overwrite", prop("boolean", "Replace an existing file") } }, { "path", "output_path" }));
    add("transcribe_audio", "Makes a drum score from a recording: the drums are separated from the other instruments (Demucs), their "
        "strokes found and sorted (bass drum, snare, hi-hat, cymbals, toms) and written on a grid of sixteenth notes at the tempo "
        "of the song. Needs the audio engine (see audio_engine). Takes about the length of the song or more.",
        schema({ { "audio_path", prop("string", "The recording: wav, flac, ogg, mp3 (m4a and others with ffmpeg)") },
                   { "output_path", prop("string", "The .kitnote score to write") }, { "title", prop("string", "Title (default: the file name)") },
                   { "separate", prop("boolean", "Separate the drums first (default true; false for a recording of drums alone)") },
                   { "tempo", prop("number", "The tempo of the beat, if known (otherwise it is estimated); in 6/8 and 12/8 the beat is a dotted quarter") },
                   { "accurate", prop("boolean", "A better separation (htdemucs_ft): four times slower, and about 320 MB more to download the first time - ask the user first") },
                   { "ghost_notes", prop("boolean", "Also write the weak snare strokes as ghost notes (default false: a chart usually leaves them out, and many are the noise of other drums)") },
                   { "time_signature", QJsonObject { { "type", "string" }, { "enum", QJsonArray { "auto", "4/4", "3/4", "2/4", "6/8", "12/8" } },
                       { "description", "The time signature of the song (default auto: found from the groove - 4/4, 3/4 or 6/8)" } } },
                   { "stems_folder", prop("string", "Also write the separated instruments there as drums.wav, bass.wav, vocals.wav, other.wav") },
                   { "repeat_signs", prop("boolean", "Write a measure that is the same as the one before as a % sign, and a 2- or 4-measure phrase played again as the 2- or 4-measure repeat sign (easier to read)") },
                   { "sections", prop("boolean", "Mark sections where the groove changes and stays changed, A, B, A ... (default true); rename them after listening") } },
               { "audio_path", "output_path" }));
    add("audio_engine", "The audio engine that transcribe_audio needs (Python, PyTorch, Demucs; about 1 GB, kept apart in a folder "
        "of KitNote's). action status: whether it is installed. action install / remove: ONLY after the user has agreed in the "
        "chat - tell them what it downloads and how big it is - with confirm: true. Installing takes some minutes.",
        schema({ { "action", QJsonObject { { "type", "string" }, { "enum", QJsonArray { "status", "install", "remove" } },
                                           { "description", "status (default), install or remove" } } },
                 { "confirm", prop("boolean", "true: the user has agreed to the install or removal") } }, {}));
    add("view_score", "Shows a page of the score as a picture (PNG), to check how it looks: the notation, the sections, the "
        "spacing. Use it after writing, before telling the user it is done.",
        schema({ { "path", path }, { "page", prop("integer", "The page, from 1 (default 1)") } }, { "path" }));
    add("open_in_kitnote", "Opens a score in the KitNote window, for the user to look at, play and edit.",
        schema({ { "path", path } }, { "path" }));
    return tools;
}

Result callTool(const QString& name, const QJsonObject& args)
{
    const QString path = args.value("path").toString();

    // a path names a file: an empty one would be the base folder itself
    static const QStringList WITHOUT_PATH { "list_patterns", "transcribe_audio", "audio_engine" };
    if (!WITHOUT_PATH.contains(name)) {
        if (path.trimmed().isEmpty()) {
            return fail("path is missing");
        }
        if (QFileInfo(absolute(path)).isDir()) {
            return fail(path + " is a folder, not a score file");
        }
    }

    if (name == "create_score") {
        if (path.isEmpty()) {
            return fail("path is missing");
        }
        if (QFileInfo::exists(absolute(path)) && !args.value("overwrite").toBool()) {
            return fail(path + " exists already (overwrite: true replaces it)");
        }
        if (!editable(path)) {
            return fail("a score is a .kitnote file");
        }
        QJsonObject properties = args;
        if (!properties.contains("measures")) {
            properties["measures"] = 16;      // (the description says so: the AI gives the length it was asked for)
        }
        if (!properties.contains("title")) {
            properties["title"] = QFileInfo(path).completeBaseName();
        }
        if (args.contains("kit") && !QStringList { "standard", "4-piece", "large", "marching" }.contains(args.value("kit").toString())) {
            return fail("kit must be standard, 4-piece, large or marching");
        }
        bool ok = true;
        QString problem = "measures must be 1 to 2000 and tempo 20 to 400, as numbers";
        const QStringList commands = propertyCommands(properties, &ok, &problem);
        if (!ok) {
            return fail(problem);
        }
        return withSummary(path, runKitNote(templateFor(args.value("kit").toString("standard")), path, commands),
                           "Created " + QDir::toNativeSeparators(absolute(path))
                           + ". Next: write the music with write_drum_tab (the drum tab text as `tab`) or fill_pattern.");
    }
    if (name == "read_score") {
        return readScore(path);
    }
    if (name == "list_patterns") {
        return { true,
                 "Grooves: rock-8, rock-8-push, rock-16, quarter, crash-rock-8, ride-8, half-time, four-floor, disco, motown, "
                 "bossa-nova, reggae (one drop), shuffle, triplet-feel, jazz-ride, six-eight (for 6/8)\n"
                 "Fills: snare-fill, tom-fill, half-bar-fill (two beats of groove, two of fill), triplet-fill\n"
                 "Rudiments (with sticking): single-stroke, double-stroke, paradiddle, stroke-triplets, double-paradiddle, "
                 "paradiddle-diddle, flams, flam-tap, flam-accent" };
    }
    if (name == "open_in_kitnote") {
        if (!QFileInfo::exists(absolute(path))) {
            return fail("no such score: " + path);
        }
        const bool started = QProcess::startDetached(kitNoteExe(), { QDir::toNativeSeparators(absolute(path)) });
        return started ? Result { true, "Opened in KitNote." } : fail("KitNote could not be started");
    }
    if (name == "transcribe_audio") {
        return transcribe(args);
    }
    if (name == "audio_engine") {
        return audioEngine(args);
    }

    // the rest edit a score that is there
    if (!QFileInfo::exists(absolute(path))) {
        return fail("no such score: " + path + " (create_score makes one)");
    }

    if (name == "view_score") {
        bool ok = true;
        const int page = number(args, "page", 1, &ok);
        if (!ok || page < 1) {
            return fail("page must be a number from 1");
        }
        QTemporaryDir pictures;
        // 80 dots an inch: the notation is clear, and the picture small enough to send
        const Result run = runKitNote(path, pictures.filePath("page.png"), {}, { "-r", "80" });
        if (!run.ok) {
            return run;
        }
        QFile picture(pictures.filePath(QString("page-%1.png").arg(page)));
        if (!picture.open(QIODevice::ReadOnly)) {
            const int pages = static_cast<int>(QDir(pictures.path()).entryList({ "page-*.png" }, QDir::Files).size());
            return fail(QString("the score has %1 page(s)").arg(pages));
        }
        Result result { true, QString("Page %1 of %2.").arg(page).arg(QFileInfo(path).fileName()) };
        result.png = picture.readAll();
        return result;
    }

    if (name == "export_score") {
        const QString output = args.value("output_path").toString();
        if (output.isEmpty()) {
            return fail("output_path is missing");
        }
        static const QStringList FORMATS { "pdf", "png", "svg", "mid", "midi", "musicxml", "mxl", "xml", "mp3", "wav", "ogg",
                                           "flac", "txt", "kitnote", "mscz", "mscx" };
        if (!FORMATS.contains(QFileInfo(output).suffix().toLower())) {
            return fail("output_path must end in one of: " + FORMATS.join(", "));
        }
        // (pictures of several pages are written as name-1.png, name-2.png ...)
        const QFileInfo target(absolute(output));
        if (target.isDir()) {
            return fail(output + " is a folder, not a file");
        }
        const QRegularExpression pageName("^" + QRegularExpression::escape(target.completeBaseName()) + "-\\d+\\."
                                          + QRegularExpression::escape(target.suffix()) + "$", QRegularExpression::CaseInsensitiveOption);
        const bool pagesThere = !QDir(target.absolutePath()).entryList({ "*-*." + target.suffix() }, QDir::Files).filter(pageName).isEmpty();
        if ((target.exists() || pagesThere) && !args.value("overwrite").toBool()) {
            return fail(output + " exists already (overwrite: true replaces it)");
        }
        const Result run = runKitNote(path, output, {});
        return run.ok ? Result { true, "Written:\n" + run.text } : run;
    }

    if (!editable(path)) {
        return fail("only .kitnote (or .mscz, .mscx) scores are changed in place; export_score writes other formats");
    }

    QStringList commands;
    QTemporaryDir temp;
    if (name == "write_drum_tab") {
        if (!args.value("tab").isString() || args.value("tab").toString().trimmed().isEmpty()) {
            return fail("tab is missing: drum tab text, a line for every drum (see the description of the tool)");
        }
        // a line with a label KitNote does not know would be passed over without a word
        static const QRegularExpression LINE(QStringLiteral("^\\s*([A-Za-z][A-Za-z0-9]{0,3})\\s*[|:]"));
        static const QStringList KNOWN { "BD", "B", "K", "KD", "KK", "BA", "B1", "BASS", "SD", "S", "SN", "SNR", "CS", "RS", "XS",
                                         "HH", "H", "HC", "CH", "OH", "HO", "HF", "FH", "PH", "RD", "R", "RC", "RI", "CC", "C", "CR",
                                         "CY", "C1", "C2", "T1", "HT", "T", "TT", "ST", "T2", "MT", "T3", "FT", "LT", "F", "FL" };
        QStringList unknown;
        static const QRegularExpression CELLS(QStringLiteral("^\\s*[A-Za-z][A-Za-z0-9]{0,3}\\s*[|:][\\s|:\\-._xXoOgGfFbBdD#*@()?]+$"));
        for (const QString& line : args.value("tab").toString().split('\n')) {
            const QRegularExpressionMatch match = LINE.match(line);
            // a line of words ("Note: open hi-hat") is no drum line; PC (other percussion) is passed over
            if (match.hasMatch() && CELLS.match(line.trimmed()).hasMatch() && line.contains('-')
                && !KNOWN.contains(match.captured(1).toUpper()) && match.captured(1).toUpper() != "PC"
                && !unknown.contains(match.captured(1))) {
                unknown << match.captured(1);
            }
        }
        if (!unknown.isEmpty()) {
            return fail("unknown drum label(s): " + unknown.join(", ") + ". Use CC (crash), RD (ride), HH (hi-hat), T1, T2 (toms), "
                        "SD (snare), FT (floor tom), BD (bass drum), HF (hi-hat pedal).");
        }
        const QString tab = temp.filePath("tab.txt");
        QFile file(tab);
        if (!file.open(QIODevice::WriteOnly)) {
            return fail("no temporary file");
        }
        file.write(args.value("tab").toString().toUtf8());
        file.close();
        bool ok = true;
        const int start = number(args, "start_measure", 1, &ok);
        if (!ok || start < 1) {
            return fail("start_measure must be a number of a measure, from 1");
        }
        commands << QString("@%1 tabpaste:%2").arg(start).arg(tab);
    } else if (name == "fill_pattern" || name == "clear_measures") {
        const QString measures = range(args);
        if (measures.isEmpty()) {
            return fail("first_measure must be a number of a measure, from 1");
        }
        bool ok = true;
        if (args.contains("last_measure") && number(args, "last_measure", 0, &ok) < number(args, "first_measure", 0, &ok)) {
            return fail("last_measure comes before first_measure");
        }
        static const QStringList PATTERN_NAMES { "rock-8", "rock-8-push", "rock-16", "quarter", "crash-rock-8", "ride-8", "half-time",
                                                 "four-floor", "disco", "motown", "bossa-nova", "reggae", "shuffle", "triplet-feel",
                                                 "jazz-ride", "six-eight", "snare-fill", "tom-fill", "half-bar-fill", "triplet-fill",
                                                 "single-stroke", "double-stroke", "paradiddle", "stroke-triplets", "double-paradiddle",
                                                 "paradiddle-diddle", "flams", "flam-tap", "flam-accent" };
        if (name == "fill_pattern" && !PATTERN_NAMES.contains(args.value("pattern").toString().trimmed().toLower())) {
            return fail("unknown pattern \"" + args.value("pattern").toString() + "\": " + PATTERN_NAMES.join(", "));
        }
        commands << measures + (name == "fill_pattern" ? "pattern:" + args.value("pattern").toString().trimmed().toLower() : QString("clear"));
    } else if (name == "add_section") {
        bool ok = true;
        if (number(args, "measure", 0, &ok) < 1 || !ok) {
            return fail("measure must be the number of a measure, from 1");
        }
        if (args.value("name").toString().trimmed().isEmpty()) {
            return fail("name is missing (Intro, Verse, Chorus ...)");
        }
        bool measureOk = true;
        commands << QString("@%1 section:%2").arg(number(args, "measure", 1, &measureOk)).arg(args.value("name").toString().trimmed());
    } else if (name == "set_properties") {
        bool ok = true;
        QString problem = "measures must be 1 to 2000 and tempo 20 to 400, as numbers";
        commands = propertyCommands(args, &ok, &problem);
        if (!ok) {
            return fail(problem);
        }
        if (commands.isEmpty()) {
            return fail("nothing to change: give title, composer, tempo, time_signature, measures, swing, recording or recording_timing");
        }
    } else if (name == "edit_score") {
        if (!args.value("commands").isArray() || args.value("commands").toArray().isEmpty()) {
            return fail("commands is missing: a list of command texts");
        }
        for (const QJsonValue& value : args.value("commands").toArray()) {
            if (!value.isString()) {
                return fail("every command is a text, e.g. \"@1-4 pattern:rock-8\"");
            }
            if (!allowedCommand(value.toString())) {
                return fail("this command is not available here: " + value.toString().left(80));
            }
            commands << value.toString();
        }
    } else {
        return fail("unknown tool " + name);
    }

    return withSummary(path, runKitNote(path, path, commands), "Done.");
}


// --- prompts: ready requests, offered by the AI tools as commands (in Claude Code: /mcp__kitnote__...) ----

struct PromptArgument {
    const char* name;
    const char* description;
    bool required;
};

struct Prompt {
    const char* name;
    const char* description;
    std::vector<PromptArgument> arguments;
    const char* text;       // %1, %2 ... are the arguments in order; an empty one says "(not given)"
};

const std::vector<Prompt>& prompts()
{
    static const std::vector<Prompt> PROMPTS {
        { "drum_chart", "Write a drum chart for a song: its form, grooves and fills",
          { { "style", "The style, e.g. rock, funk, shuffle, bossa nova, metal", true },
            { "tempo", "Quarter notes a minute (dotted quarter = 60 in 6/8 is 90)", false },
            { "form", "The sections and their lengths, e.g. intro 4, verse 8, chorus 8, bridge 4, outro 4", false },
            { "path", "Where to save the .kitnote file", false } },
          "Write a drum chart with the kitnote tools. Style: %1. Tempo: %2. Form: %3. File: %4.\n"
          "Choose what is not given (a tempo usual for the style; a common song form; a file in the current folder named "
          "after the title). Create the score with enough measures, mark each section with add_section, write the grooves "
          "with write_drum_tab (vary them between sections: the chorus opens up, e.g. ride or open hi-hat), end each section "
          "with a fill and start the next with a crash. Check with read_score, export a PDF next to it, and say what you wrote." },
        { "groove_exercise", "Practice exercises: a groove or rudiment that changes step by step",
          { { "topic", "What to practise, e.g. ghost notes, paradiddles, 16th-note hi-hat, linear grooves", true },
            { "measures", "How many measures", false },
            { "path", "Where to save the .kitnote file", false } },
          "Write a practice sheet with the kitnote tools on: %1. Measures: %2. File: %3.\n"
          "Begin simply and add one thing at a time, two or four measures for each step, with section names that say what "
          "is new. Add sticking (edit_score sticking-r) where the hands matter and counting where the rhythm is new. "
          "Export a PDF next to it and list the steps." },
        { "transcribe_song", "Turn a recording into a drum score and tidy it up",
          { { "audio_path", "The recording (wav, mp3, flac, ogg)", true },
            { "time_signature", "4/4, 3/4, 2/4, 6/8 or 12/8 (default: found from the recording)", false },
            { "output_path", "Where to save the .kitnote file", false } },
          "Transcribe the drums of %1 with transcribe_audio (time signature %2, output %3; next to the recording when not "
          "given). If the audio engine is not installed (audio_engine), ask me before installing it. Then read the score back with read_score, mark the sections where the music clearly changes, and point out "
          "what looks doubtful (an automatic transcription is a starting point). Do not invent what is not in the recording." },
        { "change_score", "Change a score: what to change, in words",
          { { "path", "The .kitnote score", true },
            { "change", "What to change, e.g. put the chorus on the ride, add a fill before every section", true } },
          "Change the drum score %1 with the kitnote tools: %2.\n"
          "Read it first with read_score, change only what is asked, keep everything else, check the result with read_score "
          "and say in a few sentences what you changed." },
    };
    return PROMPTS;
}

QJsonArray promptList()
{
    QJsonArray list;
    for (const Prompt& prompt : prompts()) {
        QJsonArray arguments;
        for (const PromptArgument& argument : prompt.arguments) {
            arguments.append(QJsonObject { { "name", argument.name }, { "description", argument.description },
                                           { "required", argument.required } });
        }
        list.append(QJsonObject { { "name", prompt.name }, { "description", prompt.description }, { "arguments", arguments } });
    }
    return list;
}

//! the text of a prompt with its arguments in it; empty when there is no such prompt
QString promptText(const QString& name, const QJsonObject& values, QString* description)
{
    for (const Prompt& prompt : prompts()) {
        if (name != prompt.name) {
            continue;
        }
        const QString pattern = QString::fromUtf8(prompt.text);
        QString text;
        for (qsizetype i = 0; i < pattern.size(); ++i) {
            const int n = i + 1 < pattern.size() && pattern.at(i) == '%' ? pattern.at(i + 1).digitValue() : -1;
            if (n >= 1 && n <= static_cast<int>(prompt.arguments.size())) {
                const QString value = values.value(prompt.arguments[n - 1].name).toString().trimmed();
                text += value.isEmpty() ? QString("(not given)") : value;
                ++i;
            } else {
                text += pattern.at(i);
            }
        }
        *description = QString::fromUtf8(prompt.description);
        return text;
    }
    return QString();
}

// --- JSON-RPC over stdio ---------------------------------------------------------------------------

void send(const QJsonObject& message)
{
    const QByteArray line = QJsonDocument(message).toJson(QJsonDocument::Compact) + '\n';
    std::fwrite(line.constData(), 1, static_cast<size_t>(line.size()), stdout);
    std::fflush(stdout);
}

void reply(const QJsonValue& id, const QJsonObject& result)
{
    send({ { "jsonrpc", "2.0" }, { "id", id }, { "result", result } });
}

//! the protocol version agreed on at initialize
QString g_protocol;

//! The files a successful call wrote, as MCP resource links (protocol 2025-06-18 on): a client can offer
//! them to open or download. A score that was only read, and the source of an export, are not among them.
QJsonArray writtenFiles(const QString& tool, const QJsonObject& args)
{
    static const QStringList READING { "read_score", "view_score", "list_patterns", "open_in_kitnote", "audio_engine" };
    static const QMap<QString, QString> TYPES {
        { "pdf", "application/pdf" }, { "png", "image/png" }, { "svg", "image/svg+xml" }, { "mid", "audio/midi" },
        { "midi", "audio/midi" }, { "mp3", "audio/mpeg" }, { "wav", "audio/wav" }, { "ogg", "audio/ogg" }, { "flac", "audio/flac" },
        { "txt", "text/plain" }, { "musicxml", "application/vnd.recordare.musicxml+xml" }, { "mxl", "application/vnd.recordare.musicxml" },
    };
    QJsonArray links;
    if (READING.contains(tool)) {
        return links;
    }
    QStringList files;
    for (const char* key : { "path", "output_path" }) {
        if (!args.contains(key) || (tool == "export_score" && QString(key) == "path")) {
            continue;
        }
        const QFileInfo file(absolute(args.value(key).toString()));
        if (file.exists()) {
            files << file.absoluteFilePath();
        } else {
            // pictures of several pages: name-1.png, name-2.png ...
            const QDir dir = file.absoluteDir();
            for (const QString& page : dir.entryList({ file.completeBaseName() + "-*." + file.suffix() }, QDir::Files, QDir::Name)) {
                files << dir.absoluteFilePath(page);
            }
        }
    }
    for (const QString& path : files) {
        QJsonObject link { { "type", "resource_link" }, { "uri", QUrl::fromLocalFile(path).toString() },
                           { "name", QFileInfo(path).fileName() } };
        const QString type = TYPES.value(QFileInfo(path).suffix().toLower());
        if (!type.isEmpty()) {
            link["mimeType"] = type;
        }
        links.append(link);
    }
    return links;
}

void replyError(const QJsonValue& id, int code, const QString& message)
{
    send({ { "jsonrpc", "2.0" }, { "id", id }, { "error", QJsonObject { { "code", code }, { "message", message } } } });
}

const char* INSTRUCTIONS
    = "Load the kitnote skill first if your client has skills. KitNote writes drum scores. Make a score with create_score, write the music as drum tab text with write_drum_tab "
      "(or fill_pattern for ready-made grooves), mark the form with add_section, check it with read_score, then export_score "
      "to PDF, MIDI, MusicXML or audio, or open_in_kitnote for the user. transcribe_audio turns a recording into a score (audio_engine installs what it "
      "needs, after the user has agreed).";
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("kitnote-mcp");
    QCoreApplication::setApplicationVersion(VERSION);

    if (app.arguments().contains("--version")) {
        std::printf("kitnote-mcp %s\n", VERSION);
        return 0;
    }

#ifdef Q_OS_WIN
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty() || line == "\r") {
            continue;
        }

        QJsonParseError error;
        const QJsonDocument document = QJsonDocument::fromJson(QByteArray::fromStdString(line), &error);
        if (error.error != QJsonParseError::NoError) {
            replyError(QJsonValue(), -32700, "parse error");
            continue;
        }
        if (!document.isObject() || (document.object().contains("id") && !document.object().contains("method"))) {
            if (document.isObject() && document.object().contains("result")) {
                continue;       // an answer to a request of ours: there are none
            }
            replyError(document.isObject() ? document.object().value("id") : QJsonValue(), -32600, "invalid request");
            continue;
        }

        const QJsonObject message = document.object();
        const QString method = message.value("method").toString();
        const QJsonValue id = message.value("id");
        const bool isRequest = message.contains("id");

        if (method == "initialize") {
            // the version the client asks for, when it is one this server knows
            static const QStringList KNOWN { "2024-11-05", "2025-03-26", "2025-06-18" };
            const QString asked = message.value("params").toObject().value("protocolVersion").toString();
            const QString version = KNOWN.contains(asked) ? asked : KNOWN.last();
            g_protocol = version;
            reply(id, { { "protocolVersion", version },
                        { "capabilities", QJsonObject { { "tools", QJsonObject {} }, { "prompts", QJsonObject {} } } },
                        { "serverInfo", QJsonObject { { "name", "kitnote" }, { "version", VERSION } } },
                        { "instructions", QString(INSTRUCTIONS) + " Relative paths are in "
                          + QDir::toNativeSeparators(baseFolder()) + "; tell the user where the files are." } });
        } else if (method == "tools/list") {
            reply(id, { { "tools", toolList() } });
        } else if (method == "tools/call") {
            const QJsonObject params = message.value("params").toObject();
            static const QStringList TOOLS { "create_score", "read_score", "view_score", "write_drum_tab", "fill_pattern", "list_patterns",
                                             "clear_measures", "add_section", "set_properties", "edit_score", "export_score",
                                             "transcribe_audio", "audio_engine", "open_in_kitnote" };
            if (!TOOLS.contains(params.value("name").toString())) {
                replyError(id, -32602, "unknown tool: " + params.value("name").toString());
                continue;
            }
            g_progressToken = params.value("_meta").toObject().value("progressToken");
            const Result result = callTool(params.value("name").toString(), params.value("arguments").toObject());
            g_progressToken = QJsonValue();
            QJsonArray content { QJsonObject { { "type", "text" }, { "text", result.text } } };
            if (!result.png.isEmpty()) {
                content.append(QJsonObject { { "type", "image" }, { "mimeType", "image/png" },
                                             { "data", QString::fromLatin1(result.png.toBase64()) } });
            }
            if (result.ok && g_protocol >= "2025-06-18") {
                for (const QJsonValue& link : writtenFiles(params.value("name").toString(), params.value("arguments").toObject())) {
                    content.append(link);
                }
            }
            reply(id, { { "content", content }, { "isError", !result.ok } });
        } else if (method == "prompts/list") {
            reply(id, { { "prompts", promptList() } });
        } else if (method == "prompts/get") {
            const QJsonObject params = message.value("params").toObject();
            QString description;
            bool missing = false;
            for (const Prompt& prompt : prompts()) {
                if (params.value("name").toString() == prompt.name) {
                    for (const PromptArgument& argument : prompt.arguments) {
                        missing = missing || (argument.required
                                              && params.value("arguments").toObject().value(argument.name).toString().trimmed().isEmpty());
                    }
                }
            }
            if (missing) {
                replyError(id, -32602, "a required argument of the prompt is missing");
                continue;
            }
            const QString text = promptText(params.value("name").toString(), params.value("arguments").toObject(), &description);
            if (text.isEmpty()) {
                replyError(id, -32602, "unknown prompt: " + params.value("name").toString());
                continue;
            }
            reply(id, { { "description", description },
                        { "messages", QJsonArray { QJsonObject { { "role", "user" },
                                                                 { "content", QJsonObject { { "type", "text" }, { "text", text } } } } } } });
        } else if (method == "ping") {
            reply(id, {});
        } else if (isRequest) {
            replyError(id, -32601, "method not found: " + method);
        }
        // notifications (notifications/initialized, cancelled ...) need no answer
    }

    return 0;
}
