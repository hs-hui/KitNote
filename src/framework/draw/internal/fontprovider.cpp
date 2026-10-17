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
#include "fontprovider.h"

#include "log.h"

using namespace kfw;
using namespace kfw::draw;

int FontProvider::addSymbolFont(const kfw::String& family, const io::path_t& path)
{
    UNUSED(family);
    UNUSED(path);
    return 1;
}

double FontProvider::lineSpacing(const kfw::draw::Font& f) const
{
    return fontsEngine()->lineSpacing(f);
}

double FontProvider::xHeight(const kfw::draw::Font& f) const
{
    return fontsEngine()->xHeight(f);
}

double FontProvider::height(const kfw::draw::Font& f) const
{
    return fontsEngine()->height(f);
}

double FontProvider::ascent(const kfw::draw::Font& f) const
{
    return fontsEngine()->ascent(f);
}

double FontProvider::capHeight(const kfw::draw::Font& f) const
{
    return fontsEngine()->capHeight(f);
}

double FontProvider::descent(const kfw::draw::Font& f) const
{
    return fontsEngine()->descent(f);
}

bool FontProvider::inFont(const kfw::draw::Font& f, char32_t ucs4) const
{
    return fontsEngine()->inFont(f, ucs4);
}

// Text
double FontProvider::horizontalAdvance(const kfw::draw::Font& f, const kfw::String& string) const
{
    return fontsEngine()->horizontalAdvance(f, string.toStdU32String());
}

double FontProvider::horizontalAdvance(const kfw::draw::Font& f, char32_t ucs4) const
{
    return fontsEngine()->horizontalAdvance(f, ucs4);
}

RectF FontProvider::boundingRect(const kfw::draw::Font& f, const kfw::String& string) const
{
    return fontsEngine()->boundingRect(f, string.toStdU32String());
}

RectF FontProvider::boundingRect(const kfw::draw::Font& f, char32_t ucs4) const
{
    return fontsEngine()->boundingRect(f, ucs4);
}

RectF FontProvider::tightBoundingRect(const kfw::draw::Font& f, const kfw::String& string) const
{
    return fontsEngine()->tightBoundingRect(f, string.toStdU32String());
}
