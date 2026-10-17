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

kfw_create_module(kfw_audioplugins ALIAS kfw::audioplugins)

target_sources(kfw_audioplugins PRIVATE
    audiopluginsmodule.cpp
    audiopluginsmodule.h
    audiopluginstypes.h
    audiopluginserrors.h
    iaudiopluginsconfiguration.h
    iknownaudiopluginsregister.h
    iaudiopluginsscanner.h
    iaudiopluginsscannerregister.h
    iaudiopluginmetareader.h
    iaudiopluginmetareaderregister.h
    iregisteraudiopluginsscenario.h

    internal/audiopluginsutils.h
    internal/audiopluginsconfiguration.cpp
    internal/audiopluginsconfiguration.h
    internal/knownaudiopluginsregister.cpp
    internal/knownaudiopluginsregister.h
    internal/audiopluginsscannerregister.cpp
    internal/audiopluginsscannerregister.h
    internal/audiopluginmetareaderregister.cpp
    internal/audiopluginmetareaderregister.h
    internal/registeraudiopluginsscenario.cpp
    internal/registeraudiopluginsscenario.h
)

if (KFW_MODULE_AUDIOPLUGINS_TESTS)
    add_subdirectory(tests)
endif()
