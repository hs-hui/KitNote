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

#include <map>
#include <optional>
#include <utility>
#include <vector>

#include <QString>

#include "engraving/types/types.h"
#include "notation/inotationplayback.h"

namespace kn::engraving {
class Score;
}

//! KitNote: the recording of a score - the song a drum score was transcribed from, or any recording
//! attached to it - played in the Mixer's "Recording" channel along with the score. The score keeps it in
//! two of its properties (File > Project properties shows them):
//!     kitnoteRecording        the file, relative to the score's folder when it is in it, else absolute
//!     kitnoteRecordingOffset  where measure 1 begins in the recording, in seconds ("1.234"; may be < 0)
//! Measure 1 is the first measure numbered 1, so a drum key or a pickup before it does not move the
//! recording. The offset holds for playback at 100%: the recording is not time-stretched.
namespace kn::playback {
static const kfw::String RECORDING_TAG(u"kitnoteRecording");
static const kfw::String RECORDING_OFFSET_TAG(u"kitnoteRecordingOffset");

//! the Mixer channel of the recording, among the instruments' (as the metronome's)
const engraving::InstrumentTrackId& recordingTrackId();

struct ScoreRecording {
    QString path;           // as stored
    double offset = 0.0;    // seconds

    bool isSet() const { return !path.isEmpty(); }
    bool operator==(const ScoreRecording& other) const { return path == other.path && qFuzzyCompare(1.0 + offset, 1.0 + other.offset); }
    bool operator!=(const ScoreRecording& other) const { return !operator==(other); }
};

ScoreRecording scoreRecording(const engraving::Score* score);

//! the score's properties with this recording in them (without a path: without the recording)
std::map<kfw::String, kfw::String> metaTagsWithRecording(const engraving::Score* score, const ScoreRecording& recording);

//! the path to keep in the score: relative to the score's folder for a file in it (or below it)
QString recordingPathToStore(const QString& absolutePath, const QString& scoreFilePath);

//! the file the stored path stands for; when it is not there any more, a file of that name next to the
//! score (a folder that was moved, or a score sent with its recording). Empty when there is none.
QString resolveRecordingPath(const QString& storedPath, const QString& scoreFilePath);

//! where measure 1 begins in the playback of the score, in seconds at 100%
double measureOneSecs(const engraving::Score* score);

//! a note of the score as a recording sounds it: when it is played (seconds at 100%), and the band of the
//! spectrum its stroke stands out in - 0 low (bass drum), 1 middle (snare, toms), 2 high (hi-hat, cymbals)
struct RecordingNote {
    double time = 0.0;
    int band = 1;
};
using RecordingNotes = std::vector<RecordingNote>;
RecordingNotes scoreNotes(const notation::INotationPlaybackPtr& playback, double tempoMultiplier);

//! Finds where measure 1 begins in the recording. The rise of its spectrum (spectral flux) is measured in
//! the three bands and scaled by how it varies around (a z-score); every offset of the score, 5 ms apart,
//! gets the mean of it under its notes, each in the band of its drum - so a bass drum where only a hi-hat
//! is heard counts against an offset, and a groove shifted by a beat does not fit. The best offset is
//! then refined on the strokes heard near the notes of the first 20 seconds: a score whose tempo is a
//! little off the song's starts with it, and drifts later. The drums kept by the transcription
//! ("<song> stems/drums.wav") are used when they are there. Nothing when the recording cannot be read
//! or the score does not fit it.
std::optional<double> findRecordingOffset(const QString& audioPath, const RecordingNotes& notes, double measureOneSecs);
}
