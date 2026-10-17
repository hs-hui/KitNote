/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "notation/inotationconfiguration.h"

#include "pianokeyboardtypes.h"

namespace kn::notation {
class PianoKeyboardController : public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<INotationConfiguration> notationConfiguration;
    kfw::ContextInject<context::IGlobalContext> context = { this };

public:
    PianoKeyboardController(const kfw::modularity::ContextPtr& iocCtx);

    std::optional<piano_key_t> pressedKey() const;
    void setPressedKey(std::optional<piano_key_t> key);
    void setHoveredKey(std::optional<piano_key_t> key);

    KeyState keyState(piano_key_t key) const;
    kfw::async::Notification keyStatesChanged() const;

    bool isFromMidi() const;

private:
    INotationPtr currentNotation() const;

    void onNotationChanged();
    void updateNotesKeys(const std::vector<const Note*>& receivedNotes);

    void sendNoteOn(piano_key_t key);
    void sendNoteOff(piano_key_t key);

    std::optional<piano_key_t> m_pressedKey = std::nullopt;
    std::optional<piano_key_t> m_hoveredKey = std::nullopt;
    std::unordered_set<piano_key_t> m_keys;
    std::unordered_set<piano_key_t> m_otherNotesInChord;

    bool m_isFromMidi = false;

    kfw::async::Notification m_keyStatesChanged;
};
}
