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

#include <QPixmap>
#include <QDate>

#include "translation.h"

#include "types/id.h"
#include "types/translatablestring.h"

#include "engraving/dom/articulation.h"
#include "engraving/dom/chord.h"
#include "engraving/dom/durationtype.h"
#include "engraving/dom/engravingitem.h"
#include "engraving/dom/guitarbend.h"
#include "engraving/dom/hairpin.h"
#include "engraving/dom/harmony.h"
#include "engraving/dom/hook.h"
#include "engraving/dom/instrtemplate.h"
#include "engraving/dom/instrtemplate.h"
#include "engraving/dom/instrument.h"
#include "engraving/dom/key.h"
#include "engraving/dom/measure.h"
#include "engraving/dom/engravingglobals.h"
#include "engraving/dom/note.h"
#include "engraving/dom/ottava.h"
#include "engraving/dom/page.h"
#include "engraving/dom/part.h"
#include "engraving/dom/realizedharmony.h"
#include "engraving/dom/rest.h"
#include "engraving/dom/score.h"
#include "engraving/dom/slur.h"
#include "engraving/dom/staff.h"
#include "engraving/dom/stafftype.h"
#include "engraving/dom/stem.h"
#include "engraving/dom/system.h"
#include "engraving/dom/timesig.h"
#include "engraving/dom/tuplet.h"

#include "engraving/rendering/layoutoptions.h"

