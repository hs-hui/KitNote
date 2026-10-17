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

#include "layoutcontext.h"
#include "dom/textbase.h"

namespace kn::engraving::rendering::score {
class TextLayout
{
public:
    static void layoutBaseTextBase(const TextBase* item, TextBase::LayoutData* data);
    static void layoutBaseTextBase(TextBase* item, LayoutContext& ctx);
    static void layoutBaseTextBase1(const TextBase* item, TextBase::LayoutData* data);
    static void layoutBaseTextBase1(TextBase* item, const LayoutContext& ctx);

    static void computeTextHighResShape(const TextBase* item, TextBase::LayoutData* ldata);
    static void layoutTextBlock(TextBlock* item, const TextBase* t);
private:
    static void textHorizontalLayout(const TextBase* item, Shape& shape, double maxBlockWidth, TextBase::LayoutData* ldata);
    static void justifyLine(const TextBase* item, TextBlock* textBlock, double maxBlockWidth);
    static double musicSymbolBaseLineAdjust(const TextBlock* block, const TextBase* t, const TextFragment& f,
                                            const std::list<TextFragment>::iterator fi);
};
}
