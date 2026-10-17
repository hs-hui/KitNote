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
#ifndef KFW_GLOBAL_DEFER_H
#define KFW_GLOBAL_DEFER_H

#include <functional>

namespace kfw {
struct Defer
{
    template<class F>
    Defer(F f)
        : m_f(f) {}

    ~Defer()
    {
        if (m_f) {
            m_f();
        }
    }

    std::function<void()> m_f;
};
}

#define CONCAT_IMPL(x, y) x##y
#define MACRO_CONCAT(x, y) CONCAT_IMPL(x, y)

#define DEFER const ::kfw::Defer MACRO_CONCAT(defer_, __LINE__) = [&]()

#endif // KFW_GLOBAL_DEFER_H
