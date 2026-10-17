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

#include <memory>

#include "modularity/imodulesetup.h"

namespace kfw::tours {
class ToursService;
class ToursConfiguration;
class ToursProvider;
class ToursModule : public modularity::IModuleSetup
{
public:
    std::string moduleName() const override;
    void registerExports() override;

private:
    std::shared_ptr<ToursService> m_service;
    std::shared_ptr<ToursConfiguration> m_configuration;
    std::shared_ptr<ToursProvider> m_provider;
};
}
