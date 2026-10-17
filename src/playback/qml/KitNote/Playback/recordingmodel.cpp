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
#include "recordingmodel.h"

#include <thread>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QPointer>
#include <QUrl>

#include "engraving/dom/masterscore.h"
#include "engraving/editing/editscoreproperties.h"
#include "engraving/editing/recordingtempo.h"

#include "translation.h"
#include "log.h"

using namespace kn::playback;
using namespace kn::notation;

RecordingModel::RecordingModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void RecordingModel::load()
{
    controller()->recordingStateChanged().onNotify(this, [this]() {
        emit changed();
    });
    controller()->isPlayingChanged().onNotify(this, [this]() {
        emit playingChanged();
    });

    controller()->recheckRecording();     // KitNote: a file that was missing may be there now

    emit changed();
    emit playingChanged();
}

ScoreRecording RecordingModel::recording() const
{
    const IMasterNotationPtr master = context()->currentMasterNotation();
    return master ? scoreRecording(master->masterScore()) : ScoreRecording();
}

QString RecordingModel::scorePath() const
{
    const project::INotationProjectPtr project = context()->currentProject();
    return project && !project->isNewlyCreated() ? project->path().toQString() : QString();
}

bool RecordingModel::attached() const
{
    return recording().isSet();
}

QString RecordingModel::fileName() const
{
    return QFileInfo(recording().path).fileName();
}

QString RecordingModel::filePath() const
{
    const QString found = controller()->recordingState().filePath;
    return QDir::toNativeSeparators(found.isEmpty() ? recording().path : found);
}

double RecordingModel::offset() const
{
    return recording().offset;
}

bool RecordingModel::finding() const
{
    return m_finding;
}

bool RecordingModel::playing() const
{
    return controller()->isPlaying();
}

QString RecordingModel::status() const
{
    if (m_finding) {
        return kfw::qtrc("playback", "Finding where measure 1 begins in the recording…");
    }
    if (!attached()) {
        return QString();
    }
    const IPlaybackController::RecordingState& state = controller()->recordingState();
    if (state.failed) {
        return kfw::qtrc("playback", "The recording was not found, or it is not a WAV, FLAC, MP3 or OGG file.");
    }
    if (state.mutedBySpeed) {
        return kfw::qtrc("playback", "The recording is silent while the playback speed is not 100%: it is not time-stretched, so it would not stay with the score.");
    }
    if (m_message.isEmpty() && followsTiming()) {
        return kfw::qtrc("playback", "The score follows the timing of the recording: hidden tempo changes keep every measure with it.");
    }
    return m_message;
}

bool RecordingModel::warning() const
{
    const IPlaybackController::RecordingState& state = controller()->recordingState();
    return !m_finding && attached() && (state.failed || state.mutedBySpeed || m_messageIsWarning);
}

void RecordingModel::apply(const ScoreRecording& recording)
{
    const IMasterNotationPtr master = context()->currentMasterNotation();
    if (!master || recording == this->recording()) {
        return;
    }

    engraving::MasterScore* score = master->masterScore();
    master->notation()->undoStack()->prepareChanges(kfw::TranslatableString("undoableAction", "Change the recording"));
    score->undo(new engraving::ChangeMetaTags(score, metaTagsWithRecording(score, recording)));
    master->notation()->undoStack()->commitChanges();
    master->notation()->notationChanged().notify();

    emit changed();
}

void RecordingModel::chooseFile()
{
    // (tools/kitnote checks the dialog with a recording of its own: the file dialog of the system cannot be driven)
    if (qEnvironmentVariableIsSet("KITNOTE_TEST_AUDIO")) {
        setFile(qEnvironmentVariable("KITNOTE_TEST_AUDIO"));
        return;
    }

    const QString folder = scorePath().isEmpty() ? QDir::homePath() : QFileInfo(scorePath()).absolutePath();
    const kfw::io::path_t path = interactive()->selectOpeningFileSync(
        kfw::trc("playback", "Choose a recording"), kfw::io::path_t(folder),
        { kfw::trc("playback", "Recordings") + " (*.wav *.flac *.mp3 *.ogg)" });
    if (!path.empty()) {
        setFile(path.toQString());
    }
}

void RecordingModel::setFile(const QString& pathOrUrl)
{
    const QUrl url(pathOrUrl);
    const QString path = url.isLocalFile() ? url.toLocalFile() : pathOrUrl;
    if (path.isEmpty() || !QFileInfo(path).isFile()) {
        return;
    }

    m_message.clear();
    m_messageIsWarning = false;
    apply({ recordingPathToStore(path, scorePath()), 0.0 });

    // where the score begins in it: found straight away, then corrected by ear if need be
    findOffset();
}

