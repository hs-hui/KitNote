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
#ifndef KFW_IPC_IPCLOCK_H
#define KFW_IPC_IPCLOCK_H

#include <QString>

class QSystemSemaphore;
namespace kfw::ipc {
class IpcLock
{
public:
    IpcLock(const QString& name);
    ~IpcLock();

    bool lock();
    bool unlock();

private:
    QSystemSemaphore* m_locker = nullptr;
};

class IpcLockGuard
{
public:
    IpcLockGuard(IpcLock* lock)
        : m_lock(lock)
    {
        m_lock->lock();
    }

    ~IpcLockGuard()
    {
        m_lock->unlock();
    }

private:
    IpcLock* m_lock = nullptr;
};
}

#endif // KFW_IPC_IPCLOCK_H
