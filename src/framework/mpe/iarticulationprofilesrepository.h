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

#ifndef KFW_MPE_IARTICULATIONPROFILESREPOSITORY_H
#define KFW_MPE_IARTICULATIONPROFILESREPOSITORY_H

#include "modularity/imoduleinterface.h"
#include "async/channel.h"
#include "io/path.h"

#include "mpetypes.h"

namespace kfw::mpe {
class IArticulationProfilesRepository : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IArticulationProfilesRepository)

public:
    virtual ~IArticulationProfilesRepository() = default;

    virtual ArticulationsProfilePtr createNew() const = 0;
    virtual ArticulationsProfilePtr defaultProfile(const ArticulationFamily family) const = 0;
    virtual ArticulationsProfilePtr loadProfile(const io::path_t& path) const = 0;
    virtual void saveProfile(const io::path_t& path, const ArticulationsProfilePtr profilePtr) = 0;
    virtual async::Channel<io::path_t> profileChanged() const = 0;
};
}

#endif // KFW_MPE_IARTICULATIONPROFILESREPOSITORY_H