namespace kn::notation {
using Page = kn::engraving::Page;
using System = kn::engraving::System;
using EngravingItem = kn::engraving::EngravingItem;
using ElementType = kn::engraving::ElementType;
using PropertyValue = engraving::PropertyValue;
using Note = kn::engraving::Note;
using Measure = kn::engraving::Measure;
using DurationType = kn::engraving::DurationType;
using Duration = kn::engraving::TDuration;
using SelectType = kn::engraving::SelectType;
using SelectionState = kn::engraving::SelState;
using Pad = kn::engraving::Pad;
using ViewMode = engraving::LayoutMode;
using PitchMode = kn::engraving::UpDownMode;
using StyleId = kn::engraving::Sid;
using StyleIdSet = kn::engraving::StyleIdSet;
using SymbolId = kn::engraving::SymId;
using Key = kn::engraving::Key;
using KeyMode = kn::engraving::KeyMode;
using TimeSigType = kn::engraving::TimeSigType;
using TimeSignature = kn::engraving::TimeSig;
using Part = kn::engraving::Part;
using Staff = kn::engraving::Staff;
using NoteHead = kn::engraving::NoteHead;
using SharpFlat = kn::engraving::PreferSharpFlat;
using TransposeMode = kn::engraving::TransposeMode;
using TransposeDirection = kn::engraving::TransposeDirection;
using Fraction = kn::engraving::Fraction;
using ElementPattern = kn::engraving::ElementPattern;
using SelectionFilterTypesVariant = kn::engraving::SelectionFilterTypesVariant;
using Chord = kn::engraving::Chord;
using ChordRest = kn::engraving::ChordRest;
using Harmony = kn::engraving::Harmony;
using RealisedHarmony = kn::engraving::RealizedHarmony;
using Articulation = kn::engraving::Articulation;
using SlurSegment = kn::engraving::SlurSegment;
using Rest = kn::engraving::Rest;
using Stem = kn::engraving::Stem;
using Hook = kn::engraving::Hook;
using Fraction = kn::engraving::Fraction;
using NoteInputMethod = kn::engraving::NoteEntryMethod;
using NoteInputParams = kn::engraving::NoteInputParams;
using AccidentalType = kn::engraving::AccidentalType;
using OttavaType = kn::engraving::OttavaType;
using HairpinType = kn::engraving::HairpinType;
using TextBase = kn::engraving::TextBase;
using TupletNumberType = kn::engraving::TupletNumberType;
using TupletBracketType = kn::engraving::TupletBracketType;
using GraceNoteType = kn::engraving::NoteType;
using BeamMode = kn::engraving::BeamMode;
using LayoutBreakType = kn::engraving::LayoutBreakType;
using Interval = kn::engraving::Interval;
using Drumset = kn::engraving::Drumset;
using StringData = kn::engraving::StringData;
using Clef = kn::engraving::Clef;
using ClefType = kn::engraving::ClefType;
using ClefTypeList = kn::engraving::ClefTypeList;
using BracketType = kn::engraving::BracketType;
using StaffGroup = kn::engraving::StaffGroup;
using StaffType = kn::engraving::StaffType;
using StaffTypeId = kn::engraving::StaffTypes;
using StaffName = kn::engraving::StaffName;
using StaffNameList = kn::engraving::StaffNameList;
using Segment = kn::engraving::Segment;
using TextStyleType = kn::engraving::TextStyleType;
using TraitType = kn::engraving::TraitType;
using HarmonyDurationType = kn::engraving::HDuration;
using Voicing = kn::engraving::Voicing;
using Instrument = kn::engraving::Instrument;
using InstrumentTemplate = kn::engraving::InstrumentTemplate;
using InstrumentTrait = kn::engraving::Trait;
using ScoreOrder = kn::engraving::ScoreOrder;
using InstrumentGenre = kn::engraving::InstrumentGenre;
using InstrumentGroup = kn::engraving::InstrumentGroup;
using PageList = std::vector<Page*>;
using PartList = std::vector<const Part*>;
using InstrumentTemplateList = std::vector<const InstrumentTemplate*>;
using InstrumentGenreList = std::vector<const InstrumentGenre*>;
using ScoreOrderList = std::vector<kn::engraving::ScoreOrder>;
using InstrumentGroupList = std::vector<const InstrumentGroup*>;
using InstrumentTrackId = kn::engraving::InstrumentTrackId;
using InstrumentTrackIdSet = kn::engraving::InstrumentTrackIdSet;
using voice_idx_t = kn::engraving::voice_idx_t;
using track_idx_t = kn::engraving::track_idx_t;
using staff_idx_t = kn::engraving::staff_idx_t;
using ScoreChanges = kn::engraving::ScoreChanges;
using GuitarBendType = kn::engraving::GuitarBendType;
using engraving::LoopBoundaryType;
using Pid = kn::engraving::Pid;
using VoiceAssignment = kn::engraving::VoiceAssignment;
using MeasureBeat = kn::engraving::MeasureBeat;

static const kfw::String COMMON_GENRE_ID("common");

enum class DragMode : unsigned char
{
    BothXY = 0,
    OnlyX,
    OnlyY
};

enum class MoveDirection : unsigned char
{
    Undefined = 0,
    Left,
    Right,
    Up,
    Down
};

enum class MoveSelectionType : unsigned char
{
    Undefined = 0,
    EngravingItem,
    Chord,
    Measure,
    Track,
    Frame,
    System,
    String // TAB Staff
};

enum class ExpandSelectionMode : unsigned char
{
    BeginSystem,
    EndSystem,
    BeginScore,
    EndScore,
};

enum class AddRemoveSystemLockType : signed char
{
    AfterEachSystem = -1,
    None = 0,
    MeasuresInterval
};

enum class BoxType : unsigned char
{
    Unknown,
    Vertical,
    Horizontal,
    Measure,
    Text,
    Fret
};

enum class AddBoxesTarget : unsigned char {
    AfterSelection,
    BeforeSelection,
    AtStartOfScore,
    AtEndOfScore
};

enum class NoteName : unsigned char
{
    C = 0,
    D,
    E,
    F,
    G,
    A,
    B
};

using NoteVal = kn::engraving::NoteVal;
using NoteValList = kn::engraving::NoteValList;

enum class NoteAddingMode : unsigned char
{
    CurrentChord,
    NextChord,
    InsertChord
};

enum class PastingType : unsigned char {
    Default,
    Half,
    Double,
    Special
};

using NoteInputState = kn::engraving::InputState;

enum class NoteFilter : unsigned char
{
    All,
    WithTie,
    WithSlur
};

enum class ZoomType : unsigned char {
    Percentage,
    PageWidth,
    WholePage,
    TwoPages
};

inline kfw::TranslatableString zoomTypeTitle(ZoomType type)
{
    switch (type) {
    case ZoomType::Percentage: return kfw::TranslatableString("notation", "Percentage");
    case ZoomType::PageWidth: return kfw::TranslatableString("notation", "Page width");
    case ZoomType::WholePage: return kfw::TranslatableString("notation", "Whole page");
    case ZoomType::TwoPages: return kfw::TranslatableString("notation", "Two pages");
    }

    return {};
}

struct Tempo
{
    int valueBpm = 0;
    DurationType duration = DurationType::V_QUARTER;
    bool withDot = false;

