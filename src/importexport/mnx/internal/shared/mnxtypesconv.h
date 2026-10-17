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

#include <optional>

#include "engraving/dom/chord.h"
#include "engraving/dom/ottava.h"
#include "engraving/dom/tuplet.h"
#include "engraving/types/types.h"

#ifdef MNXDOM_SYSTEM
#include <mnxdom/mnxdom.h>
#else
#include "mnxdom.h"
#endif

namespace kn::engraving {
struct Interval;
struct NoteVal;
class TDuration;
}

namespace kn::iex::mnxio {
// MNX values -> MuseScore values
extern engraving::BarLineType toKitNoteBarLineType(mnx::BarlineType blt);
extern engraving::BeamMode toKitNoteBeamMode(int lowestBeamStart);
extern engraving::BracketType toKitNoteBracketType(mnx::LayoutSymbol lys);
extern engraving::SymId toKitNoteBreathMarkSym(std::optional<mnx::BreathMarkSymbol> brSym);
extern engraving::ClefType toKitNoteClefType(const mnx::part::Clef& mnxClef);
extern engraving::DynamicType toKitNoteDynamicType(const engraving::String& glyph);
extern engraving::DurationType toKitNoteDurationType(mnx::NoteValueBase nvb);
extern engraving::TDuration toKitNoteDuration(mnx::NoteValue nv);
extern engraving::Fraction toKitNoteFraction(const mnx::FractionValue& fraction);
extern engraving::JumpType toKitNoteJumpType(mnx::JumpType jt);
extern engraving::Key toKitNoteKey(int fifths);
extern engraving::LyricsSyllabic toKitNoteLyricsSyllabic(mnx::LyricLineType llt);
extern engraving::NoteVal toKitNoteNoteVal(const mnx::sequence::Pitch::Required& pitch, engraving::Key key, int octaveShift);
extern engraving::OttavaType toKitNoteOttavaType(mnx::OttavaAmount ottavaAmount);
extern engraving::PreferSharpFlat toKitNotePreferSharpFlat(int keyFifthsFlipAt);
extern engraving::Fraction toKitNoteRTick(const mnx::RhythmicPosition& position);
extern engraving::SlurStyleType toKitNoteSlurStyleType(mnx::LineType lineType);
extern engraving::TremoloType toKitNoteTremoloType(int numberOfBeams);
extern engraving::TupletBracketType toKitNoteTupletBracketType(mnx::AutoYesNo bracketOption);
extern engraving::TupletNumberType toKitNoteTupletNumberType(mnx::TupletDisplaySetting numberStyle);

// MuseScore values -> MNX values
extern mnx::BarlineType toMnxBarLineType(engraving::BarLineType blt);
extern std::optional<mnx::BreathMarkSymbol> toMnxBreathMarkSym(engraving::SymId sym);
extern std::optional<mnx::part::Clef::Required> toMnxClef(engraving::ClefType clefType);
extern mnx::FractionValue toMnxFractionValue(const engraving::Fraction& fraction);
extern std::optional<mnx::JumpType> toMnxJumpType(engraving::JumpType jt);
extern int toMnxKeyFifthsFlipValue(engraving::PreferSharpFlat prefer, const engraving::Interval& keyTransposition);
extern mnx::LayoutSymbol toMnxLayoutSymbol(engraving::BracketType bracketType);
extern mnx::LyricLineType toMnxLyricLineType(engraving::LyricsSyllabic ls);
extern mnx::LineType toMnxSlurLineType(engraving::SlurStyleType sst);
extern std::optional<mnx::NoteValue::Required> toMnxNoteValue(const engraving::TDuration& duration);
extern std::optional<mnx::OttavaAmount> toMnxOttavaAmount(engraving::OttavaType ottavaType);
extern std::optional<mnx::sequence::Pitch::Required> toMnxPitch(const engraving::Note* note);
extern std::optional<mnx::TimeSignatureUnit> toMnxTimeSignatureUnit(int denominator);
extern std::optional<int> toMnxTremoloMarks(engraving::TremoloType tt);
extern mnx::AutoYesNo toMnxTupletBracketType(engraving::TupletBracketType bracketOption);
extern mnx::TupletDisplaySetting toMnxTupletNumberType(engraving::TupletNumberType numberStyle);

// MuseScore -> MuseScore
extern engraving::NoteType duraTypeToGraceNoteType(engraving::DurationType type, bool useLeft);

// utilities
extern std::string makeMnxVoiceIdFromTrack(int mnxPartStaffNum, engraving::track_idx_t curTrackIdx);
} // namespace kn::iex::musx
