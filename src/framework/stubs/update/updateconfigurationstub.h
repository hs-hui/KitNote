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
#ifndef KFW_UPDATE_UPDATECONFIGURATIONSTUB_H
#define KFW_UPDATE_UPDATECONFIGURATIONSTUB_H

#include "framework/stubs/update/iupdateconfiguration.h"

namespace kfw::update {
class UpdateConfigurationStub : public IUpdateConfiguration
{
public:
    bool isAppUpdatable() const override;

    bool allowUpdateOnPreRelease() const override;
    void setAllowUpdateOnPreRelease(bool allow) override;

    bool needCheckForUpdate() const override;
    void setNeedCheckForUpdate(bool needCheck) override;
    kfw::async::Notification needCheckForUpdateChanged() const override;

    std::string skippedReleaseVersion() const override;
    void setSkippedReleaseVersion(const std::string& version) override;

    bool checkForUpdateTestMode() const override;

    std::string checkForAppUpdateUrl() const override;
    std::string previousAppReleasesNotesUrl() const override;

    kfw::network::RequestHeaders updateHeaders() const override;

    std::string kitNoteUrl() const override;
    std::string kitNotePrivacyPolicyUrl() const override;

    io::path_t updateDataPath() const override;
    io::path_t updateRequestHistoryJsonPath() const override;
};
}

#endif // KFW_UPDATE_UPDATECONFIGURATIONSTUB_H
