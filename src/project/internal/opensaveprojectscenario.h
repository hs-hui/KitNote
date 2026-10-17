/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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

#include "iopensaveprojectscenario.h"

#include "modularity/ioc.h"
#include "iprojectconfiguration.h"
#include "iprojectfilescontroller.h"
#include "global/iinteractive.h"

namespace kn::project {
class OpenSaveProjectScenario : public IOpenSaveProjectScenario, public kfw::Contextable
{
    kfw::GlobalInject<IProjectConfiguration> configuration;
    kfw::ContextInject<IProjectFilesController> projectFilesController = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    OpenSaveProjectScenario(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    kfw::RetVal<SaveLocation> askSaveLocation(INotationProjectPtr project, SaveMode mode) const override;
    kfw::RetVal<kfw::io::path_t> askLocalPath(INotationProjectPtr project, SaveMode mode) const override;
};
}
