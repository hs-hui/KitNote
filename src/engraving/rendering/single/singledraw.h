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

#include "../paintoptions.h"

namespace kfw::draw {
class Painter;
}

namespace kn::engraving {
class MStyle;
class IEngravingFont;
class Score;
class EngravingItem;

class Accidental;
class ActionIcon;
class Ambitus;
class Arpeggio;
class Articulation;

class BagpipeEmbellishment;
class BarLine;
class Beam;
class Bend;
class Bracket;
class Breath;

class Capo;
class Chord;
class ChordBracket;
class ChordLine;
class Clef;

class DeadSlapped;
class Dynamic;

class Expression;

class Fermata;
class Fingering;
class FiguredBass;
class FiguredBassItem;
class FretDiagram;
class FSymbol;

class Glissando;
class GlissandoSegment;
class GradualTempoChange;
class GradualTempoChangeSegment;
class GuitarBend;
class GuitarBendSegment;

class Hairpin;
class HairpinSegment;
class HammerOnPullOffSegment;
class HammerOnPullOffText;
class HarpPedalDiagram;
class HarmonicMarkSegment;
class Harmony;
class Hook;

class Image;

class InstrumentChange;
class InstrumentName;

class Jump;

class KeySig;

class LayoutBreak;
class LedgerLine;
class LetRing;
class LetRingSegment;
class SLine;
class LineSegment;
class Lyrics;

class Marker;
class MeasureNumber;
class MeasureRepeat;

class Note;
class NoteHead;
class NoteLineSegment;

class Ornament;
class Ottava;
class OttavaSegment;

class PalmMute;
class PalmMuteSegment;
class Pedal;
class PedalSegment;
class PickScrapeSegment;
class PlayCountText;
class PlayTechAnnotation;

class RasgueadoSegment;
class RehearsalMark;
class Rest;

class ShadowNote;
class Slur;
class SlurSegment;
class SoundFlag;
class Spacer;
class StaffLines;
class StaffState;
class StaffText;
class StaffTypeChange;
class Stem;
class StemSlash;
class Sticking;
class StringTunings;
class Symbol;
class SystemDivider;
class SystemText;

class TabDurationSymbol;
class Tapping;
class TempoText;
class Text;
class TextBase;
class TextBlock;
class TextFragment;
class TextLine;
class TextLineSegment;
class TextLineBaseSegment;
class TieSegment;
class TimeSig;
class TremoloSingleChord;
class TremoloTwoChord;
class TremoloBar;
class Trill;
class TrillSegment;
class TripletFeel;
class Tuplet;

class Vibrato;
class VibratoSegment;
class Volta;
class VoltaSegment;

class WhammyBarSegment;
}

namespace kn::engraving::rendering::single {
class SingleDraw
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

    static void draw(const LayoutBreak* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const LedgerLine* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const LetRingSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Lyrics* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Marker* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const MeasureNumber* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const MeasureRepeat* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Note* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const NoteHead* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const NoteLineSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Ornament* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const OttavaSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const PalmMuteSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PedalSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PickScrapeSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PlayCountText* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const PlayTechAnnotation* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const RasgueadoSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const RehearsalMark* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Rest* item, kfw::draw::Painter* painter, const PaintOptions& opt);

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
    static void draw(const SoundFlag* item, kfw::draw::Painter* painter, const PaintOptions& opt);

    static void draw(const Tapping* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TempoText* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const Text* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TextLineSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TieSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
    static void draw(const TimeSig* item, kfw::draw::Painter* painter, const PaintOptions& opt);
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
    static void drawTextLineBaseSegment(const TextLineBaseSegment* item, kfw::draw::Painter* painter, const PaintOptions& opt);
};
}
