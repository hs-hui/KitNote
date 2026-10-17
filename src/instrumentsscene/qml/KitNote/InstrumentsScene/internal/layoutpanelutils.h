/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include "engraving/dom/staff.h"
#include "engraving/style/style.h"
#include "engraving/dom/utils.h"
#include "engraving/types/typesconv.h"

namespace kn::instrumentsscene {
struct SystemObjectsGroup {
    kn::engraving::ElementType type = kn::engraving::ElementType::INVALID;
    std::vector<kn::engraving::EngravingItem*> items;
    kn::engraving::Staff* staff = nullptr;
};

using SystemObjectGroups = std::vector<SystemObjectsGroup>;
using SystemObjectGroupsByStaff = std::map<const kn::engraving::Staff*, SystemObjectGroups>;

inline SystemObjectGroupsByStaff collectSystemObjectGroups(const std::vector<kn::engraving::Staff*>& staves)
{
    if (staves.empty()) {
        return {};
    }

    const std::vector<engraving::EngravingItem*> systemObjects = engraving::collectSystemObjects(staves.front()->score(), staves);
    SystemObjectGroupsByStaff result;

    for (engraving::EngravingItem* obj : systemObjects) {
        SystemObjectGroups& groups = result[obj->staff()];

        auto it = std::find_if(groups.begin(), groups.end(), [obj](const SystemObjectsGroup& group) {
            return group.type == obj->type();
        });

        if (it != groups.end()) {
            it->items.push_back(obj);
        } else {
            groups.emplace_back(SystemObjectsGroup { obj->type(), { obj }, obj->staff() });
        }
    }

    const engraving::MStyle& style = staves.front()->style();
    bool collectMeasureNumbers = style.styleV(engraving::Sid::measureNumberPlacementMode).value<engraving::MeasureNumberPlacement>()
                                 == engraving::MeasureNumberPlacement::ON_SYSTEM_OBJECT_STAVES;
    if (collectMeasureNumbers) {
        for (kn::engraving::Staff* staff : staves) {
            SystemObjectGroups& systemObjectGroups = result[staff];
            systemObjectGroups.push_back(SystemObjectsGroup { kn::engraving::ElementType::MEASURE_NUMBER, {}, staff });
        }
    }

    return result;
}

inline SystemObjectGroups collectSystemObjectGroups(const kn::engraving::Staff* systemObjectsStaff)
{
    const std::vector<kn::engraving::Staff*> staves {
        const_cast<kn::engraving::Staff*>(systemObjectsStaff)
    };

    SystemObjectGroupsByStaff systemObjects = collectSystemObjectGroups(staves);
    return systemObjects[systemObjectsStaff];
}

inline bool isSystemObjectsGroupVisible(const SystemObjectsGroup& group)
{
    if (group.type == kn::engraving::ElementType::MEASURE_NUMBER) {
        return group.staff && group.staff->shouldShowMeasureNumbers();
    }

    for (const kn::engraving::EngravingItem* item : group.items) {
        if (item->visible()) {
            return true;
        }
    }

    return false;
}

inline kfw::String translatedSystemObjectsGroupName(const SystemObjectsGroup& group)
{
    const kfw::TranslatableString& name = kn::engraving::TConv::userName(group.type);
    const int n = static_cast<int>(group.items.size());

    return name.translated(n);
}

inline kfw::String translatedSystemObjectsGroupCapitalizedName(const SystemObjectsGroup& group)
{
    const kfw::TranslatableString& name = kn::engraving::TConv::capitalizedUserName(group.type);
    const int n = static_cast<int>(group.items.size());

    return name.translated(n);
}
}
