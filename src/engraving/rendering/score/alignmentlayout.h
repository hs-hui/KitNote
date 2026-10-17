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

#ifndef KN_ENGRAVING_ALIGNMENTLAYOUT_DEV_H
#define KN_ENGRAVING_ALIGNMENTLAYOUT_DEV_H

#include <vector>
#include <set>
#include <functional>

namespace kn::engraving {
class EngravingItem;
class HammerOnPullOff;
class System;
}

namespace kn::engraving::rendering::score {
class AlignmentLayout
{
public:
    static void alignItemsWithTheirSnappingChain(const std::vector<EngravingItem*>& elements, const System* system);
    static void alignStaffCenteredItems(const std::vector<EngravingItem*>& elements, const System* system);
    static void alignItemsForSystem(const std::vector<EngravingItem*>& elements, const System* system);
    static void alignItemsGroup(const std::vector<EngravingItem*>& elements, const System* system);
    static void alignHopoLetters(const HammerOnPullOff* hopo, const System* system);

private:
    static void moveItemToY(EngravingItem* item, double y, const System* system);
    static double yOpticalCenter(const EngravingItem* item);
    static void scanConnectedItems(EngravingItem* item, const System* system, std::function<void(EngravingItem*)> func);
    static double computeAverageY(const std::vector<double>& vecOfY);
    static bool isAbove(const EngravingItem* item);
};
} // namespace kn::engraving::rendering::dev
#endif // KN_ENGRAVING_ALIGNMENTLAYOUT_DEV_H
