include(CMakeDependentOption)

macro(declare_kfw_module_opt name def)
    option(KFW_MODULE_${name} "Build ${name} module" ${def})

    # 1. if we disable the module, the submodule values are irrelevant, as they will be ignored
    # 2. if we enable the module, all its submodules are enabled by default
    # 3. we can disable some submodules manually
    option(KFW_MODULE_${name}_API "Build ${name} api" ON)
    option(KFW_MODULE_${name}_QML "Build ${name} QML" ON)
    option(KFW_MODULE_${name}_TESTS "Build ${name} tests" ON)
endmacro()

# Modules framework (alphabetical order please)
declare_kfw_module_opt(ACCESSIBILITY ON)
option(KFW_MODULE_ACCESSIBILITY_TRACE "Enable accessibility logging" OFF)

declare_kfw_module_opt(ACTIONS ON)

declare_kfw_module_opt(AUDIO ON)
option(KFW_MODULE_AUDIO_JACK "Enable jack support" OFF)

if (OS_IS_WIN)
    option(KFW_MODULE_AUDIO_ASIO "Enable asio support" ON)
endif()

if (OS_IS_LIN OR OS_IS_FBSD)
    option(KFW_MODULE_AUDIO_PIPEWIRE "Use PipeWire audio driver" OFF) # Turns ON on CI
endif()

option(KFW_MODULE_AUDIO_EXPORT "Enable audio export" ON)

# 1 - worker
# 2 - driver callback
# 3 - worker - RPC, driver callback - process
set(KFW_MODULE_AUDIO_WORKMODE 1 CACHE STRING "Audio subsystem work mode")

declare_kfw_module_opt(AUDIOPLUGINS ON)

declare_kfw_module_opt(DIAGNOSTICS ON)

declare_kfw_module_opt(DOCKWINDOW ON)

declare_kfw_module_opt(DRAW ON)
option(KFW_MODULE_DRAW_TRACE "Trace draw objects" OFF)
option(KFW_MODULE_DRAW_USE_QTFONTMETRICS "Use Qt font metrics (for some metrics)" ON)


declare_kfw_module_opt(GLOBAL ON)
option(KFW_MODULE_GLOBAL_LOGGER_DEBUGLEVEL "Enable logging debug level" ON)
option(KFW_MODULE_GLOBAL_MULTI_IOC "Enable multi ioc (multi windows)" OFF)

declare_kfw_module_opt(LANGUAGES ON)
declare_kfw_module_opt(MEDIA ON)
declare_kfw_module_opt(MIDI ON)
declare_kfw_module_opt(MIDIREMOTE ON)
declare_kfw_module_opt(MPE ON)
declare_kfw_module_opt(MULTIWINDOWS ON)

declare_kfw_module_opt(NETWORK ON)
option(KFW_MODULE_NETWORK_WEBSOCKET "Enable websocket support" OFF)

declare_kfw_module_opt(SHORTCUTS ON)

declare_kfw_module_opt(TOURS ON)

declare_kfw_module_opt(UI ON)
option(KFW_MODULE_UI_DISABLE_MODALITY "Disable dialogs modality for testing purpose" OFF)
option(KFW_MODULE_UI_SYSTEMDRAG_SUPPORTED "System drag supported" ON)
option(KFW_MODULE_UI_SYNCINTERACTIVE_SUPPORTED "Sync interactive supported" ON)
option(KFW_MODULE_UI_NAVIGATION_EXCLUDEPROJECT "Exclude the main project view from navigation" ON)


set(VST3_SDK_VERSION "3.7")
declare_kfw_module_opt(VST OFF)
set(KFW_MODULE_VST_VST3_SDK_PATH "" CACHE PATH "Path to VST3_SDK. SDK version >= ${VST3_SDK_VERSION} required")

declare_kfw_module_opt(WORKSPACE ON)

# === Enviropment ===
option(KFW_QT_SUPPORT "Build with Qt support" ON)
option(KFW_THREADS_SUPPORT "Build with threads support" ON)
option(KFW_CONFIGURATION_IS_WEB "Configuration is web" OFF)
option(KFW_COMPILE_BUILD_64 "Build 64 bit version" ON)
option(KFW_COMPILE_ASAN "Enable Address Sanitizer" OFF)
option(KFW_COMPILE_USE_PCH "Use precompiled headers." ON)
option(KFW_COMPILE_STRING_DEBUG_HACK "Enable string debug hack (only clang)" ON)
option(KFW_MULTICONTEXT_WIP "Enable transition code to support multi-context" OFF)

# === Tests ===
option(KFW_ENABLE_UNIT_TESTS "Build framework unit tests" ON)
option(KFW_ENABLE_UNIT_TESTS_CODE_COVERAGE "Enable code coverage for unit tests" OFF)

# === Tools ===
option(KFW_ENABLE_CUSTOM_ALLOCATOR "Enable custom allocator" OFF)
