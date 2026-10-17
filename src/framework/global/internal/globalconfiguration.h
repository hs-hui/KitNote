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
#ifndef KFW_GLOBAL_GLOBALCONFIGURATION_H
#define KFW_GLOBAL_GLOBALCONFIGURATION_H

#include "../iglobalconfiguration.h"

#include "modularity/ioc.h"
#include "../iapplication.h"

namespace kfw {
class GlobalConfiguration : public IGlobalConfiguration, public Contextable
{
    ContextInject<IApplication> application{ this };

public:
    GlobalConfiguration(const modularity::ContextPtr& ctx)
        : Contextable(ctx) {}

    void init();

    io::path_t appBinPath() const override;
    io::path_t appBinDirPath() const override;
    io::path_t appDataPath() const override;
    io::path_t appConfigPath() const override;

    io::path_t userAppDataPath() const override;
    io::path_t userBackupPath() const override;
    io::path_t userDataPath() const override;

    io::path_t homePath() const override;
    io::path_t downloadsPath() const override;

    bool useFactorySettings() const override;
    bool enableExperimental() const override;
    io::path_t genericDataPath() const override;

    bool devModeEnabled() const override;
    void setDevModeEnabled(bool enabled) override;

    bool metricUnit() const override;
    void setMetricUnit(bool metricUnit) override;

    std::string kitNoteUrl() const override;

    bool highResolutionTimers() const override;

private:
    QString resolveAppDataPath() const;
    QString resolveUserAppDataPath() const;

    mutable io::path_t m_appDataPath;
    mutable io::path_t m_userAppDataPath;
};
}

#endif // KFW_GLOBAL_GLOBALCONFIGURATION_H
