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

#include <QObject>
#include <qqmlintegration.h>

#include "engraving/types/types.h"

namespace kn::inspector {
namespace TextTypes {
Q_NAMESPACE;
QML_ELEMENT;

enum class FontStyle {
    //FONT_STYLE_UNDEFINED = -1,
    FONT_STYLE_NORMAL = 0,
    FONT_STYLE_BOLD = 1 << 0,
    FONT_STYLE_ITALIC = 1 << 1,
    FONT_STYLE_UNDERLINE = 1 << 2,
    FONT_STYLE_STRIKE = 1 << 3
};
Q_ENUM_NS(FontStyle)

enum class FontHorizontalAlignment {
    FONT_ALIGN_H_LEFT = 0,
    FONT_ALIGN_H_RIGHT = 1,
    FONT_ALIGN_H_CENTER = 2,
    FONT_ALIGN_H_JUSTIFY = 3,
};
Q_ENUM_NS(FontHorizontalAlignment)

enum class FontVerticalAlignment {
    FONT_ALIGN_V_TOP = 0,
    FONT_ALIGN_V_CENTER = 1,
    FONT_ALIGN_V_BOTTOM = 2,
    FONT_ALIGN_V_BASELINE = 3
};
Q_ENUM_NS(FontVerticalAlignment)

enum class FrameType {
    FRAME_TYPE_NONE = 0,
    FRAME_TYPE_SQUARE,
    FRAME_TYPE_CIRCLE
};
Q_ENUM_NS(FrameType)

// must match kn::engraving::Tid
enum class TextType {
    TEXT_TYPE_DEFAULT,
    TEXT_TYPE_TITLE,
    TEXT_TYPE_SUBTITLE,
    TEXT_TYPE_COMPOSER,
    TEXT_TYPE_POET,
    TEXT_TYPE_TRANSLATOR,
    TEXT_TYPE_FRAME,
    TEXT_TYPE_INSTRUMENT_EXCERPT,
    TEXT_TYPE_INSTRUMENT_LONG,
    TEXT_TYPE_INSTRUMENT_SHORT,
    TEXT_TYPE_INSTRUMENT_CHANGE,
    TEXT_TYPE_HEADER,
    TEXT_TYPE_FOOTER,
    TEXT_TYPE_COPYRIGHT,
    TEXT_TYPE_PAGE_NUMBER,
    TEXT_TYPE_MEASURE_NUMBER,
    TEXT_TYPE_MMREST_RANGE,
    TEXT_TYPE_TEMPO,
    TEXT_TYPE_TEMPO_CHANGE,
    TEXT_TYPE_METRONOME,
    TEXT_TYPE_REPEAT_LEFT,       // align to start of measure
    TEXT_TYPE_REPEAT_RIGHT,      // align to end of measure
    TEXT_TYPE_REHEARSAL_MARK,
    TEXT_TYPE_SYSTEM,
    TEXT_TYPE_STAFF,
    TEXT_TYPE_EXPRESSION,
    TEXT_TYPE_DYNAMICS,
    TEXT_TYPE_HAIRPIN,
    TEXT_TYPE_LYRICS_ODD,
    TEXT_TYPE_LYRICS_EVEN,
    TEXT_TYPE_HARMONY_A,
    TEXT_TYPE_HARMONY_B,
    TEXT_TYPE_HARMONY_ROMAN,
    TEXT_TYPE_HARMONY_NASHVILLE,
    TEXT_TYPE_TUPLET,
    TEXT_TYPE_STICKING,
    TEXT_TYPE_FINGERING,
    TEXT_TYPE_LH_GUITAR_FINGERING,
    TEXT_TYPE_RH_GUITAR_FINGERING,
    TEXT_TYPE_STRING_NUMBER,
    TEXT_TYPE_HARP_PEDAL_DIAGRAM,
    TEXT_TYPE_HARP_PEDAL_TEXT_DIAGRAM,
    TEXT_TYPE_TEXTLINE,
    TEXT_TYPE_VOLTA,
    TEXT_TYPE_OTTAVA,
    TEXT_TYPE_GLISSANDO,
    TEXT_TYPE_PEDAL,
    TEXT_TYPE_BEND,
    TEXT_TYPE_LET_RING,
    TEXT_TYPE_PALM_MUTE,
    TEXT_TYPE_USER1,
    TEXT_TYPE_USER2,
    TEXT_TYPE_USER3,
    TEXT_TYPE_USER4,
    TEXT_TYPE_USER5,
    TEXT_TYPE_USER6,
    TEXT_TYPE_USER7,
    TEXT_TYPE_USER8,
    TEXT_TYPE_USER9,
    TEXT_TYPE_USER10,
    TEXT_TYPE_USER11,
    TEXT_TYPE_USER12
};
Q_ENUM_NS(TextType)

enum class TextSubscriptMode {
    TEXT_SUBSCRIPT_NORMAL = 0,
    TEXT_SUBSCRIPT_TOP,
    TEXT_SUBSCRIPT_BOTTOM
};
Q_ENUM_NS(TextSubscriptMode)
}

static const QList<kn::engraving::ElementType> TEXT_ELEMENT_TYPES = {
    kn::engraving::ElementType::TEXT,
    kn::engraving::ElementType::STAFF_TEXT,
    kn::engraving::ElementType::SYSTEM_TEXT,
    kn::engraving::ElementType::TRIPLET_FEEL,
    kn::engraving::ElementType::DYNAMIC,
    kn::engraving::ElementType::EXPRESSION,
    kn::engraving::ElementType::FIGURED_BASS,
    kn::engraving::ElementType::FINGERING,
    kn::engraving::ElementType::HARMONY,
    kn::engraving::ElementType::INSTRUMENT_CHANGE,
    kn::engraving::ElementType::JUMP,
    kn::engraving::ElementType::LYRICS,
    kn::engraving::ElementType::MARKER,
    kn::engraving::ElementType::MEASURE_NUMBER,
    kn::engraving::ElementType::REHEARSAL_MARK,
    kn::engraving::ElementType::STICKING,
    kn::engraving::ElementType::TEMPO_TEXT,
    kn::engraving::ElementType::TUPLET,
    kn::engraving::ElementType::PLAY_COUNT_TEXT,
    kn::engraving::ElementType::PLAYTECH_ANNOTATION,
    kn::engraving::ElementType::CAPO,
    kn::engraving::ElementType::STRING_TUNINGS,
    kn::engraving::ElementType::HARP_DIAGRAM,
    kn::engraving::ElementType::SOUND_FLAG,
    kn::engraving::ElementType::HAMMER_ON_PULL_OFF_TEXT,
};
}
