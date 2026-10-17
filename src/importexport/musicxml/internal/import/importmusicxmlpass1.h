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

#include "global/serialization/xmlstreamreader.h"
#include "musicxmltupletstate.h"
#include "musicxmlpart.h"
#include "engraving/engravingerrors.h"

namespace kn::engraving {
class Fraction;
class Score;
class Part;
class VBox;
}

namespace kn::iex::musicxml {
class VoiceOverlapDetector;

//---------------------------------------------------------
//   PageFormat
//---------------------------------------------------------

struct PageFormat {
    engraving::SizeF size;                         // automatically initialized (to invalid)
    double printableWidth = 5;          // _width - left margin - right margin
    double evenLeftMargin = 0.2;        // values in inch
    double oddLeftMargin = 0.2;
    double evenTopMargin = 0.2;
    double evenBottomMargin = 0.2;
    double oddTopMargin = 0.2;
    double oddBottomMargin = 0.2;
    bool twosided = false;
};

typedef std::pair<int, int> StartStop;
typedef std::vector<StartStop> StartStopList;

//---------------------------------------------------------
//   MusicXmlOctaveShiftDesc
//---------------------------------------------------------

struct MusicXmlOctaveShiftDesc {
    enum class Type : char {
        UP, DOWN, STOP, NONE
    };
    Type tp;
    short size;
    engraving::Fraction time;
    short num;
    MusicXmlOctaveShiftDesc()
        : tp(Type::NONE), size(0), num(-1) {}
    MusicXmlOctaveShiftDesc(Type _tp, short _size, engraving::Fraction _tm)
        : tp(_tp), size(_size), time(_tm), num(-1) {}
};

//---------------------------------------------------------
//   MusicXmlPartGroup
//---------------------------------------------------------

struct MusicXmlPartGroup {
    int span = 0;
    int start = 0;
    engraving::BracketType type = engraving::BracketType::NO_BRACKET;
    bool barlineSpan = false;
    kfw::draw::Color color;
    size_t column = 0;
};
typedef std::vector<MusicXmlPartGroup*> MusicXmlPartGroupList;
typedef std::map<kfw::String, engraving::Part*> PartMap;
typedef std::map<int, MusicXmlPartGroup*> MusicXmlPartGroupMap;

//---------------------------------------------------------
//   CreditWords
//    a single parsed MusicXML credit-words element
//---------------------------------------------------------

struct CreditWords {
    int page = 0;
    kfw::String type;
    double defaultX = 0.0;
    double defaultY = 0.0;
    double fontSize = 0.0;
    kfw::String justify;
    kfw::String hAlign;
    kfw::String vAlign;
    kfw::String words;
    CreditWords(int p, kfw::String tp, double dx, double dy, double fs, kfw::String j, kfw::String ha, kfw::String va, kfw::String w)
    {
        page = p;
        type = tp;
        defaultX = dx;
        defaultY = dy;
        fontSize = fs;
        justify  = j;
        hAlign   = ha;
        vAlign   = va;
        words    = w;
    }
};
typedef  std::vector<CreditWords*> CreditWordsList;

//---------------------------------------------------------
//   declarations
//---------------------------------------------------------

bool isLikelyCreditText(const kfw::String& text, const bool caseInsensitive);
bool isLikelySubtitleText(const kfw::String& text, const bool caseInsensitive);

//---------------------------------------------------------
//   MusicXmlParserPass1
//---------------------------------------------------------

class MusicXmlLogger;

class MusicXmlParserPass1
{
public:
    MusicXmlParserPass1(engraving::Score* score, MusicXmlLogger* logger);
    void initPartState(const kfw::String& partId);
    engraving::Err parse(const kfw::ByteArray& data);
    engraving::Err parse();
    kfw::String errors() const { return m_errors; }
    void scorePartwise();
    void identification();
    void credit(CreditWordsList& credits);
    void defaults();
    void pageLayout(PageFormat& pf, const double conversion);
    void partList(MusicXmlPartGroupList& partGroupList);
    void partGroup(const int scoreParts, MusicXmlPartGroupList& partGroupList, MusicXmlPartGroupMap& partGroups,
                   kfw::String& curPartGroupName);
    void scorePart(const kfw::String& curPartGroupName);
    void scoreInstrument(const kfw::String& partId, const kfw::String& curPartGroupName);
    void setStyle(const kfw::String& type, const double val);
    void midiInstrument(const kfw::String& partId);
    void part();
    void measure(const kfw::String& partId, const engraving::Fraction cTime, engraving::Fraction& mdur, VoiceOverlapDetector& vod,
                 const int measureNr);
    void print(const int measureNr);
    void attributes(const kfw::String& partId, const engraving::Fraction cTime);
    void clef(const kfw::String& partId);
    void time(const engraving::Fraction cTime);
    void transpose(const kfw::String& partId, const engraving::Fraction& tick);
    void divisions();
    void direction(const kfw::String& partId, const engraving::Fraction& cTime);
    void directionType(const engraving::Fraction cTime, std::vector<MusicXmlOctaveShiftDesc>& starts,
                       std::vector<MusicXmlOctaveShiftDesc>& stops);
    void handleOctaveShift(const engraving::Fraction& cTime, const kfw::String& type, short size, MusicXmlOctaveShiftDesc& desc);
    void notations(MusicXmlStartStop& tupletStartStop);
    void note(const kfw::String& partId, const engraving::Fraction& cTime, engraving::Fraction& missingPrev, engraving::Fraction& dura,
              engraving::Fraction& missingCurr, VoiceOverlapDetector& vod, MusicXmlTupletStates& tupletStates);
    void notePrintSpacingNo(engraving::Fraction& dura);
    engraving::Fraction calcTicks(const int& intTicks, const int& _divisions, const kfw::XmlStreamReader* xmlReader);
    engraving::Fraction calcTicks(const int& intTicks) { return calcTicks(intTicks, m_divs, &m_e); }
    void duration(engraving::Fraction& dura, kfw::XmlStreamReader& e);
    void duration(engraving::Fraction& dura) { duration(dura, m_e); }
    void forward(engraving::Fraction& dura);
    void backup(engraving::Fraction& dura);
    void timeModification(engraving::Fraction& timeMod);
    void pitch(int& step, float& alter, int& oct);
    void rest();
    void skipLogCurrElem();
    bool determineMeasureLength(std::vector<engraving::Fraction>& ml) const;
    VoiceList getVoiceList(const kfw::String& id) const;
    bool determineStaffMoveVoice(const kfw::String& id, const int mxStaff, const int& mxVoice, int& msMove, int& msTrack,
                                 int& msVoice) const;
    int voiceToInt(const kfw::String& voice);
    engraving::track_idx_t trackForPart(const kfw::String& id) const;
    bool hasPart(const kfw::String& id) const;
    engraving::Part* getPart(const kfw::String& id) const { return kfw::value(m_partMap, id); }
    MusicXmlPart getMusicXmlPart(const kfw::String& id) const { return kfw::value(m_parts, id); }
    size_t nparts() const { return m_parts.size(); }
    MusicXmlInstruments getInstruments(const kfw::String& id) const { return kfw::value(m_instruments, id); }
    void setDrumsetDefault(const kfw::String& id, const kfw::String& instrId, const engraving::NoteHeadGroup hg, const int line,
                           const engraving::DirectionV sd);
    MusicXmlInstrList getInstrList(const kfw::String& id) const;
    MusicXmlIntervalList getIntervals(const kfw::String& id) const;
    engraving::Fraction getMeasureStart(const size_t i) const;
    int octaveShift(const kfw::String& id, const engraving::staff_idx_t staff, const engraving::Fraction& f) const;
    const CreditWordsList& credits() const { return m_credits; }
    bool hasBeamingInfo() const { return m_hasBeamingInfo; }
    bool isVocalStaff(const kfw::String& partId) const { return m_parts.at(partId).isVocalStaff(); }
    bool isPercussionStaff(const kfw::String& partId) const { return m_parts.at(partId).isPercussionStaff(); }
    static engraving::VBox* createAndAddVBoxForCreditWords(engraving::Score* score, engraving::Fraction tick);
    void createDefaultHeader(engraving::Score* const score);
    void createMeasuresAndVboxes(engraving::Score* const score, const std::vector<engraving::Fraction>& ml,
                                 const std::vector<engraving::Fraction>& ms, const std::set<int>& systemStartMeasureNrs,
                                 const std::set<int>& pageStartMeasureNrs, const CreditWordsList& crWords, const kfw::Size& pageSize);
    void setHasInferredHeaderText(bool b) { m_hasInferredHeaderText = b; }
    bool hasInferredHeaderText() const { return m_hasInferredHeaderText; }
    int maxDiff() const { return m_maxDiff; }
    void insertAdjustedDuration(engraving::Fraction key, engraving::Fraction value) { m_adjustedDurations.insert({ key, value }); }
    std::map<engraving::Fraction, engraving::Fraction>& adjustedDurations() { return m_adjustedDurations; }
    void insertSeenDenominator(int val) { m_seenDenominators.emplace(val); }
    MusicXmlExporterSoftware exporterSoftware() const { return m_exporterSoftware; }
    bool sibOrDolet() const;
    bool dolet() const;

private:
    // functions
    void addError(const kfw::String& error);        // Add an error to be shown in the GUI
    void setExporterSoftware(kfw::String& exporter);
    void setExporterStyles();

