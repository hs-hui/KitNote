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
#ifndef KFW_GLOBAL_CONFIG_H
#define KFW_GLOBAL_CONFIG_H

#include "types/val.h"

namespace kfw {
//! NOTE Some modules have configuration files, this is their model
struct Config {
    Config() = default;
    Config(const ValMap& v)
        : m_data(v) {}

    const Val& value(const std::string& key, const Val& def = Val()) const
    {
        auto it = m_data.find(key);
        if (it != m_data.end()) {
            return it->second;
        }
        return def;
    }

    void set(const std::string& key, const Val& val)
    {
        m_data[key] = val;
    }

    auto begin() { return m_data.begin(); }
    auto end() { return m_data.end(); }
    auto begin() const { return m_data.begin(); }
    auto end() const { return m_data.end(); }
    auto cbegin() const { return m_data.cbegin(); }
    auto cend() const { return m_data.cend(); }

    const ValMap& data() const { return m_data; }

private:
    ValMap m_data;
};
}

#endif // KFW_GLOBAL_CONFIG_H
