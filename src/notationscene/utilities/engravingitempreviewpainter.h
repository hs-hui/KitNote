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

#include <memory>

#include "engraving/dom/engravingitem.h"
#include "engraving/rendering/isinglerenderer.h"

namespace kn::notation {
class EngravingItemPreviewPainter
{
public:
    struct PaintParams
    {
        kfw::draw::Painter* painter = nullptr;

        kn::engraving::Color color;

        double mag = 1.0;
        double xoffset = 0.0;
        double yoffset = 0.0;

        kfw::RectF rect;
        double spatium = 1.0;

        bool useElementColors = false;
        bool colorsInversionEnabled = false;
        int numStaffLines = 0;
    };

    static void paintPreview(std::shared_ptr<engraving::rendering::ISingleRenderer> render, kn::engraving::EngravingItem* element,
                             PaintParams& params);
    static void paintItem(std::shared_ptr<engraving::rendering::ISingleRenderer> render, kn::engraving::EngravingItem* element,
                          PaintParams& params);

private:
    static double paintStaff(PaintParams& params);
};
}
