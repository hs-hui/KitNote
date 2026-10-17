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

#include "global/async/asyncable.h"

#include "modularity/ioc.h"
#include "iappshellconfiguration.h"
#include "iinteractive.h"

namespace kn::appshell {
class FirstLaunchSetupModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT

    Q_PROPERTY(int numberOfPages READ numberOfPages CONSTANT)
    Q_PROPERTY(int currentPageIndex READ currentPageIndex WRITE setCurrentPageIndex NOTIFY currentPageChanged)
    Q_PROPERTY(QVariantMap currentPage READ currentPage NOTIFY currentPageChanged)

    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY currentPageChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY currentPageChanged)
    Q_PROPERTY(bool canFinish READ canFinish NOTIFY currentPageChanged)

    QML_ELEMENT

    kfw::GlobalInject<IAppShellConfiguration> configuration;
    kfw::ContextInject<kfw::IInteractive> interactive = { this };

public:
    explicit FirstLaunchSetupModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();

    int numberOfPages() const;
    int currentPageIndex() const;
    QVariantMap currentPage() const;

    bool canGoBack() const;
    bool canGoForward() const;
    bool canFinish() const;

    Q_INVOKABLE bool askAboutClosingEarly();

    Q_INVOKABLE void finish();

public slots:
    void setCurrentPageIndex(int index);

signals:
    void currentPageChanged();

private:
    struct Page {
        QString url;
        std::string backgroundUri;

        QVariantMap toMap() const;
    };

    QList<Page> m_pages;
    int m_currentPageIndex = -1;
};
}
