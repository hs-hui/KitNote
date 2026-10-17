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

#include "async/notification.h"
#include "async/channel.h"

#include "../notationtypes.h"

namespace kn::engraving {
class EditData;
enum class ElementType : unsigned char;
}

namespace kn::notation {
class INotationUndoStack
{
public:
    virtual ~INotationUndoStack() = default;

    virtual bool canUndo() const = 0;
    virtual void undo(kn::engraving::EditData*) = 0;

    virtual bool canRedo() const = 0;
    virtual void redo(kn::engraving::EditData*) = 0;

    virtual void undoRedoToIndex(size_t, kn::engraving::EditData*) = 0;

    virtual void prepareChanges(const kfw::TranslatableString&) = 0;
    virtual void rollbackChanges() = 0;
    virtual void commitChanges() = 0;

    virtual void mergeCommands(const size_t startIdx) = 0;

    virtual bool isStackClean() const = 0;

    virtual void lock() = 0;
    virtual void unlock() = 0;
    virtual bool isLocked() const = 0;

    virtual const kfw::TranslatableString topMostUndoActionName() const = 0;
    virtual const kfw::TranslatableString topMostRedoActionName() const = 0;
    virtual size_t undoRedoActionCount() const = 0;
    virtual size_t currentStateIndex() const = 0;
    virtual const kfw::TranslatableString lastActionNameAtIdx(size_t) const = 0;

    virtual kfw::async::Notification stackChanged() const = 0;
    virtual kfw::async::Channel<ScoreChanges> changesChannel() const = 0;
    virtual kfw::async::Notification undoRedoNotification() const = 0;
};

using INotationUndoStackPtr = std::shared_ptr<INotationUndoStack>;
}
