# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2021 MuseScore Limited and others
# Modified for KitNote, 2026
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

kfw_create_module(kfw_multiwindows ALIAS kfw::multiwindows)

target_sources(kfw_multiwindows PRIVATE
    multiwindowsmodule.cpp
    multiwindowsmodule.h
    imultiwindowsprovider.h
    iprojectprovider.h
    resourcelockguard.h

    internal/multiwindowsuiactions.cpp
    internal/multiwindowsuiactions.h

    # Single process (multi window)
    internal/singleprocess/singleprocessprovider.cpp
    internal/singleprocess/singleprocessprovider.h

    # Multi process
    internal/multiprocess/multiprocessprovider.cpp
    internal/multiprocess/multiprocessprovider.h

    internal/multiprocess/ipc/ipclog.h
    internal/multiprocess/ipc/ipc.cpp
    internal/multiprocess/ipc/ipc.h
    internal/multiprocess/ipc/ipcchannel.cpp
    internal/multiprocess/ipc/ipcchannel.h
    internal/multiprocess/ipc/ipcserver.cpp
    internal/multiprocess/ipc/ipcserver.h
    internal/multiprocess/ipc/ipcsocket.cpp
    internal/multiprocess/ipc/ipcsocket.h
    internal/multiprocess/ipc/ipclock.cpp
    internal/multiprocess/ipc/ipclock.h
    internal/multiprocess/ipc/ipcloop.cpp
    internal/multiprocess/ipc/ipcloop.h
)

if (KFW_MODULE_MULTIWINDOWS_QML)
    add_subdirectory(qml/KitNote/MultiWindows)
endif()
