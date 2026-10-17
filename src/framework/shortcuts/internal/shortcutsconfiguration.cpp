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
#include "shortcutsconfiguration.h"

#include "settings.h"
#include "io/path.h"

#include "global/configreader.h"

#include "log.h"

using namespace kfw;
using namespace kfw::shortcuts;

void ShortcutsConfiguration::init()
{
    m_config = ConfigReader::read(":/configs/shortcuts.cfg");
}

QString ShortcutsConfiguration::currentKeyboardLayout() const
{
    NOT_IMPLEMENTED;
    return "US-QWERTY";
}

void ShortcutsConfiguration::setCurrentKeyboardLayout(const QString& layout)
{
    UNUSED(layout);
    NOT_IMPLEMENTED;
    return;
}

io::path_t ShortcutsConfiguration::shortcutsUserAppDataPath() const
{
    return globalConfiguration()->userAppDataPath() + "/shortcuts.xml";
}

io::path_t ShortcutsConfiguration::shortcutsAppDataPath() const
{
#if defined(Q_OS_MACOS)
    return m_config.value("shortcuts_mac").toPath();
#endif

    return m_config.value("shortcuts").toPath();
}
