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

#pragma once

#include "global/async/asyncable.h"

#include "modularity/ioc.h"
#include "global/iinteractive.h"
#include "../inotationconfiguration.h"

namespace kn::notation {
class ScoreErrorsController : public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    ScoreErrorsController(const kfw::modularity::ContextPtr& iocCtx);

    void checkAndShowMScoreError();

private:
    kn::engraving::MsError m_currentDialogError = kn::engraving::MsError::MS_NO_ERROR;
};
}
