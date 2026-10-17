# SPDX-License-Identifier: GPL-3.0-only
# KitNote: kitnote-mcp, KitNote as a Model Context Protocol server for AI tools (see main.cpp).

add_executable(kitnote-mcp main.cpp)
target_link_libraries(kitnote-mcp PRIVATE Qt6::Core)
set_target_properties(kitnote-mcp PROPERTIES AUTOMOC OFF)

if (OS_IS_MAC)
    # KitNote.app/Contents/MacOS, beside KitNote itself: Preferences > AI finds it there (serverPath)
    # and the Qt frameworks of the bundle (Contents/Frameworks) are found from there
    set_target_properties(kitnote-mcp PROPERTIES
        BUILD_WITH_INSTALL_RPATH OFF
        INSTALL_RPATH "@executable_path/../Frameworks")
    install(TARGETS kitnote-mcp RUNTIME DESTINATION ${KITNOTE_SHARE_NAME}Contents/MacOS)
else()
    install(TARGETS kitnote-mcp RUNTIME DESTINATION bin)
endif()
