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
#include "aboutmodel.h"

#include "translation.h"

#include <QApplication>
#include <QClipboard>
#include <QUrl>

using namespace kn::appshell;

AboutModel::AboutModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

QString AboutModel::kitNoteVersion() const
{
    QString version = QString::fromStdString(configuration()->kitNoteVersion());
    return application()->unstable()
           ? kfw::qtrc("appshell/about", "Unstable prerelease for %1").arg(version)
           : version;
}

QString AboutModel::kitNoteRevision() const
{
    return QString::fromStdString(configuration()->kitNoteRevision());
}

QVariantMap AboutModel::kitNoteUrl() const
{
    QUrl kitNoteUrl(QString::fromStdString(configuration()->kitNoteUrl()));
    return makeUrl(kitNoteUrl, false);
}

QVariantMap AboutModel::kitNoteForumUrl() const
{
    QUrl kitNoteForumUrl(QString::fromStdString(configuration()->kitNoteForumUrl()));
    return makeUrl(kitNoteForumUrl);
}

QVariantMap AboutModel::kitNoteContributionUrl() const
{
    QUrl kitNoteContributionUrl(QString::fromStdString(configuration()->kitNoteContributionUrl()));
    return makeUrl(kitNoteContributionUrl);
}

QVariantMap AboutModel::kitNotePrivacyPolicyUrl() const
{
    QUrl kitNotePrivacyPolicyUrl(QString::fromStdString(updateConfiguration()->kitNotePrivacyPolicyUrl()));
    return makeUrl(kitNotePrivacyPolicyUrl);
}

QVariantMap AboutModel::musicXMLLicenseUrl() const
{
    QUrl musicXMLLicenseUrl(QString::fromStdString(configuration()->musicXMLLicenseUrl()));
    return makeUrl(musicXMLLicenseUrl);
}

QVariantMap AboutModel::musicXMLLicenseDeedUrl() const
{
    QUrl musicXMLLicenseDeedUrl(QString::fromStdString(configuration()->musicXMLLicenseDeedUrl()));
    return makeUrl(musicXMLLicenseDeedUrl);
}

void AboutModel::copyRevisionToClipboard() const
{
    QApplication::clipboard()->setText(
        QString("OS: %1, Arch.: %2, KitNote version (%3-bit): %4-%5 (KitNote " KITNOTE_ENGINE_VERSION " engine), revision: %6")
        .arg(QSysInfo::prettyProductName()
             + ((QSysInfo::productType() == "windows" && (QSysInfo::productVersion() == "10" || QSysInfo::productVersion() == "11"))
                ? " or later" : ""))
        .arg(QSysInfo::currentCpuArchitecture())
        .arg(QSysInfo::WordSize)
        .arg(application()->version().toString())
        .arg(application()->build())
        .arg(application()->revision()));
}

void AboutModel::toggleDevMode()
{
    globalConfiguration()->setDevModeEnabled(!globalConfiguration()->devModeEnabled());
}

QVariantMap AboutModel::makeUrl(const QUrl& url, bool showPath) const
{
    QVariantMap urlMap;
    urlMap["url"] = url.toString();
    urlMap["displayName"] = showPath ? url.host() + url.path() : url.host();

    return urlMap;
}
