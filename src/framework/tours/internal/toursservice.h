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

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "iinteractive.h"
#include "itoursprovider.h"
#include "itoursconfiguration.h"

#include "../itoursservice.h"

namespace kfw::tours {
class ToursService : public IToursService, public Contextable, public async::Asyncable
{
    ContextInject<IInteractive> interactive = { this };
    ContextInject<IToursProvider> toursProvider = { this };
    GlobalInject<IToursConfiguration> toursConfiguration;

public:
    ToursService(const kfw::modularity::ContextPtr& ctx)
        : Contextable(ctx) {}

    void registerTour(const String& eventCode, const Tour& tour) override;

    void onEvent(const String& eventCode) override;

private:
    std::unordered_map<String /*eventCode*/, Tour> m_eventsMap;
};
}
