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
#ifndef KN_ENGRAVING_SCOREACCESS_H
#define KN_ENGRAVING_SCOREACCESS_H

#include "../dom/masterscore.h"

//! NOTE This is a temporary class for controlling (master)score access
//! See Project class description for detail

namespace kn::engraving::compat {
class ScoreAccess
{
public:

    static MasterScore* createMasterScore(const kfw::modularity::ContextPtr& iocCtx);
    static MasterScore* createMasterScoreWithBaseStyle(const kfw::modularity::ContextPtr& iocCtx);
    static MasterScore* createMasterScoreWithDefaultStyle(const kfw::modularity::ContextPtr& iocCtx);
    static MasterScore* createMasterScore(const kfw::modularity::ContextPtr& iocCtx, const MStyle& style);
};
}

#endif // KN_ENGRAVING_SCOREACCESS_H
