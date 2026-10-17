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

#include "types/retval.h"
#include "async/promise.h"
#include "progress.h"

#include "updatetypes.h"

#include "modularity/imoduleinterface.h"

namespace kfw::update {
class IAppUpdateService : MODULE_CONTEXT_INTERFACE
{
    INTERFACE_ID(IAppUpdateService)

public:
    virtual ~IAppUpdateService() = default;

    virtual async::Promise<kfw::RetVal<ReleaseInfo> > checkForUpdate() = 0;
    virtual const RetVal<ReleaseInfo>& lastCheckResult() const = 0;
    virtual RetVal<Progress> downloadRelease() = 0;
};
}
