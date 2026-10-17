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
#ifndef IMPORTMIDI_BEAT_H
#define IMPORTMIDI_BEAT_H

#include <set>
#include <map>

namespace kn::engraving {
class TimeSigMap;
}

namespace kn::iex::midi {
class ReducedFraction;
class MidiChord;
class MTrack;

namespace MidiBeat {
void removeEvery2ndBeat(std::set<ReducedFraction>& beatSet);
void findBeatLocations(
    const std::multimap<ReducedFraction, MidiChord>& allChords, engraving::TimeSigMap* sigmap, double ticksPerSec);

void adjustChordsToBeats(std::multimap<int, MTrack>& tracks);
void setTimeSignature(engraving::TimeSigMap* sigmap);
} // namespace MidiBeat
} // namespace kn::iex::midi

#endif // IMPORTMIDI_BEAT_H
