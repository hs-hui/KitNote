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
#include "firstlaunchsetupmodel.h"

#include "translation.h"
#include "global/async/async.h"

using namespace kfw;
using namespace kn;
using namespace kn::appshell;

FirstLaunchSetupModel::FirstLaunchSetupModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
    m_pages = {
        // KitNote: the MuseSounds and video tutorial pages advertise MuseScore services.
        // With only this page left, the Home page stays behind the dialog; an empty score page
        // would be all a new user sees after closing it.
        Page { "ThemesPage.qml", "kitnote://home" }
    };
}

void FirstLaunchSetupModel::load()
{
    setCurrentPageIndex(0);
}

int FirstLaunchSetupModel::numberOfPages() const
{
    return m_pages.size();
}

int FirstLaunchSetupModel::currentPageIndex() const
{
    return m_currentPageIndex;
}

QVariantMap FirstLaunchSetupModel::Page::toMap() const
{
    return {
        { "url", url },
    };
}

QVariantMap FirstLaunchSetupModel::currentPage() const
{
    if (m_currentPageIndex < 0 || m_currentPageIndex >= m_pages.size()) {
        return {};
    }

    return m_pages.at(m_currentPageIndex).toMap();
}

bool FirstLaunchSetupModel::canGoBack() const
{
    return m_currentPageIndex > 0;
}

bool FirstLaunchSetupModel::canGoForward() const
{
    return m_currentPageIndex < m_pages.size() - 1;
}

bool FirstLaunchSetupModel::canFinish() const
{
    return m_currentPageIndex == m_pages.size() - 1;
}

void FirstLaunchSetupModel::setCurrentPageIndex(int index)
{
    if (index == m_currentPageIndex || index < 0 || index >= m_pages.size()) {
        return;
    }

    m_currentPageIndex = index;
    emit currentPageChanged();

    async::Async::call(this, [this]() {
        interactive()->open(m_pages.at(m_currentPageIndex).backgroundUri);
    });
}

bool FirstLaunchSetupModel::askAboutClosingEarly()
{
    const std::string title = kfw::trc("appshell/gettingstarted", "Are you sure you want to cancel?");
    const std::string body = kfw::trc("appshell/gettingstarted",
                                       "The setup will be shown again the next time you start KitNote.");
    const IInteractive::Text text(body);

    const IInteractive::ButtonData keepGoingBtn {
        IInteractive::Button::Continue,
        kfw::trc("appshell/gettingstarted", "Keep going"),
        true,
        false,
#ifdef Q_OS_WINDOWS
        IInteractive::AcceptRole
#else
        IInteractive::ContinueRole
#endif
    };

    const IInteractive::ButtonDatas buttons {
        interactive()->buttonData(IInteractive::Button::Cancel), keepGoingBtn
    };

    IInteractive::Result result = interactive()->warningSync(title, text, buttons, int(IInteractive::Button::Cancel));

    return result.standardButton() == IInteractive::Button::Cancel;
}

void FirstLaunchSetupModel::finish()
{
    configuration()->setHasCompletedFirstLaunchSetup(true);
}
