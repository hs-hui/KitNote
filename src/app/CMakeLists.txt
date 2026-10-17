# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2021-2024 MuseScore Limited and others
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

###########################################
# Setup main application
###########################################

set(EXECUTABLE_NAME kitnote)

###########################################
# Platform specific
###########################################
include(GetPlatformInfo)
if (OS_IS_WIN)
    set(EXECUTABLE_NAME ${KITNOTE_APP_NAME})

    include(GetCompilerInfo)

    if (CC_IS_MSVC)
        # MSVC recognizes a *.rc file and will invoke the resource compiler to link it
        # KitNote: a second resource file carries the version information.
        configure_file(${PROJECT_SOURCE_DIR}/share/icons/windows_version.rc.in ${PROJECT_BINARY_DIR}/windows_version.rc @ONLY)
        set(WINDOWS_ICONS_RC ${PROJECT_SOURCE_DIR}/share/icons/windows_icons.rc ${PROJECT_BINARY_DIR}/windows_version.rc)
    elseif(CC_IS_MINGW)
        set(WINDOWS_ICONS_RC ${PROJECT_BINARY_DIR}/windows_icons_rc.o)
        add_custom_command(
            OUTPUT ${PROJECT_BINARY_DIR}/windows_icons_rc.o
            COMMAND ${CMAKE_RC_COMPILER} -i windows_icons.rc -o ${PROJECT_BINARY_DIR}/windows_icons_rc.o
            DEPENDS ${PROJECT_SOURCE_DIR}/share/icons/windows_icons.rc
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/share/icons
            )
        set_source_files_properties(${PROJECT_BINARY_DIR}/windows_icons_rc.o PROPERTIES generated true)
    endif()

elseif(OS_IS_LIN)

    # add ssl support on local linux machine
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L /usr/local/lib -lcrypto -lssl")

    if (KITNOTE_APP_INSTALL_SUFFIX)
        set(EXECUTABLE_NAME "${EXECUTABLE_NAME}${KITNOTE_APP_INSTALL_SUFFIX}")
    endif(KITNOTE_APP_INSTALL_SUFFIX)

    set(CMAKE_INSTALL_RPATH "${QT_INSTALL_LIBS}")
    if (BUILD_SHARED_LIBS)
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${SHARED_LIBS_INSTALL_DESTINATION}")
    endif(BUILD_SHARED_LIBS)

elseif(OS_IS_MAC)

    # The bundle (KitNote.app) and its binary are named after the product, as KitNote.exe is on Windows.
    set(EXECUTABLE_NAME ${KITNOTE_APP_NAME})

    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${KITNOTE_APP_GUI_IDENTIFIER})
    set(MACOSX_BUNDLE_BUNDLE_NAME ${KITNOTE_APP_TITLE})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${KITNOTE_APP_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${KITNOTE_APP_VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${CMAKE_BUILD_NUMBER})
    set(MACOSX_BUNDLE_COPYRIGHT "Copyright © 1999-2026 MuseScore Limited. Published under the GNU General Public License version 3.")

    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${QT_INSTALL_LIBS};@loader_path/../Frameworks")
    if (KITNOTE_COMPILE_MACOS_PRECOMPILED_DEPS_PATH)
        list(PREPEND CMAKE_INSTALL_RPATH "${KITNOTE_COMPILE_MACOS_PRECOMPILED_DEPS_PATH}/lib")
    endif()

    message("MACOSX_BUNDLE_VERSION: ${MACOSX_BUNDLE_LONG_VERSION_STRING}.${MACOSX_BUNDLE_BUNDLE_VERSION}")

elseif(OS_IS_WASM)

    set(EXECUTABLE_NAME KitNote)

elseif(OS_IS_FBSD)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L /usr/local/lib -lasound -lpthread")

    set(CMAKE_INSTALL_RPATH "${QT_INSTALL_LIBS}")
    if (BUILD_SHARED_LIBS)
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH};${SHARED_LIBS_INSTALL_DESTINATION}")
    endif(BUILD_SHARED_LIBS)