    bool operator==(const Tempo& other) const
    {
        return valueBpm == other.valueBpm && duration == other.duration && withDot == other.withDot;
    }
};

static constexpr int MAX_STAVES  = 4;

struct InstrumentKey
{
    kfw::String instrumentId;
    kfw::ID partId;
    Fraction tick = kn::engraving::Fraction(0, 1);
};

inline bool isMainInstrumentForPart(const InstrumentKey& instrumentKey, const Part* part)
{
    return instrumentKey.instrumentId == part->instrumentId() && instrumentKey.tick == Part::MAIN_INSTRUMENT_TICK;
}

inline QString formatInstrumentTitle(const QString& instrumentName, const InstrumentTrait& trait)
{
    // Comments for translators start with //:
    switch (trait.type) {
    case TraitType::Tuning:
        //: %1=tuning ("D"), %2=name ("Tin Whistle"). Example: "D Tin Whistle"
        return kfw::qtrc("notation", "%1 %2", "Tuned instrument displayed in the UI")
               .arg(trait.name, instrumentName);
    case TraitType::Transposition:
        //: %1=name ("Horn"), %2=transposition ("C alto"). Example: "Horn in C alto"
        return kfw::qtrc("notation", "%1 in %2", "Transposing instrument displayed in the UI")
               .arg(instrumentName, trait.name);
    case TraitType::Course:
        //: %1=name ("Tenor Lute"), %2=course/strings ("7-course"). Example: "Tenor Lute (7-course)"
        return kfw::qtrc("notation", "%1 (%2)", "String instrument displayed in the UI")
               .arg(instrumentName, trait.name);
    case TraitType::Unknown:
        return instrumentName; // Example: "Flute"
    }
    Q_UNREACHABLE();
}

inline QString formatInstrumentTitle(const QString& instrumentName, const InstrumentTrait& trait, int instrumentNumber)
{
    if (instrumentNumber == 0) {
        // Only one instance of this instrument in the score
        return formatInstrumentTitle(instrumentName, trait);
    }

    QString number = QString::number(instrumentNumber);

    // Comments for translators start with //:
    switch (trait.type) {
    case TraitType::Tuning:
        //: %1=tuning ("D"), %2=name ("Tin Whistle"), %3=number ("2"). Example: "D Tin Whistle 2"
        return kfw::qtrc("notation", "%1 %2 %3", "One of several tuned instruments displayed in the UI")
               .arg(trait.name, instrumentName, number);
    case TraitType::Transposition:
        //: %1=name ("Horn"), %2=transposition ("C alto"), %3=number ("2"). Example: "Horn in C alto 2"
        return kfw::qtrc("notation", "%1 in %2 %3", "One of several transposing instruments displayed in the UI")
               .arg(instrumentName, trait.name, number);
    case TraitType::Course:
        //: %1=name ("Tenor Lute"), %2=course/strings ("7-course"), %3=number ("2"). Example: "Tenor Lute (7-course) 2"
        return kfw::qtrc("notation", "%1 (%2) %3", "One of several string instruments displayed in the UI")
               .arg(instrumentName, trait.name, number);
    case TraitType::Unknown:
        //: %1=name ("Flute"), %2=number ("2"). Example: "Flute 2"
        return kfw::qtrc("notation", "%1 %2", "One of several instruments displayed in the UI")
               .arg(instrumentName, number);
    }
    Q_UNREACHABLE();
}

struct PartInstrument
{
    kfw::ID partId;
    InstrumentTemplate instrumentTemplate;

    bool isExistingPart = false;
    bool isSoloist = false;
};

using PartInstrumentList = QList<PartInstrument>;

struct PartInstrumentListScoreOrder
{
    PartInstrumentList instruments;
    ScoreOrder scoreOrder;
};

struct FilterElementsOptions
{
    ElementType elementType = ElementType::INVALID;
    int staffStart = -1;
    int staffEnd = -1;
    int voice = -1;
    const kn::engraving::System* system = nullptr;
    Fraction durationTicks{ -1, 1 };
    Fraction beat{ 0, 0 };
    const kn::engraving::Measure* measure = nullptr;

    bool bySubtype = false;
    int subtype = -1;

    bool isValid() const
    {
        return elementType != ElementType::INVALID;
    }

    virtual ~FilterElementsOptions() = default;
};

struct FilterNotesOptions : FilterElementsOptions
{
    int pitch = -1;
    int string = kn::engraving::INVALID_STRING_INDEX;
    int tpc = kn::engraving::Tpc::TPC_INVALID;
    kn::engraving::NoteHeadGroup notehead = kn::engraving::NoteHeadGroup::HEAD_INVALID;
    kn::engraving::TDuration durationType = kn::engraving::TDuration();
    kn::engraving::NoteType noteType = kn::engraving::NoteType::INVALID;
};

struct StaffConfig
{
    bool visible = false;
    engraving::Spatium userDistance = engraving::Spatium(0.0);
    bool cutaway = false;
    bool hideSystemBarline = false;
    engraving::AutoOnOff mergeMatchingRests = engraving::AutoOnOff::AUTO;
    bool reflectTranspositionInLinkedTab = false;
    ClefTypeList clefTypeList;
    engraving::StaffType staffType;

