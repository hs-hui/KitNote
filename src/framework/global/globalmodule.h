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
#ifndef KFW_GLOBAL_GLOBALMODULE_H
#define KFW_GLOBAL_GLOBALMODULE_H

#include <memory>
#include <optional>

#include "logger.h"

#include "modularity/imodulesetup.h"
#include "modularity/ioc.h"
#include "io/ifilesystem.h"
#include "ticker.h"

namespace kfw {
class SystemInfo;
class GlobalConfiguration;
class BaseApplication;
class ITickerProvider;
class GlobalModule : public modularity::IModuleSetup
{
    GlobalInject<io::IFileSystem> fileSystem;

public:

    GlobalModule();

    std::string moduleName() const override;
    void registerExports() override;
    void onPreInit(const IApplication::RunMode& mode) override;
    void onInit(const IApplication::RunMode& mode) override;
    void onDeinit() override;

    void setLoggerLevel(const kfw::logger::Level& level);

private:
    std::shared_ptr<GlobalConfiguration> m_configuration;
    std::shared_ptr<SystemInfo> m_systemInfo;
    std::shared_ptr<ITickerProvider> m_tickerProvider;
    Ticker m_asyncTicker;

    std::optional<kfw::logger::Level> m_loggerLevel;

    bool m_endTimePeriod = false;
};
}

#endif // KFW_GLOBAL_GLOBALMODULE_H
