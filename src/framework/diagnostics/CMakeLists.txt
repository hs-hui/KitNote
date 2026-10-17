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

kfw_create_module(kfw_diagnostics ALIAS kfw::diagnostics NO_COVERAGE)

target_sources(kfw_diagnostics PRIVATE
    diagnosticsmodule.cpp
    diagnosticsmodule.h
    diagnosticutils.h
    idiagnosticspathsregister.h
    idiagnosticsconfiguration.h

    internal/diagnosticsconfiguration.cpp
    internal/diagnosticsconfiguration.h
    internal/diagnosticsactions.cpp
    internal/diagnosticsactions.h
    internal/diagnosticsactionscontroller.cpp
    internal/diagnosticsactionscontroller.h
    internal/diagnosticspathsregister.cpp
    internal/diagnosticspathsregister.h

    internal/isavediagnosticfilesscenario.h
    internal/savediagnosticfilesscenario.cpp
    internal/savediagnosticfilesscenario.h
    internal/diagnosticfileswriter.cpp
    internal/diagnosticfileswriter.h
)


if (KFW_QT_SUPPORT)
    target_link_libraries(kfw_diagnostics PRIVATE Qt::Gui Qt::Quick)
endif()

if (KFW_MODULE_DIAGNOSTICS_TESTS)
    add_subdirectory(tests)
endif()

if (KFW_MODULE_DIAGNOSTICS_QML)
    add_subdirectory(qml/KitNote/Diagnostics)
endif()
