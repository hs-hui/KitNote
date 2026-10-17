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

# - Registers the given path as QML import path for QtCreator
macro(add_qml_import_path import_path)
    if (NOT ${import_path} STREQUAL "")
        set(QML_IMPORT_PATH "$CACHE{QML_IMPORT_PATH}")
        list(APPEND QML_IMPORT_PATH ${import_path})
        list(REMOVE_DUPLICATES QML_IMPORT_PATH)
        set(QML_IMPORT_PATH "${QML_IMPORT_PATH}" CACHE STRING
            "QtCreator extra import paths for QML modules" FORCE)
    endif()
endmacro()

# - Sets up precompiled headers for the given target, with a workaround for clang + ccache
function(target_precompile_headers_clang_ccache target)
    target_precompile_headers(${target} ${ARGN})

    # https://discourse.cmake.org/t/ccache-clang-and-fno-pch-timestamp/7253
    if (CC_IS_CLANG AND COMPILER_CACHE_PROGRAM)
        target_compile_options(${target} PRIVATE
            "$<$<COMPILE_LANGUAGE:CXX>:SHELL:-Xclang -fno-pch-timestamp>"
        )
    endif()
endfunction()

# - Creates a target and sets up common properties for Muse modules
function(kfw_create_module target_name)
    set(options NO_COVERAGE NO_QT NO_PCH NO_UNITY STUB)
    set(oneValueArgs ALIAS)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${oneValueArgs}" "")

    # Status message
    set(message "Configuring ${target_name}")
    if (arg_ALIAS)
        set(message "${message} <${arg_ALIAS}>")
    endif()
    if (arg_STUB)
        set(message "${message} [stub]")
    endif()
    message(STATUS "${message}")

    # Create target
    if (NOT arg_NO_QT AND KFW_QT_SUPPORT)
        # STATIC/SHARED based on BUILD_SHARED_LIBS, which is set in SetupBuildEnvironment.cmake
        qt_add_library(${target_name})
    else()
        # STATIC/SHARED based on BUILD_SHARED_LIBS, which is set in SetupBuildEnvironment.cmake
        add_library(${target_name})

        set_target_properties(${target_name} PROPERTIES
            AUTOMOC OFF
            AUTOUIC OFF
            AUTORCC OFF
        )
    endif()

    # Alias target
    if (arg_ALIAS)
        add_library(${arg_ALIAS} ALIAS ${target_name})
    endif()

    # Include directories
    if (NOT KFW_FRAMEWORK_PATH)
        set(KFW_FRAMEWORK_PATH ${PROJECT_SOURCE_DIR})
    endif()

    target_include_directories(${target_name} PRIVATE
        ${PROJECT_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}

        ${PROJECT_SOURCE_DIR}/src

        ${KFW_FRAMEWORK_PATH}
        ${KFW_FRAMEWORK_PATH}/framework
        ${KFW_FRAMEWORK_PATH}/framework/global
        ${KFW_FRAMEWORK_PATH}/framework/testing/thirdparty/googletest/googletest/include

        # compat
        ${KFW_FRAMEWORK_PATH}/src
        ${KFW_FRAMEWORK_PATH}/src/framework
        ${KFW_FRAMEWORK_PATH}/src/framework/global
        ${KFW_FRAMEWORK_PATH}/src/framework/testing/thirdparty/googletest/googletest/include
        # end compat
    )

    # Precompiled header
    if (NOT arg_NO_PCH AND KFW_COMPILE_USE_PCH)
        target_precompile_headers_clang_ccache(${target_name} REUSE_FROM kfw_pch)
        target_link_libraries(${target_name} PRIVATE kfw_pch)
    endif()

    # Unity build
    if (arg_NO_UNITY)
        set_target_properties(${target_name} PROPERTIES UNITY_BUILD OFF)
    elseif(KFW_COMPILE_USE_UNITY)
        set_target_properties(${target_name} PROPERTIES UNITY_BUILD ON)
    endif()

    # Code coverage
    if (NOT arg_NO_COVERAGE AND KFW_ENABLE_UNIT_TESTS_CODE_COVERAGE)
        set(COVERAGE_FLAGS -fprofile-arcs -ftest-coverage --coverage)
        target_compile_options(${target_name} PRIVATE ${COVERAGE_FLAGS})
        target_link_options(${target_name} PRIVATE -lgcov --coverage -fprofile-arcs -ftest-coverage)
    endif()

    # Link with global module
    if (NOT ${target_name} STREQUAL kfw_global)
        target_link_libraries(${target_name} PRIVATE kfw_global)
    endif()
endfunction()

function(kfw_create_qml_module target_name)
    set(oneValueArgs FOR)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "${oneValueArgs}" "")

    kfw_create_module(${target_name} ${arg_UNPARSED_ARGUMENTS})
    target_link_libraries(${target_name} INTERFACE ${target_name}plugin)

    if (arg_FOR)
        get_target_property(_for_dir ${arg_FOR} SOURCE_DIR)
        target_include_directories(${target_name} PRIVATE ${_for_dir})

        target_link_libraries(${target_name} PRIVATE ${arg_FOR})

        # This might not be the cleanest way to obtain this path, but it is a
        # good balance between simplicity and correctness
        get_target_property(_for_binary_dir ${arg_FOR} BINARY_DIR)
        add_qml_import_path(${_for_binary_dir}/qml)
    endif()
endfunction()

# - Creates a target and sets up common properties for Muse thirdparty modules
function(kfw_create_thirdparty_module target_name)
    set(options INTERFACE)
    set(oneValueArgs ALIAS)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "${oneValueArgs}" "")

    # Status message
    set(message "Configuring ${target_name}")
    if (arg_ALIAS)
        set(message "${message} <${arg_ALIAS}>")
    endif()
    message(STATUS "${message}")

    # Create target
    # STATIC/SHARED based on BUILD_SHARED_LIBS, which is set in SetupBuildEnvironment.cmake

    if (arg_INTERFACE)
        add_library(${target_name} INTERFACE)
        if (arg_UNPARSED_ARGUMENTS)
            target_link_libraries(${target_name} INTERFACE ${arg_UNPARSED_ARGUMENTS})
        endif()
    else()
        add_library(${target_name})
    endif()

    set_target_properties(${target_name} PROPERTIES
        AUTOMOC OFF
        AUTOUIC OFF
        AUTORCC OFF
    )

    # Alias target
    if (arg_ALIAS)
        add_library(${arg_ALIAS} ALIAS ${target_name})
    endif()
endfunction()

function(kfw_module_add_qrc target_name)
    if (NOT KFW_QT_SUPPORT)
        message(WARNING "Building without Qt support, cannot add QRC to target ${target_name}")
        return()
    endif()

    set(options BIG_RESOURCES)
    cmake_parse_arguments(PARSE_ARGV 1 arg "${options}" "" "")

    if (arg_BIG_RESOURCES)
        qt_add_big_resources(QRC_SOURCES ${arg_UNPARSED_ARGUMENTS})
    else()
        qt_add_resources(QRC_SOURCES ${arg_UNPARSED_ARGUMENTS})
    endif()

    target_sources(${target_name} PRIVATE ${QRC_SOURCES})
endfunction()

function(fixup_qml_module_dependencies target_name)
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        # The Visual Studio generator doesn't correctly resolve the dependencies for "qmltyperegistration"
        # generated code files, unless we add this explicit target-level dependency. Other generators
        # don't seem to have this problem.
        add_dependencies(${target_name}_qmltyperegistration ${target_name})
    endif()
endfunction()
