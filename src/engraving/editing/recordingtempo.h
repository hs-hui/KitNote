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

#include <utility>
#include <vector>

#include "../types/types.h"

//! KitNote: the timing of a recording in a score. A song is not played at one exact tempo: the tempo found
//! is a little off, and a drummer drifts. A score made from a recording therefore follows it measure by
//! measure with hidden tempo marks (View > Show invisible shows them), so that every measure begins where it
//! begins in the recording, while the chart shows one tempo mark, rounded ("♩ = 94") - and a mark of its own
//! where the song clearly changes its tempo for a while (a slower bridge). The score keeps
//!     kitnoteRecordingTempo  the tempo of the first mark (quarters a minute) that the hidden marks go with
//! in its properties. What happens later is kept simple:
//!   - The first tempo mark changed (tap tempo, its text, Properties, set_properties tempo): the hidden marks
//!     and the later marks are scaled with it, in the same step of the undo history - the song's feel at the
//!     new speed. (The recording, never time-stretched, then no longer lines up: undo, or set it back.)
//!   - The recording taken away: the score may keep the timing (it still plays like the song) or drop it.
//!   - A hidden mark taken away or changed: the score plays as the marks say; nothing is put back.
namespace kn::engraving {
class EngravingItem;
class Score;

static const String RECORDING_TEMPO_TAG(u"kitnoteRecordingTempo");

//! a measure starts this far at most from where it starts in the recording before a hidden mark corrects it
//! (a drummer is a few milliseconds early or late in every measure: a tighter fit would put a mark on each)
constexpr double RECORDING_TEMPO_TOLERANCE = 0.010;

//! a passage this long whose tempo differs this much from the tempo marked gets a visible mark of its own
constexpr int RECORDING_SECTION_MEASURES = 4;
constexpr double RECORDING_SECTION_CHANGE = 0.05;

//! one of the hidden tempo marks of a recording's timing: hidden, of the plain kind (not "a tempo" or "tempo
//! primo", as a drum key's are), at the beginning of a measure
bool isRecordingTempoMark(const EngravingItem* item);

//! the score's playback follows the timing of a recording (the property is there)
bool tempoFollowsRecording(const Score* score);
int recordingTempoMarkCount(const Score* score);

struct RecordingTempo {
    int measures = 0;                           // the measures that follow the recording
    int hiddenMarks = 0;
    std::vector<std::pair<int, int> > marks;    // the visible tempo marks added: measure number, as written
    double tempo = 0.0;                         // of the first mark, quarters a minute
    double largestDrift = 0.0;                  // seconds: a measure's start from where it is in the recording
};

//! Inside an edit: the score's playback follows the recording whose measures begin at these times (seconds;
//! the first is where measure 1 begins, the last where the last measure ends). The first measure gets a
//! visible tempo mark when it has none; the hidden marks of an earlier call are replaced.
RecordingTempo followRecordingTimes(Score* score, const std::vector<double>& measureTimes);

//! Inside an edit: the hidden tempo marks taken away, and the property; how many marks there were.
int removeRecordingTempo(Score* score);

//! Inside an edit, at its end (Score::endCmd): when the first tempo mark is no longer at the tempo the hidden
//! marks go with, they and the later marks are scaled with it.
void keepRecordingTempoScaled(Score* score);
}
