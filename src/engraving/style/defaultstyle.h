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
#ifndef KN_ENGRAVING_DEFAULTSTYLE_H
#define KN_ENGRAVING_DEFAULTSTYLE_H

#include "global/io/path.h"

#include "style.h"

namespace kn::engraving {
class DefaultStyle
{
public:
    static DefaultStyle* instance();

    void init(const kfw::io::path_t& defaultStyleFilePath, const kfw::io::path_t& partStyleFilePath, const SizeF& defaultPageSize);

    static const MStyle& baseStyle();

    static const MStyle& defaultStyle();
    static const MStyle* defaultStyleForParts();

    static const MStyle& resolveStyleDefaults(const int defaultsVersion);

private:
    DefaultStyle() = default;

    static bool doLoadStyle(MStyle* style, const kfw::io::path_t& filePath);

    MStyle m_baseStyle; // builtin initial style
    MStyle m_defaultStyle; // builtin modified by preferences
    MStyle* m_defaultStyleForParts = nullptr;
};
}

#endif // KN_ENGRAVING_DEFAULTSTYLE_H
