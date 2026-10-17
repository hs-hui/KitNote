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
#ifndef KN_ENGRAVING_MODIFYDOM_DEV_H
#define KN_ENGRAVING_MODIFYDOM_DEV_H

#include "layoutcontext.h"

namespace kn::engraving {
class Measure;
}

//! NOTE Here collected functions of DOM modification, witch called on layout stage.
//! They probably need to be called either before the layout or directly during the DOM modification,
//! but not during the layout.
namespace kn::engraving::rendering::score {
class ModifyDom
{
public:

    static void setCrossMeasure(const Measure* measure, LayoutContext& ctx);
    static void connectTremolo(Measure* m);
    static void cmdUpdateNotes(const Measure* measure, const DomAccessor& dom);
    static void createStems(const Measure* measure, LayoutContext& ctx);
    static void setTrackForChordGraceNotes(Measure* measure, const DomAccessor& dom);
    static void sortMeasureSegments(Measure* measure, LayoutContext& ctx);
private:
    static void removeAndAddBeginSegments(Measure* measure);
};
}

#endif // KN_ENGRAVING_MODIFYDOM_DEV_H
