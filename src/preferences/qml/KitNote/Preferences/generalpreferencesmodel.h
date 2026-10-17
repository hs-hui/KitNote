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

#include <qqmlintegration.h>

#include <QObject>

#include "progress.h"

#include "modularity/ioc.h"
#include "appshell/iappshellconfiguration.h"
#include "async/asyncable.h"

#include "global/iinteractive.h"
#include "languages/ilanguagesconfiguration.h"
#include "languages/ilanguagesservice.h"
#include "shortcuts/ishortcutsconfiguration.h"
#include "framework/stubs/update/iupdateconfiguration.h"

namespace kn::preferences {
class GeneralPreferencesModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT
    QML_ELEMENT;

    Q_PROPERTY(QVariantList languages READ languages NOTIFY languagesChanged)
    Q_PROPERTY(QString currentLanguageCode READ currentLanguageCode WRITE setCurrentLanguageCode NOTIFY currentLanguageCodeChanged)

    Q_PROPERTY(QStringList keyboardLayouts READ keyboardLayouts CONSTANT)
    Q_PROPERTY(QString currentKeyboardLayout READ currentKeyboardLayout WRITE setCurrentKeyboardLayout NOTIFY currentKeyboardLayoutChanged)

    Q_PROPERTY(bool isOSCRemoteControl READ isOSCRemoteControl WRITE setIsOSCRemoteControl NOTIFY isOSCRemoteControlChanged)
    Q_PROPERTY(int oscPort READ oscPort WRITE setOscPort NOTIFY oscPortChanged)

    Q_PROPERTY(bool restartRequired READ restartRequired WRITE setRestartRequired NOTIFY restartRequiredChanged)

    Q_PROPERTY(QVariantList startupModes READ startupModes CONSTANT)
    Q_PROPERTY(int currentStartupMode READ currentStartupMode WRITE setCurrentStartupMode NOTIFY currentStartupModeChanged)
    Q_PROPERTY(QString startupScorePath READ startupScorePath WRITE setStartupScorePath NOTIFY startupScorePathChanged)
    Q_PROPERTY(QStringList scorePathFilter READ scorePathFilter CONSTANT)

    Q_PROPERTY(bool showWelcomeDialog READ showWelcomeDialog WRITE setShowWelcomeDialog NOTIFY showWelcomeDialogChanged)

    Q_PROPERTY(
        bool needCheckForNewAppVersion READ needCheckForNewAppVersion WRITE setNeedCheckForNewAppVersion NOTIFY needCheckForNewAppVersionChanged)

    kfw::GlobalInject<appshell::IAppShellConfiguration> configuration;
    kfw::GlobalInject<kfw::languages::ILanguagesConfiguration> languagesConfiguration;
    kfw::GlobalInject<kfw::shortcuts::IShortcutsConfiguration> shortcutsConfiguration;
    kfw::GlobalInject<kfw::languages::ILanguagesService> languagesService;
    kfw::GlobalInject<kfw::update::IUpdateConfiguration> updateConfiguration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    explicit GeneralPreferencesModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();
    Q_INVOKABLE void checkUpdateForCurrentLanguage();

    QVariantList languages() const;
    QString currentLanguageCode() const;

    QStringList keyboardLayouts() const;
    QString currentKeyboardLayout() const;

    bool isOSCRemoteControl() const;
    int oscPort() const;
    bool restartRequired() const;

    QVariantList startupModes() const;
    int currentStartupMode() const;
    QString startupScorePath() const;
    QStringList scorePathFilter() const;

    bool showWelcomeDialog() const;
    void setShowWelcomeDialog(bool show);

    bool needCheckForNewAppVersion() const;
    Q_INVOKABLE bool isAppUpdatable() const;
    Q_INVOKABLE QString kitNotePrivacyPolicyUrl() const;

public slots:
    void setCurrentLanguageCode(const QString& currentLanguageCode);
    void setCurrentKeyboardLayout(const QString& keyboardLayout);
    void setIsOSCRemoteControl(bool isOSCRemoteControl);
    void setOscPort(int oscPort);
    void setRestartRequired(bool restartRequired);
    void setCurrentStartupMode(int mode);
    void setStartupScorePath(const QString& scorePath);
    void setNeedCheckForNewAppVersion(bool value);

signals:
    void languagesChanged(QVariantList languages);
    void currentLanguageCodeChanged(QString currentLanguageCode);
    void currentKeyboardLayoutChanged();
    void isOSCRemoteControlChanged(bool isOSCRemoteControl);
    void oscPortChanged(int oscPort);

    void receivingUpdateForCurrentLanguage(int current, int total, QString status);
    void restartRequiredChanged();

    void currentStartupModeChanged();
    void startupScorePathChanged();

    void showWelcomeDialogChanged();

    void needCheckForNewAppVersionChanged(bool value);

private:
    kfw::Progress m_languageUpdateProgress;

    bool m_restartRequired = false;
};
}
