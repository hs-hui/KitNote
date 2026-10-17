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
#ifndef KN_ENGRAVING_DRAWDATACONVERTER_H
#define KN_ENGRAVING_DRAWDATACONVERTER_H

#include "global/types/ret.h"
#include "global/io/path.h"
#include "draw/types/drawdata.h"
#include "draw/types/pixmap.h"

namespace kn::engraving {
class DrawDataConverter
{
public:
    DrawDataConverter() = default;

    kfw::Ret drawDataToPng(const kfw::io::path_t& dataFile, const kfw::io::path_t& outFile);
    kfw::Ret drawDiffToPng(const kfw::io::path_t& diffFile, const kfw::io::path_t& refFile, const kfw::io::path_t& outFile);

    kfw::Ret saveAsPng(const kfw::draw::DrawDataPtr& data, const kfw::io::path_t& path);

    kfw::draw::Pixmap drawDataToPixmap(const kfw::draw::DrawDataPtr& data);
    void drawOnPixmap(kfw::draw::Pixmap& px, const kfw::draw::DrawDataPtr& data, const kfw::draw::Color& overlay = kfw::draw::Color());
};
}

#endif // KN_ENGRAVING_DRAWDATACONVERTER_H
