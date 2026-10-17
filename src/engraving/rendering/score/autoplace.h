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
#ifndef KN_ENGRAVING_AUTOPLACE_DEV_H
#define KN_ENGRAVING_AUTOPLACE_DEV_H

#include "dom/articulation.h"
#include "dom/engravingitem.h"
#include "dom/spanner.h"

namespace kn::engraving::rendering::score {
class Autoplace
{
public:

    static void autoplaceSegmentElement(const EngravingItem* item, EngravingItem::LayoutData* ldata, bool above, bool add);
    static void autoplaceMeasureElement(const EngravingItem* item, EngravingItem::LayoutData* ldata, bool above, bool add);
    static void autoplaceSegmentElement(const EngravingItem* item, EngravingItem::LayoutData* ldata, bool add = true)
    {
        autoplaceSegmentElement(item, ldata, item->placeAbove(), add);
    }

    static void autoplaceMeasureElement(const EngravingItem* item, EngravingItem::LayoutData* ldata, bool add = true)
    {
        autoplaceMeasureElement(item, ldata, item->placeAbove(), add);
    }

    static void autoplaceSpannerSegment(const SpannerSegment* item, SpannerSegment::LayoutData* ldata, double spatium);

    static double rebaseOffset(const EngravingItem* item, EngravingItem::LayoutData* ldata, bool nox = true);
    static bool rebaseMinDistance(const EngravingItem* item, EngravingItem::LayoutData* ldata, double& md, double& yd, double sp,
                                  double rebase, bool above, bool fix);

    static void setOffsetChanged(const EngravingItem* item, EngravingItem::LayoutData* ldata, bool v, bool absolute = true,
                                 const PointF& diff = PointF());

    static bool itemsShouldIgnoreEachOther(const EngravingItem* itemToAutoplace, const EngravingItem* itemInSkyline);
};
}

#endif // KN_ENGRAVING_AUTOPLACE_DEV_H
