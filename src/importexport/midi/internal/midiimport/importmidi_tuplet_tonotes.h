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
#ifndef IMPORTMIDI_TUPLET_TONOTES_H
#define IMPORTMIDI_TUPLET_TONOTES_H

#include <map>

namespace kn::engraving {
class DurationElement;
class Staff;
}

namespace kn::iex::midi {
class ReducedFraction;

namespace MidiTuplet {
struct TupletData;

void addElementToTuplet(int voice, const ReducedFraction& onTime, const ReducedFraction& len, engraving::DurationElement* el,
                        std::multimap<ReducedFraction, TupletData>& tuplets);

void createTupletNotes(engraving::Staff* staff, const std::multimap<ReducedFraction, TupletData>& tuplets);

#ifdef QT_DEBUG
bool haveTupletsEnoughElements(const engraving::Staff* staff);
#endif
} // namespace MidiTuplet
} // namespace kn::iex::midi

#endif // IMPORTMIDI_TUPLET_TONOTES_H
