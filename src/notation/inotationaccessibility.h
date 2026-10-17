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

#ifndef KN_NOTATION_INOTATIONACCESSIBILITY_H
#define KN_NOTATION_INOTATIONACCESSIBILITY_H

#include "types/retval.h"
#include "notationtypes.h"

#include "engraving/accessibility/accessibleroot.h"

namespace kn::notation {
class INotationAccessibility
{
public:
    virtual ~INotationAccessibility() = default;

    virtual kfw::ValCh<std::string> accessibilityInfo() const = 0;

    virtual void setMapToScreenFunc(const kn::engraving::AccessibleMapToScreenFunc& func) = 0;

    virtual void setEnabled(bool enabled) = 0;
};

using INotationAccessibilityPtr = std::shared_ptr<INotationAccessibility>;
}

#endif // KN_NOTATION_INOTATIONACCESSIBILITY_H
