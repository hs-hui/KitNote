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

#ifndef KN_ENGRAVING_PARSERBASE_H
#define KN_ENGRAVING_PARSERBASE_H

#include "log.h"
#include "mpe/mpetypes.h"

#include "playback/renderingcontext.h"

namespace kn::engraving {
class EngravingItem;

template<class T>
class MetaParserBase
{
public:
    static void parse(const EngravingItem* item, const RenderingContext& context, kfw::mpe::ArticulationMap& result)
    {
        IF_ASSERT_FAILED(item) {
            return;
        }

        T::doParse(item, context, result);
    }

protected:
    static void appendArticulationData(kfw::mpe::ArticulationMeta&& meta, kfw::mpe::ArticulationMap& result)
    {
        result.emplace(meta.type,
                       kfw::mpe::ArticulationAppliedData(std::forward<kfw::mpe::ArticulationMeta>(meta), 0, kfw::mpe::HUNDRED_PERCENT));
    }
};
}

#endif // KN_ENGRAVING_PARSERBASE_H