void RecordingModel::setOffset(double seconds)
{
    const ScoreRecording current = recording();
    if (!current.isSet() || std::abs(current.offset - seconds) < 0.0005) {
        return;
    }
    m_message.clear();
    m_messageIsWarning = false;
    apply({ current.path, seconds });
}

void RecordingModel::findOffset()
{
    const IMasterNotationPtr master = context()->currentMasterNotation();
    const ScoreRecording current = recording();
    if (!master || !current.isSet() || m_finding) {
        return;
    }

    const QString file = resolveRecordingPath(current.path, scorePath());
    if (file.isEmpty()) {
        emit changed();
        return;
    }

    RecordingNotes notes = scoreNotes(master->playback(), controller()->tempoMultiplier());
    const double measureOne = measureOneSecs(master->masterScore());

    m_finding = true;
    emit changed();

    // a few seconds for a song: not on the interface's thread
    QPointer<RecordingModel> self(this);
    const QString path = current.path;
    std::thread([self, file, path, notes = std::move(notes), measureOne]() {
        const std::optional<double> offset = findRecordingOffset(file, notes, measureOne);
        QMetaObject::invokeMethod(qApp, [self, path, offset]() {
            if (self) {
                self->onOffsetFound(path, offset);
            }
        }, Qt::QueuedConnection);
    }).detach();
}

void RecordingModel::onOffsetFound(const QString& path, std::optional<double> offset)
{
    m_finding = false;
    if (recording().path != path) {
        emit changed();     // another recording was chosen meanwhile (an undo, the project properties)
        return;
    }
    if (offset) {
        m_message = kfw::qtrc("playback", "Measure 1 begins at %1 s in the recording. Listen, and move it if it is not quite right.")
                    .arg(QString::number(*offset, 'f', 3));
        m_messageIsWarning = false;
        const ScoreRecording current = recording();
        if (current.isSet()) {
            apply({ current.path, std::round(*offset * 1000.0) / 1000.0 });
        }
    } else {
        m_message = kfw::qtrc("playback", "KitNote could not find where the score fits the recording. Play, and set where measure 1 begins by ear.");
        m_messageIsWarning = true;
    }
    emit changed();
}

bool RecordingModel::followsTiming() const
{
    const IMasterNotationPtr master = context()->currentMasterNotation();
    return master && engraving::tempoFollowsRecording(master->masterScore());
}

void RecordingModel::remove()
{
    if (!followsTiming()) {
        removeRecording(false);
        return;
    }

    // The score follows the recording's timing (engraving/editing/recordingtempo.h): it may keep it, and still
    // play like the song, or play at its tempo marks alone.
    const int keep = int(kfw::IInteractive::Button::CustomButton) + 1;
    const int drop = keep + 1;
    interactive()->question(kfw::trc("playback", "Keep the timing of the recording?"),
                            kfw::trc("playback", "The score follows the timing of its recording with hidden tempo changes. "
                                                  "Without the recording it can keep them, and still play like the song, "
                                                  "or play at its tempo marks alone."),
                            { kfw::IInteractive::ButtonData(keep, kfw::trc("playback", "Keep timing"), true),
                              kfw::IInteractive::ButtonData(drop, kfw::trc("playback", "Remove timing")),
                              interactive()->buttonData(kfw::IInteractive::Button::Cancel) }, keep)
    .onResolve(this, [this, keep, drop](const kfw::IInteractive::Result& result) {
        if (result.isButton(keep) || result.isButton(drop)) {
            removeRecording(result.isButton(drop));
        }
    });
}

//! the recording taken away - and its timing, when asked - as one step of the undo history
void RecordingModel::removeRecording(bool timingToo)
{
    const IMasterNotationPtr master = context()->currentMasterNotation();
    if (!master) {
        return;
    }
    m_message.clear();
    m_messageIsWarning = false;

    engraving::MasterScore* score = master->masterScore();
    master->notation()->undoStack()->prepareChanges(kfw::TranslatableString("undoableAction", "Change the recording"));
    if (timingToo) {
        engraving::removeRecordingTempo(score);
    }
    score->undo(new engraving::ChangeMetaTags(score, metaTagsWithRecording(score, ScoreRecording())));
    master->notation()->undoStack()->commitChanges();
    master->notation()->notationChanged().notify();

    emit changed();
}

void RecordingModel::togglePlay()
{
    dispatcher()->dispatch("play");
}
