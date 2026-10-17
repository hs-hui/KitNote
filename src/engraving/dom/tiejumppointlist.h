/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include "global/types/string.h"
#include "global/types/translatablestring.h"

namespace kn::engraving {
class Note;
class Tie;
class TieJumpPointList;

// Items of TieJumpPointList
// Stores information about an endpoint
class TieJumpPoint
{
public:
    TieJumpPoint(Note* note, bool active, int idx, bool followingNote);
    TieJumpPoint() {}

    Note* note() const { return m_note; }
    Tie* endTie() const;
    bool followingNote() const { return m_followingNote; }
    const kfw::String& id() const { return m_id; }
    bool active() const { return m_active; }
    void setActive(bool v) { m_active = v; }
    void undoSetActive(bool v);

    // Stores the list that this end point belongs to. This is needed to find the start tie from an endpoint
    void setJumpPointList(TieJumpPointList* jumpPointList) { m_jumpPointList = jumpPointList; }
    TieJumpPointList* jumpPointList() { return m_jumpPointList; }

    kfw::TranslatableString menuTitle() const;

private:
    kfw::String precedingJumpItemName() const;
    Note* m_note = nullptr;
    bool m_active = false;
    kfw::String m_id;
    TieJumpPointList* m_jumpPointList = nullptr;
    bool m_followingNote = false;
};

// Created & deleted by Note
// Manages adding and removing ties from endpoints
class TieJumpPointList
{
public:
    TieJumpPointList(Note* note)
        : m_note(note) {}
    ~TieJumpPointList();

    void add(TieJumpPoint* item);
    void clear();
    size_t size() const { return m_jumpPoints.size(); }
    bool empty() const { return m_jumpPoints.empty(); }

    Tie* startTie() const;

    TieJumpPoint* findJumpPoint(const kfw::String& id);
    void toggleJumpPoint(const kfw::String& id);

    void undoAddTieToScore(TieJumpPoint* jumpPoint);
    void undoRemoveTieFromScore(TieJumpPoint* jumpPoint);

    std::vector<TieJumpPoint*>::iterator begin() { return m_jumpPoints.begin(); }
    std::vector<TieJumpPoint*>::const_iterator begin() const { return m_jumpPoints.begin(); }
    std::vector<TieJumpPoint*>::iterator end() { return m_jumpPoints.end(); }
    std::vector<TieJumpPoint*>::const_iterator end() const { return m_jumpPoints.end(); }

private:
    std::vector<TieJumpPoint*> m_jumpPoints;
    Note* m_note = nullptr;
};
}
