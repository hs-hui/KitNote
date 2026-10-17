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

#include <unordered_map>

#include "../inotationsolomutestate.h"
#include "async/asyncable.h"

namespace kn::notation {
class NotationSoloMuteState : public INotationSoloMuteState, public kfw::async::Asyncable
{
public:
    kfw::Ret read(const engraving::MscReader& reader, const kfw::io::path_t& pathPrefix = "") override;
    kfw::Ret write(kfw::io::IODevice* out) override;

    bool trackSoloMuteStateExists(const engraving::InstrumentTrackId& trackId) const override;
    const SoloMuteState& trackSoloMuteState(const engraving::InstrumentTrackId& trackId) const override;
    void setTrackSoloMuteState(const engraving::InstrumentTrackId& trackId, const SoloMuteState& state) override;
    void removeTrackSoloMuteState(const engraving::InstrumentTrackId& trackId) override;
    kfw::async::Channel<engraving::InstrumentTrackId, SoloMuteState> trackSoloMuteStateChanged() const override;

private:
    std::unordered_map<engraving::InstrumentTrackId, SoloMuteState> m_trackSoloMuteStatesMap;
    kfw::async::Channel<engraving::InstrumentTrackId, SoloMuteState> m_trackSoloMuteStateChanged;
};
}
