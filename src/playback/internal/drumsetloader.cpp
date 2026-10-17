/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include "drumsetloader.h"

#include "notation/notationtypes.h"

using namespace kn::playback;
using namespace kfw::audio;
using namespace kn::notation;
using namespace kn::engraving;

void DrumsetLoader::loadDrumset(INotationPtr notation, const InstrumentTrackId& trackId, const AudioResourceMeta& /*resourceMeta*/)
{
    TRACEFUNC;

    if (!notation) {
        return;
    }

    // KitNote has no sampler with its own drumsets: every track gets the instrument's drumset
    const InstrumentTemplate& templ = instrumentsRepository()->instrumentTemplate(trackId.instrumentId);
    if (!templ.useDrumset) {
        return;
    }

    if (templ.drumset) {
        replaceDrumset(notation, trackId, *templ.drumset);
    } else if (kn::engraving::smDrumset) {
        replaceDrumset(notation, trackId, *kn::engraving::smDrumset);
    }
}

void DrumsetLoader::replaceDrumset(INotationPtr notation, const InstrumentTrackId& trackId, const Drumset& drumset)
{
    const Part* part = notation->parts()->part(trackId.partId);
    if (!part) {
        return;
    }

    QString instrumentId = trackId.instrumentId.toQString();
    const kn::engraving::InstrumentList& instruments = part->instruments();

    for (auto it = instruments.cbegin(); it != instruments.cend(); ++it) {
        if (it->second->id() != trackId.instrumentId) {
            continue;
        }

        InstrumentKey instrumentKey;
        instrumentKey.instrumentId = instrumentId;
        instrumentKey.partId = trackId.partId;
        instrumentKey.tick = Fraction::fromTicks(it->first);

        notation->parts()->replaceDrumset(instrumentKey, drumset, false /*undoable*/);
    }
}
