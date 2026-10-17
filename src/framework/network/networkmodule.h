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
#ifndef KFW_NETWORK_NETWORKMODULE_H
#define KFW_NETWORK_NETWORKMODULE_H

#include "modularity/imodulesetup.h"

namespace kfw::network {
class NetworkConfiguration;
class NetworkModule : public modularity::IModuleSetup
{
public:
    std::string moduleName() const override;

    void registerExports() override;
    void onInit(const IApplication::RunMode& mode) override;

private:
    std::shared_ptr<NetworkConfiguration> m_configuration;
};
}

#endif // KFW_NETWORK_NETWORKMODULE_H