else()
    message(FATAL_ERROR "Unsupported Platform: ${CMAKE_HOST_SYSTEM_NAME}")
endif()

###########################################
# Dependency library declaration
###########################################
set(LINK_LIB

    # Always needed by everyone
    ${QT_LIBRARIES}
    kfw::global
    kfw::draw
    )

macro(add_to_link_if_exists lib)
    if (TARGET ${lib})
        list(APPEND LINK_LIB ${lib})

        if (TARGET ${lib}_qml)
            list(APPEND LINK_LIB ${lib}_qml)
        endif()
    endif()
endmacro()

# NOTE There may be cases:
# 1. The module is enabled and the target exists.
# 2. The module is disabled, but there is a stub, the target exists
# 3. The module is disabled and the target does not exist.
# This function also adds qml parts to the link
add_to_link_if_exists(kfw::accessibility)
add_to_link_if_exists(kfw::actions)
add_to_link_if_exists(kfw::audio)
add_to_link_if_exists(kfw::audioplugins)
add_to_link_if_exists(kfw::diagnostics)
add_to_link_if_exists(kfw::dockwindow)
add_to_link_if_exists(kfw::languages)
add_to_link_if_exists(kfw::learn)
add_to_link_if_exists(kfw::media)
add_to_link_if_exists(kfw::midi)
add_to_link_if_exists(kfw::midiremote)
add_to_link_if_exists(kfw::mpe)
add_to_link_if_exists(kfw::multiwindows)
add_to_link_if_exists(kfw::network)
add_to_link_if_exists(kfw::shortcuts)
add_to_link_if_exists(kfw::tours)
add_to_link_if_exists(kfw::ui)
add_to_link_if_exists(kfw::ui_dialogs_qml)
add_to_link_if_exists(kfw::uicomponents)
add_to_link_if_exists(kfw::uicomponents_qmlapi)
add_to_link_if_exists(kfw::graphicaleffects_qml)
add_to_link_if_exists(kfw::legacytreeview_qml)
add_to_link_if_exists(kfw::update)
add_to_link_if_exists(kfw::vst)
add_to_link_if_exists(kfw::workspace)

add_to_link_if_exists(appshell)
add_to_link_if_exists(braille)
add_to_link_if_exists(context)
add_to_link_if_exists(converter)
add_to_link_if_exists(engraving)
add_to_link_if_exists(iex_audioexport)
add_to_link_if_exists(iex_bb)
add_to_link_if_exists(iex_bww)
add_to_link_if_exists(iex_capella)
add_to_link_if_exists(iex_guitarpro)
add_to_link_if_exists(iex_imagesexport)
add_to_link_if_exists(iex_lyricsexport)
add_to_link_if_exists(iex_mei)
add_to_link_if_exists(iex_midi)
add_to_link_if_exists(iex_mnx)
add_to_link_if_exists(iex_musedata)
add_to_link_if_exists(iex_musicxml)
add_to_link_if_exists(iex_ove)
add_to_link_if_exists(iex_tabledit)
add_to_link_if_exists(inspector)
add_to_link_if_exists(instrumentsscene)
add_to_link_if_exists(notation)
add_to_link_if_exists(notationscene)
add_to_link_if_exists(palette)
add_to_link_if_exists(playback)
add_to_link_if_exists(preferences)
add_to_link_if_exists(print)
add_to_link_if_exists(project)     


set(MSCORE_APPEND_SRC)

if (OS_IS_WIN)
    set(MANIFEST_FILE_NAME "${EXECUTABLE_NAME}.manifest")
    configure_file("configs/app_win.manifest.in" ${MANIFEST_FILE_NAME} @ONLY)

    list(APPEND MSCORE_APPEND_SRC
        ${WINDOWS_ICONS_RC}
        "${CMAKE_CURRENT_BINARY_DIR}/${MANIFEST_FILE_NAME}"
    )
