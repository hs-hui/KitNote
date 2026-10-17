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

#ifndef KN_ENGRAVING_MEASURENUMBER_H
#define KN_ENGRAVING_MEASURENUMBER_H

#include "measurenumberbase.h"

namespace kn::engraving {
//---------------------------------------------------------
//   MeasureNumber
//---------------------------------------------------------

class MeasureNumber : public MeasureNumberBase
{
    OBJECT_ALLOCATOR(engraving, MeasureNumber)
    DECLARE_CLASSOF(ElementType::MEASURE_NUMBER)

public:
    MeasureNumber(Measure* parent = nullptr, TextStyleType tid = TextStyleType::MEASURE_NUMBER);
    MeasureNumber(const MeasureNumber& other);

    virtual MeasureNumber* clone() const override { return new MeasureNumber(*this); }

    bool isSystemObjectBelowBottomStaff() const override;
};
} // namespace kn::engraving

#endif