    bool operator==(const StaffConfig& conf) const
    {
        bool equal = visible == conf.visible;
        equal &= userDistance == conf.userDistance;
        equal &= cutaway == conf.cutaway;
        equal &= hideSystemBarline == conf.hideSystemBarline;
        equal &= mergeMatchingRests == conf.mergeMatchingRests;
        equal &= clefTypeList == conf.clefTypeList;
        equal &= staffType == conf.staffType;
        equal &= reflectTranspositionInLinkedTab == conf.reflectTranspositionInLinkedTab;

        return equal;
    }

    bool operator!=(const StaffConfig& conf) const
    {
        return !(*this == conf);
    }
};

struct TransposeOptions
{
    TransposeMode mode = TransposeMode::UNKNOWN;
    TransposeDirection direction = TransposeDirection::UNKNOWN;
    Key key = Key::C;
    int interval = 0;
    bool needTransposeKeys = false;
    bool needTransposeChordNames = false;
    bool needTransposeDoubleSharpsFlats = false;
};

struct TupletOptions
{
    Fraction ratio = { -1, -1 };
    TupletNumberType numberType = TupletNumberType::SHOW_NUMBER;
    TupletBracketType bracketType = TupletBracketType::AUTO_BRACKET;
    bool autoBaseLen = false;
};

struct LoopBoundaries
{
    Fraction loopInTick;
    Fraction loopOutTick;
    bool enabled = false;

    bool isNull() const
    {
        return loopInTick.isZero() && loopOutTick.isZero();
    }

    bool operator==(const LoopBoundaries& boundaries) const
    {
        bool equals = true;

        equals &= loopInTick == boundaries.loopInTick;
        equals &= loopOutTick == boundaries.loopOutTick;
        equals &= enabled == boundaries.enabled;

        return equals;
    }

    bool operator!=(const LoopBoundaries& boundaries) const
    {
        return !(*this == boundaries);
    }
};

enum class ScoreConfigType : unsigned char
{
    ShowInvisibleElements,
    ShowUnprintableElements,
    ShowFrames,
    ShowPageMargins,
    ShowSoundFlags,
    MarkIrregularMeasures
};

struct ScoreConfig
{
    bool isShowInvisibleElements = false;
    bool isShowUnprintableElements = false;
    bool isShowFrames = false;
    bool isShowPageMargins = false;
    bool isShowSoundFlags = false;
    bool isMarkIrregularMeasures = false;

    bool operator==(const ScoreConfig& conf) const
    {
        bool equal = (isShowInvisibleElements == conf.isShowInvisibleElements);
        equal &= (isShowUnprintableElements == conf.isShowUnprintableElements);
        equal &= (isShowFrames == conf.isShowFrames);
        equal &= (isShowPageMargins == conf.isShowPageMargins);
        equal &= (isShowSoundFlags == conf.isShowSoundFlags);
        equal &= (isMarkIrregularMeasures == conf.isMarkIrregularMeasures);

        return equal;
    }
};

inline QString staffTypeToString(StaffTypeId type)
{
    const StaffType* preset = StaffType::preset(type);
    return preset ? preset->name().toQString() : QString();
}

// KitNote: what "Tools > Change the selection" does to the drum notes that are selected.
enum class DrumTransform : unsigned char
{
    HiHatToRide,
    RideToHiHat,
    SwapSticking,
    AccentBeats,
    AccentOffBeats,
    ClearAccents,
};

// KitNote: a section of a drum chart - what a rehearsal mark or a section name (Intro, Verse, Chorus)
// starts, up to the next one (see INotationInteraction::chartSections).
struct ChartSection {
    QString name;
    int firstMeasure = 0;       // as counted in the score, from 1
    int measureCount = 0;
    bool current = false;       // the selection is in it
};

// KitNote: one measure of a drum staff as a drum machine shows it - a row for every drum, a step for
// every sixteenth note (see INotationInteraction::drumStepGrid).
struct DrumStepGrid {
    struct Row {
        int pitch = -1;
        QString name;
        std::vector<bool> steps;
        std::vector<int> states;    // per step: 0 off, 1 stroke, 2 accented, 3 ghost note
    };

