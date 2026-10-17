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

#ifndef KN_ENGRAVING_PALM_MUTE_H
#define KN_ENGRAVING_PALM_MUTE_H

#include "chordtextlinebase.h"

namespace kn::engraving {
class PalmMute;

//---------------------------------------------------------
//   @@ PalmMuteSegment
//---------------------------------------------------------

class PalmMuteSegment final : public TextLineBaseSegment
{
    OBJECT_ALLOCATOR(engraving, PalmMuteSegment)
    DECLARE_CLASSOF(ElementType::PALM_MUTE_SEGMENT)

    Sid getPropertyStyle(Pid) const override;

public:
    PalmMuteSegment(PalmMute* sp, System* parent);

    PalmMuteSegment* clone() const override { return new PalmMuteSegment(*this); }

    PalmMute* palmMute() const { return (PalmMute*)spanner(); }

    friend class PalmMute;
};

//---------------------------------------------------------
//   @@ PalmMute
//---------------------------------------------------------

class PalmMute final : public ChordTextLineBase
{
    OBJECT_ALLOCATOR(engraving, PalmMute)
    DECLARE_CLASSOF(ElementType::PALM_MUTE)

    Sid getPropertyStyle(Pid) const override;

public:
    PalmMute(EngravingItem* parent);

    PalmMute* clone() const override { return new PalmMute(*this); }

//      virtual void write(XmlWriter& xml) const override;

    LineSegment* createLineSegment(System* parent) override;
    PropertyValue propertyDefault(Pid propertyId) const override;

    void setChannel();

    friend class PalmMuteLine;
};
} // namespace kn::engraving
#endif
