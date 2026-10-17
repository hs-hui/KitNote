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

#include "../ieditmoderenderer.h"

namespace kfw::draw {
class Painter;
}

namespace kn::engraving {
class MStyle;
class IEngravingFont;
class Score;
class EngravingItem;
class EditData;

class BarLine;
class Dynamic;
class SlurTieSegment;
class TextBase;
class TextBlock;
class TextFragment;
}

namespace kn::engraving::rendering::editmode {
class EditModeRenderer : public IEditModeRenderer
{
public:
    EditModeRenderer() = default;

    void drawItem(const EngravingItem* item, kfw::draw::Painter* p, const EditData& ed, double currentViewScaling,
                  const PaintOptions& opt) override;

private:
    static void drawEngravingItem(const EngravingItem* item, kfw::draw::Painter* painter, const EditData& ed, double currentViewScaling,
                                  const PaintOptions& opt);

    static void drawBarline(const BarLine* item, kfw::draw::Painter* painter, const EditData& ed, double currentViewScaling,
                            const PaintOptions& opt);
    static void drawDynamic(const Dynamic* item, kfw::draw::Painter* painter, const EditData& ed, double currentViewScaling,
                            const PaintOptions& opt);
    static void drawSlurTieSegment(const SlurTieSegment* item, kfw::draw::Painter* painter, const EditData& ed, double currentViewScaling,
                                   const PaintOptions& opt);

    static void drawTextBase(const TextBase* item, kfw::draw::Painter* painter, const EditData& ed, double currentViewScaling,
                             const PaintOptions& opt);
    static void draw(const TextBlock& textBlock, const TextBase* item, kfw::draw::Painter* painter);
    static void draw(const TextFragment& textFragment, const TextBase* item, kfw::draw::Painter* painter);
};
}
