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

#include "notationundostack.h"

#include "log.h"

#include "engraving/dom/masterscore.h"
#include "engraving/editing/undo.h"

using namespace kn::notation;
using namespace kfw::async;

NotationUndoStack::NotationUndoStack(IGetScore* getScore, Notification notationChanged)
    : m_getScore(getScore), m_notationChanged(notationChanged)
{
}

bool NotationUndoStack::canUndo() const
{
    IF_ASSERT_FAILED(undoStack()) {
        return false;
    }

    return undoStack()->canUndo();
}

void NotationUndoStack::undo(kn::engraving::EditData* editData)
{
    IF_ASSERT_FAILED(score()) {
        return;
    }

    score()->undoRedo(true, editData);

    notifyAboutNotationChanged();
    notifyAboutUndoRedo();
    notifyAboutStateChanged();
}

bool NotationUndoStack::canRedo() const
{
    IF_ASSERT_FAILED(undoStack()) {
        return false;
    }

    return undoStack()->canRedo();
}

void NotationUndoStack::redo(kn::engraving::EditData* editData)
{
    IF_ASSERT_FAILED(score()) {
        return;
    }

    score()->undoRedo(false, editData);

    notifyAboutNotationChanged();
    notifyAboutUndoRedo();
    notifyAboutStateChanged();
}

void NotationUndoStack::undoRedoToIndex(size_t idx, kn::engraving::EditData* editData)
{
    auto stack = undoStack();

    IF_ASSERT_FAILED(stack) {
        return;
    }

    if (stack->currentIndex() == idx) {
        return;
    }

    while (stack->currentIndex() > idx && stack->canUndo()) {
        score()->undoRedo(true, editData);
    }
    while (stack->currentIndex() < idx && stack->canRedo()) {
        score()->undoRedo(false, editData);
    }

    notifyAboutNotationChanged();
    notifyAboutUndoRedo();
    notifyAboutStateChanged();
}

void NotationUndoStack::prepareChanges(const kfw::TranslatableString& actionName)
{
    IF_ASSERT_FAILED(score()) {
        return;
    }

    if (isLocked()) {
        return;
    }

    score()->startCmd(actionName);
}

void NotationUndoStack::rollbackChanges()
{
    IF_ASSERT_FAILED(score()) {
        return;
    }

    if (isLocked()) {
        return;
    }

    score()->endCmd(true);
}

void NotationUndoStack::commitChanges()
{
    IF_ASSERT_FAILED(score()) {
        return;
    }

    if (isLocked()) {
        return;
    }

    score()->endCmd();

    notifyAboutStateChanged();
}

bool NotationUndoStack::isStackClean() const
{
    IF_ASSERT_FAILED(undoStack()) {
        return false;
    }

    return undoStack()->isClean();
}

void NotationUndoStack::mergeCommands(size_t startIdx)
{
    IF_ASSERT_FAILED(undoStack()) {
        return;
    }

    undoStack()->mergeCommands(startIdx);
}

void NotationUndoStack::lock()
{
    IF_ASSERT_FAILED(undoStack()) {
        return;
    }

    undoStack()->setLocked(true);
}

void NotationUndoStack::unlock()
{
    IF_ASSERT_FAILED(undoStack()) {
        return;
    }

    undoStack()->setLocked(false);
}

bool NotationUndoStack::isLocked() const
{
    return undoStack()->isLocked();
}

const kfw::TranslatableString NotationUndoStack::topMostUndoActionName() const
{
    IF_ASSERT_FAILED(undoStack()) {
        return {};
    }

    if (auto action = undoStack()->last()) {
        return action->actionName();
    }

    return {};
}

const kfw::TranslatableString NotationUndoStack::topMostRedoActionName() const
{
    IF_ASSERT_FAILED(undoStack()) {
        return {};
    }

    if (auto action = undoStack()->next()) {
        return action->actionName();
    }

    return {};
}

size_t NotationUndoStack::undoRedoActionCount() const
{
    IF_ASSERT_FAILED(undoStack()) {
        return 0;
    }

    return undoStack()->size();
}

size_t NotationUndoStack::currentStateIndex() const
{
    IF_ASSERT_FAILED(undoStack()) {
        return kfw::nidx;
    }

    return undoStack()->currentIndex();
}

const kfw::TranslatableString NotationUndoStack::lastActionNameAtIdx(size_t idx) const
{
    IF_ASSERT_FAILED(undoStack()) {
        return {};
    }

    if (auto action = undoStack()->lastAtIndex(idx)) {
        return action->actionName();
    }

    return {};
}

kfw::async::Notification NotationUndoStack::stackChanged() const
{
    return m_stackStateChanged;
}

kfw::async::Channel<ScoreChanges> NotationUndoStack::changesChannel() const
{
    IF_ASSERT_FAILED(score()) {
        return kfw::async::Channel<ScoreChanges>();
    }

    return score()->changesChannel();
}

Notification NotationUndoStack::undoRedoNotification() const
{
    return m_undoRedoNotification;
}

kn::engraving::Score* NotationUndoStack::score() const
{
    return m_getScore->score();
}

kn::engraving::MasterScore* NotationUndoStack::masterScore() const
{
    return score() ? score()->masterScore() : nullptr;
}

kn::engraving::UndoStack* NotationUndoStack::undoStack() const
{
    return score() ? score()->undoStack() : nullptr;
}

void NotationUndoStack::notifyAboutNotationChanged()
{
    m_notationChanged.notify();
}

void NotationUndoStack::notifyAboutStateChanged()
{
    m_stackStateChanged.notify();
}

void NotationUndoStack::notifyAboutUndoRedo()
{
    m_undoRedoNotification.notify();
}
