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
#include "appshellconfiguration.h"

using namespace kfw;
using namespace kn::appshell;

static const std::string module_name("appshell");

static const QString NOTATION_NAVIGATOR_VISIBLE_KEY("showNavigator");

void AppShellConfiguration::init()
{
}

std::string AppShellConfiguration::kitNoteVersion() const
{
    return String(application()->version().toString() + u"." + application()->build()).toStdString();
}

std::string AppShellConfiguration::kitNoteRevision() const
{
    return application()->revision().toStdString();
}

bool AppShellConfiguration::isNotationNavigatorVisible() const
{
    return uiConfiguration()->isVisible(NOTATION_NAVIGATOR_VISIBLE_KEY, false);
}

void AppShellConfiguration::setIsNotationNavigatorVisible(bool visible) const
{
    uiConfiguration()->setIsVisible(NOTATION_NAVIGATOR_VISIBLE_KEY, visible);
}

kfw::async::Notification AppShellConfiguration::isNotationNavigatorVisibleChanged() const
{
    return uiConfiguration()->isVisibleChanged(NOTATION_NAVIGATOR_VISIBLE_KEY);
}
