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
#ifndef KN_PALETTE_PALETTEWORKSPACESETUP_H
#define KN_PALETTE_PALETTEWORKSPACESETUP_H

#include "modularity/ioc.h"
#include "workspace/iworkspacesdataprovider.h"
#include "ipaletteprovider.h"
#include "async/asyncable.h"

namespace kn::palette {
class PaletteWorkspaceSetup : public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::ContextInject<kfw::workspace::IWorkspacesDataProvider> workspacesDataProvider = { this };
    kfw::ContextInject<IPaletteProvider> paletteProvider = { this };

public:

    explicit PaletteWorkspaceSetup(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    void setup();

private:
    PaletteTreePtr readPalette(const kfw::ByteArray& data, const kfw::modularity::ContextPtr& iocCtx);
    void writePalette(const PaletteTreePtr& tree, QByteArray& data);
};
}

#endif // KN_PALETTE_PALETTEWORKSPACESETUP_H
