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
#include "articulationprofilesrepositorystub.h"

using namespace kfw;
using namespace kfw::mpe;

ArticulationsProfilePtr ArticulationProfilesRepositoryStub::createNew() const
{
    return std::make_shared<ArticulationsProfile>();
}

ArticulationsProfilePtr ArticulationProfilesRepositoryStub::defaultProfile(const ArticulationFamily) const
{
    return std::make_shared<ArticulationsProfile>();
}

ArticulationsProfilePtr ArticulationProfilesRepositoryStub::loadProfile(const io::path_t&) const
{
    return std::make_shared<ArticulationsProfile>();
}

void ArticulationProfilesRepositoryStub::saveProfile(const io::path_t&, const ArticulationsProfilePtr)
{
}

async::Channel<io::path_t> ArticulationProfilesRepositoryStub::profileChanged() const
{
    static async::Channel<io::path_t> ch;
    return ch;
}