endif()

if (KITNOTE_CONFIGURATION_IS_APPWEB)
    list(APPEND LINK_LIB appjs)
endif()

###########################################
# Resources
###########################################
qt_add_resources(APP_RCC_SOURCES app.qrc)
if (OS_IS_WIN)
    qt_add_resources(APP_RCC_SOURCES app_win.qrc)
endif()

###########################################
# Executable declaration
###########################################
set(APP_SRC
    main.cpp
    cmdoptions.h
    appfactory.cpp
    appfactory.h

    internal/guiapp.cpp
    internal/guiapp.h
)

if (KITNOTE_ENABLE_CONSOLEAPP)
    set(APP_SRC ${APP_SRC}
        internal/commandlineparser.cpp
        internal/commandlineparser.h
        internal/consoleapp.cpp
        internal/consoleapp.h
    )
endif()

if (CC_IS_EMCC)
    set(APP_SRC ${APP_SRC} webapi_export.cpp)
endif()

qt_add_executable(KitNote
    WIN32 MACOSX_BUNDLE
    ${APP_RCC_SOURCES}
    ${MSCORE_APPEND_SRC}
    ${APP_SRC}
)

###########################################
# Setting target properties
###########################################

set_target_properties(KitNote PROPERTIES OUTPUT_NAME ${EXECUTABLE_NAME})

if (KFW_ENABLE_UNIT_TESTS_CODE_COVERAGE)
    set(COVERAGE_FLAGS -fprofile-arcs -ftest-coverage --coverage)
    set(COVERAGE_LINK_FLAGS -lgcov --coverage)
    target_compile_options(KitNote PRIVATE ${COVERAGE_FLAGS})
    target_link_options(KitNote PRIVATE ${COVERAGE_LINK_FLAGS})
endif()

