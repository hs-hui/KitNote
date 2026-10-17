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

#include "framework/global/types/string.h"
#include "serialization/xmlstreamreader.h"

namespace kn::engraving {
enum class AccidentalType : unsigned char;
class Articulation;
class Chord;
struct ChordDescription;
class Fraction;
class Harmony;
class HarmonyInfo;
class HDegree;
class Score;
enum class SymId;
enum class Key : signed char;
}

namespace kn::iex::musicxml {
//---------------------------------------------------------
//   MusicXmlSupport -- MusicXML import support functions
//---------------------------------------------------------

class MusicXmlSupport
{
public:
    static int stringToInt(const kfw::String& s, bool* ok);
    static engraving::Fraction noteTypeToFraction(const kfw::String& type);
    static engraving::Fraction calculateFraction(const kfw::String& type, int dots, int normalNotes, int actualNotes);
};

extern kfw::String accSymId2MusicXmlString(const engraving::SymId id);
extern kfw::String accSymId2SmuflMusicXmlString(const engraving::SymId id);
extern kfw::String accidentalType2MusicXmlString(const engraving::AccidentalType type);
extern kfw::String accidentalType2SmuflMusicXmlString(const engraving::AccidentalType type);
extern engraving::AccidentalType musicXmlString2accidentalType(const kfw::String mxmlName, const kfw::String smufl);
extern kfw::String musicXmlAccidentalTextToChar(const kfw::String mxmlName);
extern engraving::SymId musicXmlString2accSymId(const kfw::String mxmlName, const kfw::String smufl = {});
extern engraving::AccidentalType microtonalGuess(double val);
extern bool isLaissezVibrer(const engraving::SymId id);
extern kfw::String errorStringWithLocation(int64_t byteOffset, const kfw::String& error);
extern kfw::String checkAtEndElement(const kfw::XmlStreamReader& e, const kfw::String& expName);

extern kfw::String harmonyXmlFunction(const engraving::HarmonyInfo* info, const engraving::Harmony* h);
extern kfw::String harmonyXmlFunction(const engraving::HarmonyInfo* info, const engraving::Harmony* h, engraving::Key k);
extern kfw::String harmonyXmlKind(const engraving::HarmonyInfo* h);
extern kfw::String harmonyXmlText(const engraving::HarmonyInfo* h);
extern kfw::String harmonyXmlSymbols(const engraving::HarmonyInfo* h);
extern kfw::String harmonyXmlParens(const engraving::HarmonyInfo* h);
extern kfw::StringList harmonyXmlDegrees(const engraving::HarmonyInfo* h);
extern const engraving::ChordDescription* harmonyFromXml(engraving::HarmonyInfo* info, engraving::Score* score, const kfw::String& kind,
                                                         const kfw::String& kindText, const kfw::String& symbols,
                                                         const kfw::String& parens, const std::vector<engraving::HDegree>& dl);
extern void setHarmonyRootTpcFromFunction(engraving::HarmonyInfo* info, const engraving::Harmony* h, const kfw::String& s,
                                          engraving::Key key);
} // namespace kn::iex::musicxml
