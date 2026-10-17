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

#pragma once

#include "async/channel.h"

#include "notationtypes.h"

namespace kfw::midi {
struct Event;
}

namespace kn::notation {
class INotationMidiInput
{
public:
    virtual ~INotationMidiInput() = default;

    virtual void onMidiEventReceived(const kfw::midi::Event& event) = 0;
    virtual kfw::async::Channel<std::vector<const Note*> > notesReceived() const = 0;

    virtual void onRealtimeAdvance() = 0;
};

using INotationMidiInputPtr = std::shared_ptr<INotationMidiInput>;
}
