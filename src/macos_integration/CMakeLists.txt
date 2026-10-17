# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2024 MuseScore Limited and others
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

if(NOT OS_IS_MAC)
    return()
endif()

message(STATUS "Configuring macOS integration")

set(CMAKE_OSX_ARCHITECTURES_BACKUP ${CMAKE_OSX_ARCHITECTURES})

set(CMAKE_OSX_ARCHITECTURES "" CACHE STRING "" FORCE)

enable_language(Swift)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

# Set up swiftrt.o and runtime library search paths
include(InitializeSwift)

# set(MACOSX_DEPLOYMENT_TARGET 12.0)
# set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0)

add_executable(KitNoteQuickLookPreviewExtension
    MACOSX_BUNDLE
    PreviewProvider.swift
    previewprovider.cpp
    previewprovider.h
)

target_link_libraries(KitNoteQuickLookPreviewExtension PRIVATE
    kfw::global
    iex_imagesexport
    project
)

target_include_directories(KitNoteQuickLookPreviewExtension PRIVATE
    ${PROJECT_SOURCE_DIR}/src

    ${KFW_FRAMEWORK_PATH}
    ${KFW_FRAMEWORK_PATH}/framework
    ${KFW_FRAMEWORK_PATH}/framework/global

    # compat
    ${KFW_FRAMEWORK_PATH}/src
    ${KFW_FRAMEWORK_PATH}/src/framework
    ${KFW_FRAMEWORK_PATH}/src/framework/global
    # end compat
)

# if(KFW_COMPILE_USE_PCH)
#     target_precompile_headers_clang_ccache(KitNoteQuickLookPreviewExtension REUSE_FROM kfw_pch)
#     target_link_libraries(KitNoteQuickLookPreviewExtension PRIVATE kfw_pch)
# endif()

target_compile_options(KitNoteQuickLookPreviewExtension PRIVATE
    $<$<COMPILE_LANGUAGE:Swift>:-application-extension>
    "$<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default>"
)

target_link_options(KitNoteQuickLookPreviewExtension PRIVATE
    "$<$<COMPILE_LANGUAGE:Swift>:SHELL:-Xlinker -e>"
    "$<$<COMPILE_LANGUAGE:Swift>:SHELL:-Xlinker _NSExtensionMain>"
)

set_target_properties(KitNoteQuickLookPreviewExtension PROPERTIES
    XCODE_PRODUCT_TYPE "com.apple.product-type.app-extension"
    BUNDLE_EXTENSION appex
    XCODE_ATTRIBUTE_WRAPPER_EXTENSION appex
    XCODE_ATTRIBUTE_GENERATE_PKGINFO_FILE YES
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    MACOSX_BUNDLE_BUNDLE_NAME "KitNoteQuickLookPreviewExtension"
    MACOSX_BUNDLE_BUNDLE_VERSION "${CMAKE_BUILD_NUMBER}"
    MACOSX_BUNDLE_COPYRIGHT "Copyright © 1999-2026 MuseScore Limited. Published under the GNU General Public License version 3."
    MACOSX_BUNDLE_GUI_IDENTIFIER "org.kitnote.KitNote.QuickLookPreviewExtension"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${KITNOTE_APP_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${KITNOTE_APP_VERSION}"
    OSX_ARCHITECTURES ""
)

set_target_properties(KitNoteQuickLookPreviewExtension PROPERTIES
    INSTALL_RPATH "@executable_path/../../../../Frameworks;${QT_INSTALL_LIBS}"
)

install(TARGETS KitNoteQuickLookPreviewExtension
    BUNDLE DESTINATION KitNote.app/Contents/PlugIns
)

set(the_appex_path "${CMAKE_INSTALL_PREFIX}/KitNote.app/Contents/PlugIns/KitNoteQuickLookPreviewExtension.appex")

# Platform plugin is loaded by Qt at runtime, rather than as a linked framework.
# Due to sandboxing, this does not work if the plugin is outside the .appex
# bundle, not even if it is inside the main .app bundle. So we shamelessly
# copy it into the .appex bundle.
install(FILES ${QT_INSTALL_PLUGINS}/platforms/libqcocoa.dylib
    DESTINATION ${the_appex_path}/Contents/MacOS/platforms
)

install(CODE "
    execute_process(COMMAND rm -f \"${the_appex_path}/Contents/MacOS/KitNoteQuickLookPreviewExtension.emit-module.d\")
    execute_process(COMMAND rm -f \"${the_appex_path}/Contents/MacOS/KitNoteQuickLookPreviewExtension.d\")
    # KitNote: the preview finds its fonts and styles in Resources of the bundle it runs from (see
    # GlobalConfiguration::resolveAppDataPath). Link them to KitNote.app/Contents/Resources.
    execute_process(COMMAND ln -sfn ../../../Resources \"${the_appex_path}/Contents/Resources\")
    execute_process(COMMAND codesign --force --sign - --timestamp=none --options runtime --entitlements \"${CMAKE_CURRENT_SOURCE_DIR}/entitlements.plist\" \"${the_appex_path}\")
")

set(CMAKE_OSX_ARCHITECTURES ${CMAKE_OSX_ARCHITECTURES_BACKUP} CACHE STRING "" FORCE)
