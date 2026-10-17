# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2026 MuseScore Limited and others
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
# along with this program. If not, see <https://www.gnu.org/licenses/>.

kfw_create_module(kfw_network ALIAS kfw::network)

target_sources(kfw_network PRIVATE
    networkmodule.cpp
    networkmodule.h
    networkerrors.h
    networktypes.h
    inetworkmanager.h
    inetworkmanagercreator.h
    inetworkconfiguration.h

    internal/networkmanager.cpp
    internal/networkmanager.h
    internal/networkmanagercreator.cpp
    internal/networkmanagercreator.h
    internal/networkconfiguration.cpp
    internal/networkconfiguration.h
)

target_link_libraries(kfw_network PUBLIC Qt::Network)

if (KFW_MODULE_NETWORK_WEBSOCKET)
    target_sources(kfw_network PRIVATE
    )

    target_compile_definitions(kfw_network PRIVATE KFW_MODULE_NETWORK_WEBSOCKET)
    target_link_libraries(kfw_network PRIVATE Qt::WebSockets)
endif()

if (KFW_MODULE_NETWORK_TESTS)
    add_subdirectory(tests)
endif()