if (OS_IS_MAC)
    include(${PROJECT_SOURCE_DIR}/share/icons/SetupIconComposerIcon.cmake)
    target_setup_iconcomposer_icon(KitNote "${PROJECT_SOURCE_DIR}/share/icons/AppIcon/AppIcon.icon")

    # KitNote: Icon Composer needs actool from Xcode 26. With an older Xcode the function above only
    # warns and sets nothing, so the prebuilt icon is used instead.
    if (NOT MACOSX_BUNDLE_ICON_NAME)
        set(KITNOTE_PREBUILT_ICNS "${PROJECT_SOURCE_DIR}/share/icons/AppIcon/kitnote_AppIcon.icns")
        target_sources(KitNote PRIVATE ${KITNOTE_PREBUILT_ICNS})
        set_source_files_properties(${KITNOTE_PREBUILT_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        set_target_properties(KitNote PROPERTIES MACOSX_BUNDLE_ICON_FILE kitnote_AppIcon.icns)
        set(MACOSX_BUNDLE_ICON_NAME kitnote_AppIcon)
    endif()

    set_target_properties(KitNote PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/buildscripts/packaging/macOS/Info.plist.in
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${MACOSX_BUNDLE_GUI_IDENTIFIER}
    )
endif (OS_IS_MAC)

if (CC_IS_MINGW)
    if (CMAKE_BUILD_TYPE MATCHES "DEBUG")
        set_target_properties(KitNote PROPERTIES LINK_FLAGS "-mwindows -mconsole")
    else (CMAKE_BUILD_TYPE MATCHES "DEBUG")
        set_target_properties(KitNote PROPERTIES LINK_FLAGS "-Wl,-S -mwindows")
    endif (CMAKE_BUILD_TYPE MATCHES "DEBUG")
endif(CC_IS_MINGW)

if (CC_IS_MSVC)
    target_link_options(KitNote PRIVATE /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup)
endif(CC_IS_MSVC)

###########################################
# Includes
###########################################

target_include_directories(KitNote PRIVATE
    ${PROJECT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}

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

###########################################
# Link
###########################################

target_link_libraries(KitNote PRIVATE ${LINK_LIB} ${COVERAGE_LINK_FLAGS})

if (CC_IS_EMCC)
    include(exported_functions.cmake)
    target_link_options(KitNote PRIVATE ${EMCC_LINKER_FLAGS})
    target_link_options(KitNote PRIVATE ${EXPORTED_FUNCTIONS})
endif()


###########################################
# INSTALL
###########################################

install(TARGETS KitNote
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION bin
)

###########################################
# Windows
###########################################
if (OS_IS_WIN)
    include(GetCompilerInfo)

    if (CC_IS_MINGW)
        get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
        get_filename_component (MINGW_ROOT ${COMPILER_DIR} DIRECTORY)

        install( FILES
               ${MINGW_ROOT}/bin/libgcc_s_seh-1.dll
               ${MINGW_ROOT}/bin/libstdc++-6.dll
               ${MINGW_ROOT}/bin/libwinpthread-1.dll
               DESTINATION bin)

    endif(CC_IS_MINGW)

    if (SNDFILE_DLL)
        install(FILES ${SNDFILE_DLL} DESTINATION bin)
    endif()

    if (KITNOTE_RUN_WINDEPLOYQT)
        # KitNote: leave out the DirectX shader compiler and Qt's own translation
        # folder (share/locale already has them). The software OpenGL fallback stays,
        # for machines without a working graphics driver.
        set(deploy_tool_options_arg "--plugindir bin --qmldir ${PROJECT_SOURCE_DIR} --no-system-dxc-compiler --no-translations")
        qt_generate_deploy_qml_app_script(
            TARGET KitNote
            OUTPUT_SCRIPT deploy_script
            NO_UNSUPPORTED_PLATFORM_ERROR
            DEPLOY_TOOL_OPTIONS ${deploy_tool_options_arg}
        )

        install(SCRIPT ${deploy_script})

        # KitNote: Qt Quick Controls styles the interface never selects, and the
        # Visual C++ installer (the runtime DLLs themselves stay next to the executable).
        install(CODE "
            file(GLOB _kitnote_unused
                \"\${CMAKE_INSTALL_PREFIX}/bin/Qt6QuickControls2Imagine*.dll\"
                \"\${CMAKE_INSTALL_PREFIX}/bin/Qt6QuickControls2Material*.dll\"
                \"\${CMAKE_INSTALL_PREFIX}/bin/Qt6QuickControls2Universal*.dll\"
                \"\${CMAKE_INSTALL_PREFIX}/bin/Qt6QuickControls2FluentWinUI3*.dll\"
                \"\${CMAKE_INSTALL_PREFIX}/bin/vc_redist*.exe\")
            if (_kitnote_unused)
                file(REMOVE \${_kitnote_unused})
            endif()
            # The QML debugging plugins are only for development.
            file(REMOVE_RECURSE \"\${CMAKE_INSTALL_PREFIX}/bin/qmltooling\")
            foreach(_style Imagine Material Universal FluentWinUI3)
                file(REMOVE_RECURSE \"\${CMAKE_INSTALL_PREFIX}/qml/QtQuick/Controls/\${_style}\")
            endforeach()
        ")
    endif() # KITNOTE_RUN_WINDEPLOYQT

###########################################
# Linux
###########################################
elseif(OS_IS_LIN)

###########################################
# FreeBSD
###########################################
elseif(OS_IS_FBSD)

###########################################
# macOS
###########################################
elseif(OS_IS_MAC)
    install(FILES
            ${PROJECT_SOURCE_DIR}/fonts/bravura/BravuraText.otf
            ${PROJECT_SOURCE_DIR}/fonts/campania/Campania.otf
            ${PROJECT_SOURCE_DIR}/fonts/edwin/Edwin-BdIta.otf
            ${PROJECT_SOURCE_DIR}/fonts/edwin/Edwin-Bold.otf
            ${PROJECT_SOURCE_DIR}/fonts/edwin/Edwin-Italic.otf
            ${PROJECT_SOURCE_DIR}/fonts/edwin/Edwin-Roman.otf
            ${PROJECT_SOURCE_DIR}/fonts/gootville/GootvilleText.otf
            ${PROJECT_SOURCE_DIR}/fonts/FreeSans.ttf
            ${PROJECT_SOURCE_DIR}/fonts/FreeSerif.ttf
            ${PROJECT_SOURCE_DIR}/fonts/FreeSerifBold.ttf
            ${PROJECT_SOURCE_DIR}/fonts/FreeSerifItalic.ttf
            ${PROJECT_SOURCE_DIR}/fonts/FreeSerifBoldItalic.ttf
            ${PROJECT_SOURCE_DIR}/fonts/leland/Leland.otf
            ${PROJECT_SOURCE_DIR}/fonts/leland/LelandText.otf
            ${PROJECT_SOURCE_DIR}/fonts/mscore-BC.ttf
            ${PROJECT_SOURCE_DIR}/fonts/mscoreTab.ttf
            ${PROJECT_SOURCE_DIR}/fonts/mscore/MScoreText.otf
            ${PROJECT_SOURCE_DIR}/fonts/musejazz/MuseJazzText.otf
            ${PROJECT_SOURCE_DIR}/fonts/petaluma/PetalumaText.otf
            ${PROJECT_SOURCE_DIR}/fonts/finalemaestro/FinaleMaestroText-Regular.otf
            ${PROJECT_SOURCE_DIR}/fonts/finalebroadway/FinaleBroadwayText.otf
            DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME}fonts
    )

    qt_generate_deploy_qml_app_script(
        TARGET KitNote
        OUTPUT_SCRIPT deploy_script
        MACOS_BUNDLE_POST_BUILD
    )

###########################################
# Wasm
###########################################
elseif(OS_IS_WASM)

    add_custom_command(
        TARGET KitNote
        POST_BUILD
        COMMAND node ${PROJECT_SOURCE_DIR}/src/web/appjs/install.js ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Install js api..."
    )

else()
    message(FATAL_ERROR "Unsupported Platform: ${CMAKE_HOST_SYSTEM_NAME}")
endif()


#################################################
# Miscellaneous Microsoft Visual Studio settings
#################################################
if (MSVC)
    # Add a post-build event to deploy the application to the output folder. Same as building the INSTALL target.
    string(TOUPPER ${CMAKE_GENERATOR} UPPER_CMAKE_GENERATOR)
    if (${UPPER_CMAKE_GENERATOR} MATCHES "VISUAL STUDIO")
        # We want $(Configuration) passed verbatim. Note the round brackets.
        set(CONFIG_STR "$(Configuration)")

        add_custom_command(
            TARGET KitNote POST_BUILD
            COMMAND ${CMAKE_COMMAND} -DBUILD_TYPE=${CONFIG_STR} -P cmake_install.cmake
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
            VERBATIM
        )
    endif()

    # Force the "install" and "package" targets not to depend on the "all" target.
    set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)
    set(CMAKE_SKIP_PACKAGE_ALL_DEPENDENCY true)

    # Set the startup project to "KitNote".
    set_property(DIRECTORY "${PROJECT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT KitNote)

    # Set the debugging properties for the "KitNote" project.
    file(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}/bin" VS_DEBUGGER_WORKING_DIRECTORY)
    set_target_properties(KitNote PROPERTIES VS_DEBUGGER_COMMAND "${VS_DEBUGGER_WORKING_DIRECTORY}\\${EXECUTABLE_NAME}${CMAKE_EXECUTABLE_SUFFIX}")
    set_target_properties(KitNote PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${VS_DEBUGGER_WORKING_DIRECTORY}")
    set_target_properties(KitNote PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "--debug")
endif (MSVC)