    bool valid = false;         // false: no measure of a drum staff is selected, or it does not fit the grid
    bool fitsGrid = true;       // false: the measure has notes between the sixteenths (triplets, 32nds)
    bool repeatSign = false;    // the measure is a measure-repeat sign: it shows nothing to put on steps
    bool triplets = false;      // the steps are triplet eighths (three to a beat), not sixteenths
    bool empty = true;          // the measure fits both kinds of grid (it is empty, or its notes are on the beats)
    int measureNumber = 0;
    int measureTick = 0;        // where the measure starts and how long it is, for the playhead
    int measureTicks = 0;
    int stepsPerBeat = 4;
    int stepCount = 0;
    std::vector<Row> rows;

    // where the measure is, for letting a drum be heard when it is put on a step
    staff_idx_t staffIdx = kfw::nidx;
    const Segment* firstSegment = nullptr;
};

enum class DrumPattern : unsigned char;

//! KitNote: a drum pattern as a drum machine would show it, for the pattern library: one measure of it,
//! a row for every drum it uses.
struct DrumPatternPreview
{
    struct Row {
        QString label;              // as in a drum tab: "HH", "SD", "BD"
        int family = 0;             // the colour: 1 bass drum, 2 snare, 3 hi-hat, 4 cymbal, 5 tom
        std::vector<int> states;    // per step: 0 nothing, 1 a stroke, 2 an accented stroke
    };

    int steps = 16;                 // 16 sixteenth notes, or 12 triplet eighths (or the 12 sixteenths of 6/8)
    int group = 4;                  // steps to a beat
    QString sticking;               // of a rudiment: a letter or a space for every step
    std::vector<Row> rows;
};

DrumPatternPreview drumPatternPreview(DrumPattern pattern);

// KitNote: grooves and fills that "Add > Drum patterns" writes into the selected measures.
enum class DrumPattern : unsigned char
{
    Rock8,
    Rock8Push,
    Rock16,
    Quarter,
    FourOnFloor,
    HalfTime,
    Ride8,
    CrashRock8,
    SnareFill,
    TomFill,
    Shuffle,
    TripletFeel,
    TripletFill,
    Disco,
    BossaNova,
    SixEight,
    Motown,
    JazzRide,
    ReggaeOneDrop,
    HalfBarFill,
    // snare rudiments, written with their sticking
    RudimentSingleStroke,
    RudimentDoubleStroke,
    RudimentParadiddle,
    RudimentStrokeTriplets,
    RudimentDoubleParadiddle,
    RudimentParadiddleDiddle,
    RudimentFlams,
    RudimentFlamTap,
    RudimentFlamAccent,
};

enum class BracketsType : unsigned char
{
    Brackets,
    Braces,
    Parentheses
};

struct ScoreCreateOptions
{
    bool withTempo = false;
    Tempo tempo;

    Fraction globalTimesig;
    TimeSigType timesigType = TimeSigType::NORMAL;

    Key key = Key::C;

    int totalMeasures = 0;

    bool withPickupMeasure = false;
    Fraction pickupTimesig;

    PartInstrumentList parts;
    ScoreOrder order;
};

inline const ScoreOrder& customOrder()
{
    static ScoreOrder order;
    order.id = "custom";
    order.name = kfw::TranslatableString("engraving/scoreorder", "Custom");

    return order;
}

static constexpr int MIN_NOTES_INTERVAL = -10;
static constexpr int MAX_NOTES_INTERVAL = 10;

static constexpr int MAX_FRET = 14;

constexpr bool isNotesIntervalValid(int interval)
{
    return interval >= MIN_NOTES_INTERVAL && interval <= MAX_NOTES_INTERVAL
           && interval != 0 && interval != -1;
}

constexpr bool isVoiceIndexValid(voice_idx_t voiceIndex)
{
    return voiceIndex < kn::engraving::VOICES;
}

inline bool isVerticalBoxTextStyle(TextStyleType type)
{
    static const std::set<TextStyleType> types {
        TextStyleType::TITLE,
        TextStyleType::SUBTITLE,
        TextStyleType::COMPOSER,
        TextStyleType::LYRICIST,
        TextStyleType::INSTRUMENT_EXCERPT,
    };

    return kfw::contains(types, type);
}

struct StringTuningPreset
{
    std::string name;
    std::vector<int> value;
    bool useFlats = false;
};

struct StringTuningsInfo
{
    size_t number = 0;
    std::vector<StringTuningPreset> presets;
};

using InstrumentStringTuningsMap = std::map<std::string, std::vector<StringTuningsInfo> >;

static const kn::engraving::ElementTypeSet NOTE_REST_TYPES {
    kn::engraving::ElementType::NOTE,
    kn::engraving::ElementType::REST,
};
}
