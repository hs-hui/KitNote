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

#include "global/types/string.h"

#include "engraving/dom/interval.h"
#include "engraving/types/types.h"

namespace kn::engraving {
class Arpeggio;
}

namespace kn::iex::musicxml {
const int MAX_NUMBER_LEVEL = 16; // maximum number of overlapping MusicXML objects

enum class MusicXmlExporterSoftware : char {
    DOLET6,
    DOLET8,
    DORICO,
    FINALE,
    NOTEFLIGHT,
    SIBELIUS,
    OTHER
};

//---------------------------------------------------------
//   MusicXmlStartStop
//---------------------------------------------------------

enum class MusicXmlStartStop : char {
    NONE, START, STOP
};

struct MusicXmlArpeggioDesc {
    engraving::Arpeggio* arp;
    int no;

    MusicXmlArpeggioDesc(engraving::Arpeggio* arp, int no)
        : arp(arp), no(no) {}
};
typedef std::multimap<int, MusicXmlArpeggioDesc> ArpeggioMap;

//---------------------------------------------------------
//   MusicXmlInstrument
//---------------------------------------------------------

/**
 A single instrument in a MusicXML part.
 Used for both a drum part and a (non-drum) multi-instrument part
 */

struct MusicXmlInstrument {
    int unpitched;                     // midi-unpitched read from MusicXML
    kfw::String name;                       // instrument-name read from MusicXML
    kfw::String sound;                      // instrument-sound read from MusicXML
    kfw::String virtLib;                    // virtual-library read from MusicXML
    kfw::String virtName;                   // virtual-name read from MusicXML
    int midiChannel;                   // midi-channel read from MusicXML
    int midiPort;                      // port read from MusicXML
    int midiProgram;                   // midi-program read from MusicXML
    int midiVolume;                    // volume read from MusicXML
    int midiPan;                       // pan value read from MusicXML
    engraving::NoteHeadGroup notehead;            // notehead symbol set
    int line = 0;                      // place notehead onto this line
    engraving::DirectionV stemDirection;

    kfw::String toString() const;

    MusicXmlInstrument()        // required by std::map
        : unpitched(-1), name(), midiChannel(-1), midiPort(-1), midiProgram(-1), midiVolume(100), midiPan(63),
        notehead(engraving::NoteHeadGroup::HEAD_INVALID), line(0), stemDirection(engraving::DirectionV::AUTO) {}
    MusicXmlInstrument(kfw::String s)
        : unpitched(-1), name(s), midiChannel(-1), midiPort(-1), midiProgram(-1), midiVolume(100), midiPan(63),
        notehead(engraving::NoteHeadGroup::HEAD_NORMAL), line(0), stemDirection(engraving::DirectionV::AUTO) {}
    /*
    MusicXmlInstrument(int p, String s, NoteHead::Group nh, int l, Direction d)
          : unpitched(p), name(s), midiChannel(-1), midiPort(-1), midiProgram(-1), midiVolume(100), midiPan(63),
          notehead(nh), line(l), stemDirection(d) {}
     */
};
typedef std::map<kfw::String, MusicXmlInstrument> MusicXmlInstruments;

class MusicXmlIntervalList : public std::map<engraving::Fraction, engraving::Interval>
{
public:
    MusicXmlIntervalList() {}
    engraving::Interval interval(const engraving::Fraction f) const;
};

class MusicXmlInstrList : public std::map<engraving::Fraction, kfw::String>
{
public:
    MusicXmlInstrList() {}
    const kfw::String instrument(const engraving::Fraction f) const;
    void setInstrument(const kfw::String instr, const engraving::Fraction f);
};

class LyricNumberHandler
{
public:
    LyricNumberHandler() {}
    void addNumber(const kfw::String& number);
    kfw::String toString() const;
    int getLyricNo(const kfw::String& number) const;
    void determineLyricNos();
private:
    std::map<kfw::String, int> m_numberToNo;
};
}
