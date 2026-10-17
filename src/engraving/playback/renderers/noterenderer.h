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

#include "playback/renderingcontext.h"

namespace kn::engraving {
class Note;
class NoteRenderer
{
public:
    static bool shouldRender(const Note* note, const RenderingContext& ctx, const kfw::mpe::ArticulationMap& articulations);
    static void render(const Note* note, const RenderingContext& ctx, kfw::mpe::PlaybackEventList& result);

private:
    static void renderPartialTie(const Note* outgoingNote, NominalNoteCtx& outgoingNoteCtx);
    static void renderNormalTie(const Note* firstNote, NominalNoteCtx& firstNoteCtx);
    static void addTiedNote(const NominalNoteCtx& tiedNoteCtx, NominalNoteCtx& firstNoteCtx);
    static void updateArticulationBoundaries(const kfw::mpe::timestamp_t noteTimestamp, const kfw::mpe::duration_t noteDuration,
                                             kfw::mpe::ArticulationMap& articulations);
    static void applySwingIfNeed(const Note* note, NominalNoteCtx& noteCtx);

    static NominalNoteCtx buildNominalNoteCtx(const Note* note, const RenderingContext& ctx);
};
}
