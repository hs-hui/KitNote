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

#include "inotationundostack.h"
#include "igetscore.h"

namespace kn::engraving {
class Score;
class MasterScore;
class UndoStack;
class EditData;
}

namespace kn::notation {
class NotationUndoStack : public INotationUndoStack
{
public:
    NotationUndoStack(IGetScore* getScore, kfw::async::Notification notationChanged);

    bool canUndo() const override;
    void undo(kn::engraving::EditData*) override;

    bool canRedo() const override;
    void redo(kn::engraving::EditData*) override;

    void undoRedoToIndex(size_t idx, kn::engraving::EditData* editData) override;

    void prepareChanges(const kfw::TranslatableString& actionName) override;
    void rollbackChanges() override;
    void commitChanges() override;

    bool isStackClean() const override;

    void mergeCommands(size_t startIdx) override;

    void lock() override;
    void unlock() override;
    bool isLocked() const override;

    const kfw::TranslatableString topMostUndoActionName() const override;
    const kfw::TranslatableString topMostRedoActionName() const override;
    size_t undoRedoActionCount() const override;
    size_t currentStateIndex() const override;
    const kfw::TranslatableString lastActionNameAtIdx(size_t idx) const override;

    kfw::async::Notification stackChanged() const override;
    kfw::async::Channel<ScoreChanges> changesChannel() const override;
    kfw::async::Notification undoRedoNotification() const override;

private:
    void notifyAboutNotationChanged();
    void notifyAboutStateChanged();
    void notifyAboutUndoRedo();

    kn::engraving::Score* score() const;
    kn::engraving::MasterScore* masterScore() const;
    kn::engraving::UndoStack* undoStack() const;

    IGetScore* m_getScore = nullptr;

    kfw::async::Notification m_notationChanged;
    kfw::async::Notification m_stackStateChanged;
    kfw::async::Notification m_undoRedoNotification;
};
}