    // generic pass 1 data
    kfw::XmlStreamReader m_e;
    MusicXmlExporterSoftware m_exporterSoftware = MusicXmlExporterSoftware::OTHER;   // Software which exported the file
    int m_divs = 0;                              // Current MusicXML divisions value
    std::map<kfw::String, MusicXmlPart> m_parts;      // Parts data, mapped on part id
    std::set<int> m_systemStartMeasureNrs;       // Measure numbers of measures starting a page
    std::set<int> m_pageStartMeasureNrs;         // Measure numbers of measures starting a page
    std::vector<engraving::Fraction> m_measureLength;       // Length of each measure
    std::vector<engraving::Fraction> m_measureStart;        // Start time of each measure
    CreditWordsList m_credits;                   // All credits collected
    PartMap m_partMap;                           // TODO merge into MusicXmlPart ??
    std::map<kfw::String, MusicXmlInstruments> m_instruments;   // instruments for each part, mapped on part id
    engraving::Score* m_score = nullptr;                    // MuseScore score
    MusicXmlLogger* m_logger = nullptr;              // Error logger
    kfw::String m_errors;                             // Errors to present to the user
    bool m_hasBeamingInfo = false;               // Whether the score supports or contains beaming info
    bool m_hasInferredHeaderText = false;

    // part specific data (TODO: move to part-specific class)
    engraving::Fraction m_timeSigDura;                      // Measure duration according to last timesig read
    std::map<int, MusicXmlOctaveShiftDesc> m_octaveShifts;   // Pending octave-shifts
    kfw::Size m_pageSize;                             // Page width read from defaults

    const int m_maxDiff = 5;                   // Duration rounding tick threshold;
    std::map<engraving::Fraction, engraving::Fraction> m_adjustedDurations;  // Rounded durations
    std::set<int> m_seenDenominators;          // Denominators seen. Used for rounding errors.
};
} // namespace kn::iex::musicxml
