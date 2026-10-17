/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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

#include "dom/engravingitem.h"

namespace kn::engraving {
class Accidental;
class ActionIcon;
class Ambitus;
class Arpeggio;
class Articulation;

class BagpipeEmbellishment;
class BarLine;
class Beam;
class Bend;

class Box;
class HBox;
class VBox;
class FBox;
class TBox;

class Bracket;
class Breath;

class Chord;
class ChordBracket;
class ChordLine;
class Clef;
class Capo;

class DeadSlapped;
class Dynamic;

class Expression;

class Fermata;
class FiguredBassItem;
class FiguredBass;
class Fingering;
class FretDiagram;

class Glissando;
class GlissandoSegment;
class GraceNotesGroup;
class GradualTempoChangeSegment;
class GradualTempoChange;

class HairpinSegment;
class Hairpin;
class HammerOnPullOffSegment;
class HammerOnPullOffText;
class HarpPedalDiagram;
class HarmonicMarkSegment;
class Harmony;
class Hook;

class IndicatorIcon;

class Image;
class InstrumentChange;
class InstrumentName;
class Jump;

class KeySig;

class LaissezVib;
class LayoutBreak;
class LedgerLine;
class LetRing;
class LetRingSegment;
class LineSegment;
class Lyrics;
class LyricsLine;
class LyricsLineSegment;

class Marker;
class MeasureBase;
class MeasureNumber;
class MeasureNumberBase;
class MeasureRepeat;
class MMRest;
class MMRestRange;

class Note;
class NoteDot;
class NoteLineSegment;

class Ornament;

class Ottava;
class OttavaSegment;

class PalmMute;
class PalmMuteSegment;
class Parenthesis;
class Pedal;
class PedalSegment;
class PickScrapeSegment;
class PlayCountText;
class PlayTechAnnotation;

class RasgueadoSegment;
class RehearsalMark;
class Rest;

class ShadowNote;
class SLine;
class Slur;
class Spacer;
class SpannerSegment;
class StaffLines;
class StaffState;
class StaffText;
class StaffTypeChange;
class Stem;
class StemSlash;
class Sticking;
class StringTunings;

class BSymbol;
class Symbol;
class FSymbol;

class SystemDivider;
class SystemText;
class SoundFlag;

class TabDurationSymbol;
class Tapping;
class TempoText;
class TextBase;
class TextBlock;
class TextFragment;
class Text;
class TextLine;
class TextLineSegment;
class TextLineBase;
class TextLineBaseSegment;
class Tie;
class TimeSig;
class TimeTickAnchor;
class TremoloSingleChord;
class TremoloTwoChord;
class TremoloBar;
class TrillSegment;
class TripletFeel;
class Trill;
class Tuplet;

class VibratoSegment;
class Vibrato;
class Volta;
class VoltaSegment;

class WhammyBarSegment;

// dev
class System;
class Measure;
class Segment;
class Chord;
}

namespace kn::engraving::rendering::score {
class TDraw
{
public:

    static void drawItem(const EngravingItem* item, kfw::draw::Painter* painter, const PaintOptions& opt); // factory

private:
    static void draw(const Accidental* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const ActionIcon* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Ambitus* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Arpeggio* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const ChordBracket* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Articulation* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const BagpipeEmbellishment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const BarLine* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Beam* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Bend* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Box* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const HBox* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const VBox* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const FBox* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TBox* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Bracket* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Breath* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const ChordLine* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Clef* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Capo* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const DeadSlapped* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Dynamic* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Expression* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Fermata* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const FiguredBass* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const FiguredBassItem* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Fingering* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const FretDiagram* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const GlissandoSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const GradualTempoChangeSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const GuitarBendSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const GuitarBendHoldSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const HairpinSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const HammerOnPullOffSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const HammerOnPullOffText* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const HarpPedalDiagram* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const HarmonicMarkSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Harmony* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Hook* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Image* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const InstrumentChange* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const InstrumentName* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Jump* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const KeySig* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const LaissezVibSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Lasso* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const LayoutBreak* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const LedgerLine* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const LetRingSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Lyrics* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const LyricsLineSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Marker* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const MeasureNumber* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const MeasureRepeat* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const MMRest* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const MMRestRange* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Note* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const NoteDot* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const NoteHead* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const NoteLineSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Ornament* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const OttavaSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Parenthesis* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PartialTieSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PalmMuteSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PedalSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PickScrapeSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PlayCountText* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PlayTechAnnotation* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const RasgueadoSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const RehearsalMark* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Rest* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void drawDebugGapRest(const Rest* item, kfw::draw::Painter* painter);

    static void draw(const ShadowNote* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const SlurSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Spacer* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const StaffLines* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const StaffState* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const StaffText* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const StaffTypeChange* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Stem* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const StemSlash* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Sticking* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const StringTunings* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Symbol* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const FSymbol* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const SystemDivider* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const SystemText* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const IndicatorIcon* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const SoundFlag* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const TabDurationSymbol* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Tapping* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TempoText* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Text* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TextLineSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TieSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TimeSig* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TimeTickAnchor* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TremoloSingleChord* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TremoloTwoChord* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TremoloBar* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TrillSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TripletFeel* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Tuplet* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const VibratoSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const VoltaSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const WhammyBarSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void drawTextBase(const TextBase* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TextBlock& textBlock, const TextBase* item, kfw::draw::Painter* painter);
    static void draw(const TextFragment& textFragment, const TextBase* item, kfw::draw::Painter* painter);
    static void drawTextWorkaround(const TextFragment& textFragment, const TextBase* item, kfw::draw::Painter* painter);
    static void drawTextLineBaseSegment(const TextLineBaseSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void setMask(const EngravingItem* item, kfw::draw::Painter* painter);
};
}
