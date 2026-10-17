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
#ifndef KFW_MI_RESOURCELOCKGUARD_H
#define KFW_MI_RESOURCELOCKGUARD_H

#include <string>
#include <memory>

#include "imultiwindowsprovider.h"

namespace kfw::mi {
class ReadResourceLockGuard
{
public:
    ReadResourceLockGuard(std::shared_ptr<IMultiWindowsProvider> provider, const std::string& name)
        : m_provider(provider), m_name(name)
    {
        if (m_provider) {
            m_provider->lockResource(m_name);
        }
    }

    ~ReadResourceLockGuard()
    {
        if (m_provider) {
            m_provider->unlockResource(m_name);
        }
    }

protected:
    std::shared_ptr<IMultiWindowsProvider> m_provider = nullptr;
    std::string m_name;
};

class WriteResourceLockGuard : ReadResourceLockGuard
{
public:
    WriteResourceLockGuard(std::shared_ptr<IMultiWindowsProvider> provider, const std::string& name)
        : ReadResourceLockGuard(provider, name)
    {
    }

    ~WriteResourceLockGuard()
    {
        if (m_provider) {
            m_provider->notifyAboutResourceChanged(m_name);
        }
    }
};
}

#endif // KFW_MI_RESOURCELOCKGUARD_H
