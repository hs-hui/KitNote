/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include "notationtypes.h"
#include "async/notification.h"

namespace kn::notation {
class INotationStyle
{
public:
    virtual ~INotationStyle() = default;

    virtual const PropertyValue& styleValue(const StyleId& styleId) const = 0;
    virtual const PropertyValue& defaultStyleValue(const StyleId& styleId) const = 0;
    virtual void setStyleValue(const StyleId& styleId, const PropertyValue& newValue) = 0;
    virtual void resetStyleValue(const StyleId& styleId) = 0;
    virtual void resetStyleValues(const std::vector<StyleId>& styleIds) = 0;

    virtual bool canApplyToAllParts() const = 0;
    virtual void applyToAllParts() = 0;

    virtual void resetAllStyleValues(const StyleIdSet& exceptTheseOnes = {}) = 0;

    virtual kfw::async::Notification styleChanged() const = 0;

    virtual bool loadStyle(const kfw::io::path_t&, bool allowAnyVersion) = 0;
    virtual bool saveStyle(const kfw::io::path_t&) = 0;
};

using INotationStylePtr = std::shared_ptr<INotationStyle>;
}
