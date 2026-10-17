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

#include <QObject>
#include <qqmlintegration.h>

#include "modularity/ioc.h"
#include "iappshellconfiguration.h"
#include "framework/stubs/update/iupdateconfiguration.h"
#include "global/iglobalconfiguration.h"
#include "global/iapplication.h"

class QUrl;

namespace kn::appshell {
class AboutModel : public QObject, public kfw::Contextable
{
    Q_OBJECT

    QML_ELEMENT

    kfw::GlobalInject<IAppShellConfiguration> configuration;
    kfw::GlobalInject<kfw::update::IUpdateConfiguration> updateConfiguration;
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::ContextInject<kfw::IApplication> application = { this };

public:
    explicit AboutModel(QObject* parent = nullptr);

    Q_INVOKABLE QString kitNoteVersion() const;
    Q_INVOKABLE QString kitNoteRevision() const;
    Q_INVOKABLE QVariantMap kitNoteUrl() const;
    Q_INVOKABLE QVariantMap kitNoteForumUrl() const;
    Q_INVOKABLE QVariantMap kitNoteContributionUrl() const;
    Q_INVOKABLE QVariantMap kitNotePrivacyPolicyUrl() const;

    Q_INVOKABLE QVariantMap musicXMLLicenseUrl() const;
    Q_INVOKABLE QVariantMap musicXMLLicenseDeedUrl() const;

    Q_INVOKABLE void copyRevisionToClipboard() const;

    Q_INVOKABLE void toggleDevMode();

private:
    QVariantMap makeUrl(const QUrl& url, bool showPath = true) const;
};
}
