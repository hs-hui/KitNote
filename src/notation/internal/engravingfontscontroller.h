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

#include "async/asyncable.h"
#include "modularity/ioc.h"
#include "inotationconfiguration.h"
#include "draw/internal/ifontsdatabase.h"
#include "ui/iuiconfiguration.h"

namespace kn::notation {
class EngravingFontsController : public kfw::async::Asyncable
{
    kfw::GlobalInject<kfw::draw::IFontsDatabase> fontsDatabase;
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::GlobalInject<kn::notation::INotationConfiguration> configuration;
    kfw::GlobalInject<kn::engraving::IEngravingFontsProvider> engravingFonts;

public:
    void init();

private:
    void scanAllDirectories() const;
    void scanDirectory(const kfw::io::path_t& path, bool isPrivate) const;
    kfw::io::path_t findFontPathGlobal(const QString& fontName) const;
    kfw::io::path_t findFontPathPrivate(const QString& metadataDir, const QString& fontName) const;
};
}
