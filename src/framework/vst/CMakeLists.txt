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

kfw_create_module(kfw_vst ALIAS kfw::vst)

add_subdirectory(sdk)

target_sources(kfw_vst PRIVATE
    ivstmodulesrepository.h
    ivstconfiguration.h
    ivstinstancesregister.h
    ivstplugininstance.h
    vsterrors.h
    vsttypes.h
    vstmodule.cpp
    vstmodule.h

    internal/vstactionscontroller.cpp
    internal/vstactionscontroller.h
    internal/vstuiactions.cpp
    internal/vstuiactions.h
    internal/vstmodulesrepository.cpp
    internal/vstmodulesrepository.h
    internal/vstinstancesregister.cpp
    internal/vstinstancesregister.h
    internal/vstconfiguration.cpp
    internal/vstconfiguration.h
    internal/vstplugininstance.cpp
    internal/vstplugininstance.h
    internal/vstcomponenthandler.cpp
    internal/vstcomponenthandler.h
    internal/vstaudioclient.cpp
    internal/vstaudioclient.h
    internal/vstpluginprovider.cpp
    internal/vstpluginprovider.h
    internal/vstpluginsscanner.cpp
    internal/vstpluginsscanner.h
    internal/vstpluginmetareader.cpp
    internal/vstpluginmetareader.h

    internal/synth/vstsynthesiser.cpp
    internal/synth/vstsynthesiser.h
    internal/synth/vstiresolver.cpp
    internal/synth/vstiresolver.h
    internal/synth/vstsequencer.cpp
    internal/synth/vstsequencer.h

    internal/fx/vstfxprocessor.cpp
    internal/fx/vstfxprocessor.h
    internal/fx/vstfxresolver.cpp
    internal/fx/vstfxresolver.h

    widgets/vstviewdialog_qwidget.cpp
    widgets/vstviewdialog_qwidget.h
)

target_link_libraries(kfw_vst PRIVATE vst3sdk)

if (KFW_QT_SUPPORT)
    list(APPEND MODULE_LINK Qt::Qml Qt::Widgets)
endif()

target_no_warning(kfw_vst -Wno-deprecated-declarations)

if (KFW_MODULE_VST_QML)
    add_subdirectory(qml/KitNote/Vst)
endif()
