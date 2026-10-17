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

#pragma once

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "global/iglobalconfiguration.h"
#include "global/iapplication.h"
#include "global/io/ifilesystem.h"
#include "multiwindows/imultiwindowsprovider.h"
#include "ui/iuiconfiguration.h"
#include "project/iprojectconfiguration.h"
#include "notation/inotationconfiguration.h"
#include "playback/iplaybackconfiguration.h"
#include "languages/ilanguagesconfiguration.h"

#include "iappshellconfiguration.h"

namespace kn::appshell {
class AppShellConfiguration : public IAppShellConfiguration, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;
    kfw::GlobalInject<kfw::mi::IMultiWindowsProvider> multiwindowsProvider;
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::GlobalInject<project::IProjectConfiguration> projectConfiguration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<playback::IPlaybackConfiguration> playbackConfiguration;
    kfw::GlobalInject<kfw::languages::ILanguagesConfiguration> languagesConfiguration;
    kfw::ContextInject<kfw::IApplication> application = { this };

public:
    AppShellConfiguration(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();

    bool hasCompletedFirstLaunchSetup() const override;
    void setHasCompletedFirstLaunchSetup(bool has) override;

    bool welcomeDialogShowOnStartup() const override;
    void setWelcomeDialogShowOnStartup(bool show) override;
    kfw::async::Notification welcomeDialogShowOnStartupChanged() const override;

    std::string welcomeDialogLastShownVersion() const override;
    void setWelcomeDialogLastShownVersion(const std::string& version) override;

    int welcomeDialogLastShownIndex() const override;
    void setWelcomeDialogLastShownIndex(int index) override;

    StartupModeType startupModeType() const override;
    void setStartupModeType(StartupModeType type) override;
    kfw::async::Notification startupModeTypeChanged() const override;

    kfw::io::path_t startupScorePath() const override;
    void setStartupScorePath(const kfw::io::path_t& scorePath) override;
    kfw::async::Notification startupScorePathChanged() const override;

    std::string kitNoteUrl() const override;
    std::string kitNoteForumUrl() const override;
    std::string kitNoteContributionUrl() const override;
    std::string musicXMLLicenseUrl() const override;
    std::string musicXMLLicenseDeedUrl() const override;

    std::string kitNoteVersion() const override;
    std::string kitNoteRevision() const override;

    bool isNotationNavigatorVisible() const override;
    void setIsNotationNavigatorVisible(bool visible) const override;
    kfw::async::Notification isNotationNavigatorVisibleChanged() const override;

    bool needShowSplashScreen() const override;
    void setNeedShowSplashScreen(bool show) override;

    const QString& preferencesDialogLastOpenedPageId() const override;
    void setPreferencesDialogLastOpenedPageId(const QString& lastOpenedPageId) override;

    void startEditSettings() override;
    void applySettings() override;
    void rollbackSettings() override;

    void revertToFactorySettings(bool keepDefaultSettings = false, bool notifyAboutChanges = true,
                                 bool notifyOtherInstances = true) const override;

    kfw::io::paths_t sessionProjectsPaths() const override;
    kfw::Ret setSessionProjectsPaths(const kfw::io::paths_t& paths) override;

private:
    std::string utmParameters(const std::string& utmMedium) const;

    std::string currentLanguageCode() const;

    kfw::io::path_t sessionDataPath() const;
    kfw::io::path_t sessionFilePath() const;

    kfw::RetVal<kfw::ByteArray> readSessionState() const;
    kfw::Ret writeSessionState(const QByteArray& data);

    kfw::io::paths_t parseSessionProjectsPaths(const QByteArray& json) const;

    QString m_preferencesDialogCurrentPageId;

    kfw::async::Notification m_welcomeDialogShowOnStartupChanged;

    kfw::async::Notification m_startupModeTypeChanged;
    kfw::async::Notification m_startupScorePathChanged;
};
}
