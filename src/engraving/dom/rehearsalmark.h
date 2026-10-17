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

#ifndef KN_ENGRAVING_REHEARSALMARK_H
#define KN_ENGRAVING_REHEARSALMARK_H

#include "textbase.h"

namespace kn::engraving {
//---------------------------------------------------------
//   @@ RehearsalMark
//---------------------------------------------------------

class RehearsalMark final : public TextBase
{
    OBJECT_ALLOCATOR(engraving, RehearsalMark)
    DECLARE_CLASSOF(ElementType::REHEARSAL_MARK)

public:
    enum class Type : unsigned char {
        Main = 0,
        Additional
    };

    RehearsalMark(Segment* parent);

    bool isEditAllowed(EditData&) const override;
    bool allowTimeAnchor() const override { return false; }
    RectF drag(EditData& ed) override;

    RehearsalMark* clone() const override { return new RehearsalMark(*this); }

    Segment* segment() const { return (Segment*)explicitParent(); }

    PropertyValue propertyDefault(Pid id) const override;

    void setType(Type type);
    Type type() const { return m_type; }

    void styleChanged() override;

    bool positionRelativeToNoteheadRest() const override { return false; }

private:
    void applyTypeStyle();

    Type m_type = Type::Main;
};
} // namespace kn::engraving
#endif
