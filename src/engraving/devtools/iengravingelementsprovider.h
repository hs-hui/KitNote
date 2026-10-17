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
#ifndef KN_ENGRAVING_IENGRAVINGELEMENTSPROVIDER_H
#define KN_ENGRAVING_IENGRAVINGELEMENTSPROVIDER_H

#include <unordered_set>

#include "modularity/imoduleinterface.h"
#include "async/channel.h"

namespace kn::engraving {
class EngravingObject;
using EngravingObjectSet = std::unordered_set<const EngravingObject*>;
class IEngravingElementsProvider : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IEngravingElementsProvider)
public:
    virtual ~IEngravingElementsProvider() = default;

    // statistic
    virtual void clearStatistic() = 0;
    virtual void printStatistic(const std::string& title) = 0;

    // register
    virtual void reg(const kn::engraving::EngravingObject* e) = 0;
    virtual void unreg(const kn::engraving::EngravingObject* e) = 0;
    virtual const EngravingObjectSet& elements() const = 0;

    // debug draw
    virtual void select(const kn::engraving::EngravingObject* e, bool arg) = 0;
    virtual bool isSelected(const kn::engraving::EngravingObject* e) const = 0;
    virtual kfw::async::Channel<const kn::engraving::EngravingObject*, bool> selectChanged() const = 0;
};
}

#endif // KN_ENGRAVING_IENGRAVINGELEMENTSPROVIDER_H
