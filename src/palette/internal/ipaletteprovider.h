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

#include "modularity/imoduleinterface.h"

#include "async/notification.h"

#include "palettetree.h"

namespace kn::palette {
class IPaletteProvider : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IPaletteAdapter)

public:
    virtual ~IPaletteProvider() = default;

    virtual void init() = 0;

    virtual PaletteTreePtr userPaletteTree() const = 0;
    virtual kfw::async::Notification userPaletteTreeChanged() const = 0;
    virtual void setUserPaletteTree(PaletteTreePtr tree) = 0;
    virtual void setDefaultPaletteTree(PaletteTreePtr tree) = 0;

    virtual kfw::async::Channel<kn::engraving::ElementPtr> addCustomItemRequested() const = 0;
};
}
