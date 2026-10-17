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
#pragma once

#include "async/channel.h"
#include "io/iodevice.h"
#include "io/path.h"
#include "types/ret.h"

#include "engraving/types/types.h"

namespace kn::engraving {
class MscReader;
}

namespace kn::notation {
class INotationSoloMuteState
{
public:
    struct SoloMuteState {
        bool mute = false;
        bool solo = false;

        bool operator ==(const SoloMuteState& other) const
        {
            return mute == other.mute
                   && solo == other.solo;
        }
    };

    virtual ~INotationSoloMuteState() = default;

    virtual kfw::Ret read(const engraving::MscReader& reader, const kfw::io::path_t& pathPrefix = "") = 0;
    virtual kfw::Ret write(kfw::io::IODevice* out) = 0;

    virtual bool trackSoloMuteStateExists(const engraving::InstrumentTrackId& trackId) const = 0;
    virtual const SoloMuteState& trackSoloMuteState(const engraving::InstrumentTrackId& trackId) const = 0;
    virtual void setTrackSoloMuteState(const engraving::InstrumentTrackId& trackId, const SoloMuteState& state) = 0;
    virtual void removeTrackSoloMuteState(const engraving::InstrumentTrackId& trackId) = 0;
    virtual kfw::async::Channel<engraving::InstrumentTrackId, SoloMuteState> trackSoloMuteStateChanged() const = 0;
};

using INotationSoloMuteStatePtr = std::shared_ptr<INotationSoloMuteState>;
}
