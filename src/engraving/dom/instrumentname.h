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

#ifndef KN_ENGRAVING_INSTRUMENTNAME_H
#define KN_ENGRAVING_INSTRUMENTNAME_H

#include "textbase.h"

namespace kn::engraving {
enum class InstrumentNameType : char {
    LONG, SHORT
};

class System;
class SysStaff;

//---------------------------------------------------------
//   InstrumentName
//---------------------------------------------------------

class InstrumentName final : public TextBase
{
    OBJECT_ALLOCATOR(engraving, InstrumentName)
    DECLARE_CLASSOF(ElementType::INSTRUMENT_NAME)

public:
    InstrumentName(System*);

    InstrumentName* clone() const override { return new InstrumentName(*this); }

    int layoutPos() const { return m_layoutPos; }
    void setLayoutPos(int val) { m_layoutPos = val; }

    String instrumentNameTypeName() const;
    InstrumentNameType instrumentNameType() const { return m_instrumentNameType; }
    void setInstrumentNameType(InstrumentNameType v);
    void setInstrumentNameType(const String& s);

    System* system() const { return toSystem(explicitParent()); }

    SysStaff* sysStaff() const { return m_sysStaff; }
    void setSysStaff(SysStaff* s) { m_sysStaff = s; }

    double largestStaffSpatium() const;

    bool isEditable() const override { return false; }
    PropertyValue getProperty(Pid propertyId) const override;
    bool setProperty(Pid propertyId, const PropertyValue&) override;
    PropertyValue propertyDefault(Pid) const override;

    bool positionRelativeToNoteheadRest() const override { return false; }

private:

    InstrumentNameType m_instrumentNameType = InstrumentNameType::LONG;
    int m_layoutPos = 0;
    SysStaff* m_sysStaff = nullptr;
};
} // namespace kn::engraving
#endif
