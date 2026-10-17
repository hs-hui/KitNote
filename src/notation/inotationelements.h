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

#include <vector>

#include "notationtypes.h"

namespace kn::notation {
class INotationElements
{
public:
    virtual ~INotationElements() = default;

    virtual kn::engraving::Score* msScore() const = 0;

    virtual std::vector<EngravingItem*> search(const QString& searchText) const = 0;
    virtual std::vector<EngravingItem*> elements(const FilterElementsOptions& elementOptions = FilterElementsOptions()) const = 0;

    virtual Measure* measure(const int measureIndex) const = 0;

    virtual const PageList& pages() const = 0;
    virtual const Page* pageByPoint(const kfw::PointF& point) const = 0;
};

using INotationElementsPtr = std::shared_ptr<INotationElements>;
}
