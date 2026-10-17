
include(${CMAKE_CURRENT_LIST_DIR}/KfwModules.cmake)

# Disable sub-options of disabled modules
foreach(NAME ${KFW_FRAMEWORK_MODULES})
    if (NOT KFW_MODULE_${NAME})
        set(KFW_MODULE_${NAME}_API OFF)
        # set(KFW_MODULE_${NAME}_QML OFF) -- Most stubs have QML
        set(KFW_MODULE_${NAME}_TESTS OFF)
    endif()
endforeach()

if (NOT KFW_ENABLE_UNIT_TESTS)
    # disable unit tests
    foreach(NAME ${KFW_FRAMEWORK_MODULES})
        set(KFW_MODULE_${NAME}_TESTS OFF)
    endforeach()
endif()

if (NOT KFW_MODULE_UI)
    set(KFW_MODULE_UI_QML OFF) # Does not have stub that has QML
endif()

if (NOT KFW_MODULE_UI_QML)
    # Disable QML modules
    foreach(NAME ${KFW_FRAMEWORK_MODULES})
        set(KFW_MODULE_${NAME}_QML OFF)
    endforeach()
endif()

if (NOT KFW_MODULE_DIAGNOSTICS)
    set(KFW_MODULE_DIAGNOSTICS_QML OFF) # Does not have stub that has QML
endif()

if (NOT KFW_MODULE_MULTIWINDOWS)
    set(KFW_MODULE_MULTIWINDOWS_QML OFF) # Stub does not have QML
endif()

if (NOT KFW_MODULE_VST)
    set(KFW_MODULE_VST_QML OFF) # Stub does not have QML
endif()

if (KFW_QT_SUPPORT)
    add_compile_definitions(KORS_LOGGER_QT_SUPPORT)
else()
    add_compile_definitions(NO_QT_SUPPORT)
endif()

configure_file(${CMAKE_CURRENT_LIST_DIR}/kfw_framework_config.h.in kfw_framework_config.h )

include(KfwCreateModule)
