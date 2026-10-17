include(GetPlatformInfo)

if(NOT(OS_IS_LIN OR OS_IS_FBSD))
    return()
endif()

# ### PACKAGING for Linux and BSD based systems (more in mscore/CMakeLists.txt) ####
#
# set library search path for runtime linker to load the same
# qt libraries as we used at compile time
#
if(KITNOTE_APP_UNSTABLE)
    # Use short name to avoid truncation by GNOME launcher. Save room for
    # a suffix in case multiple dev/nightly builds are installed.
    set(DESKTOP_LAUNCHER_NAME "KN ${KITNOTE_APP_VERSION_MAJ_MIN}") # KitNote X.Y
else(KITNOTE_APP_UNSTABLE)
    # Use full name for stable releases
    set(DESKTOP_LAUNCHER_NAME "${KITNOTE_APP_TITLE} ${KITNOTE_APP_VERSION_MAJ_MIN}") # KitNote X.Y
endif(KITNOTE_APP_UNSTABLE)

if(${KITNOTE_APP_INSTALL_SUFFIX} MATCHES "dev")
    set(DESKTOP_LAUNCHER_NAME "${DESKTOP_LAUNCHER_NAME} Dev")
elseif(${KITNOTE_APP_INSTALL_SUFFIX} MATCHES "nightly")
    set(DESKTOP_LAUNCHER_NAME "${DESKTOP_LAUNCHER_NAME} Nightly")
elseif(${KITNOTE_APP_INSTALL_SUFFIX} MATCHES "testing")
    set(DESKTOP_LAUNCHER_NAME "${DESKTOP_LAUNCHER_NAME} Testing")
endif(${KITNOTE_APP_INSTALL_SUFFIX} MATCHES "dev")

if(${KITNOTE_APP_INSTALL_SUFFIX} MATCHES "portable") # Note: "portableanything" would match
    set(DESKTOP_LAUNCHER_NAME "${DESKTOP_LAUNCHER_NAME} Portable") # distinguish our build from distro packages

    # Build portable AppImage as per https://github.com/probonopd/AppImageKit
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/portable)

    if(NOT DEFINED ARCH)
        execute_process(COMMAND arch OUTPUT_VARIABLE ARCH OUTPUT_STRIP_TRAILING_WHITESPACE) # Get architecture (strip trailing newline)
    endif(NOT DEFINED ARCH)

    get_filename_component(PORTABLE_INSTALL_PATH ${CMAKE_INSTALL_PREFIX} PATH) # Get path (dirname)
    get_filename_component(PORTABLE_INSTALL_NAME ${CMAKE_INSTALL_PREFIX} NAME) # Strip path (basename)

    if(NOT KITNOTE_APP_UNSTABLE)
        set(PORTABLE_INSTALL_NAME "${PORTABLE_INSTALL_NAME}-${KITNOTE_APP_VERSION}") # Append version info.
    endif(NOT KITNOTE_APP_UNSTABLE)

    set(PORTABLE_INSTALL_NAME "${PORTABLE_INSTALL_NAME}-${ARCH}") # Append system architecture.
    set(CMAKE_INSTALL_PREFIX ${PORTABLE_INSTALL_PATH}/${PORTABLE_INSTALL_NAME}.AppDir) # E.g. "KitNote-X.Y.Z-x86_64.AppDir"
    execute_process(COMMAND echo ${CMAKE_INSTALL_PREFIX} OUTPUT_FILE PREFIX.txt)

    # Prepare portable scripts:
    configure_file(${CMAKE_CURRENT_LIST_DIR}/portable/AppRun.in AppRun @ONLY)
    configure_file(${CMAKE_CURRENT_LIST_DIR}/portable/portable-utils.in portable-utils @ONLY)
    install(PROGRAMS ${PROJECT_BINARY_DIR}/AppRun DESTINATION . COMPONENT portable)
    install(PROGRAMS ${PROJECT_BINARY_DIR}/portable-utils
        ${CMAKE_CURRENT_LIST_DIR}/portable/ldd-recursive
        buildscripts/packaging/Linux+BSD/portable/rm-empty-dirs DESTINATION bin COMPONENT portable)
else(${KITNOTE_APP_INSTALL_SUFFIX} MATCHES "portable")
    set(MAN_PORTABLE ".\\\"") # comment out lines in man page that are only relevant to the portable version
endif(${KITNOTE_APP_INSTALL_SUFFIX} MATCHES "portable")

# Identify App's main window so that it receives the correct name
# and icon in the OS dock / taskbar. Run `xprop WM_CLASS` and click on
# App's main window to find out what string to use here.
if(KITNOTE_APP_UNSTABLE)
    set(WINDOW_MANAGER_CLASS "${KITNOTE_APP_NAME_MACHINE_READABLE_COMPAT}${KITNOTE_APP_VERSION_MAJOR}Development")
else()
    set(WINDOW_MANAGER_CLASS "${KITNOTE_APP_NAME_MACHINE_READABLE_COMPAT}${KITNOTE_APP_VERSION_MAJOR}")
endif()

# Install desktop file (perform variable substitution first)
configure_file(${CMAKE_CURRENT_LIST_DIR}/org.kitnote.KitNote.desktop.in org.kitnote.KitNote${KITNOTE_APP_INSTALL_SUFFIX}.desktop)
install(FILES ${PROJECT_BINARY_DIR}/org.kitnote.KitNote${KITNOTE_APP_INSTALL_SUFFIX}.desktop DESTINATION share/applications)

# Install appdata file (perform variable substitution first)
if("${KITNOTE_APP_INSTALL_SUFFIX}" MATCHES "-")
    message(FATAL_ERROR
        "KITNOTE_APP_INSTALL_SUFFIX='${KITNOTE_APP_INSTALL_SUFFIX}'\n"
        "KITNOTE_APP_INSTALL_SUFFIX must not contain hyphen characters. It will be used "
        "inside the <id> tag in *.appdata.xml and hyphens are discouraged there."
    )
endif("${KITNOTE_APP_INSTALL_SUFFIX}" MATCHES "-")

configure_file(${CMAKE_CURRENT_LIST_DIR}/org.kitnote.KitNote.appdata.xml.in org.kitnote.KitNote${KITNOTE_APP_INSTALL_SUFFIX}.appdata.xml)
install(FILES ${PROJECT_BINARY_DIR}/org.kitnote.KitNote${KITNOTE_APP_INSTALL_SUFFIX}.appdata.xml DESTINATION share/metainfo)

# Substitute variables within man pages
set(MAN_NAME kitnote)
set(MAN_EXTENSION .1)
set(MAN_FULL_NAME ${MAN_NAME}${KITNOTE_APP_INSTALL_SUFFIX}${MAN_EXTENSION})
set(MAN_TARGET ${CMAKE_CURRENT_LIST_DIR}/${MAN_NAME}${MAN_EXTENSION}.in)
set(MAN_BUILD ${PROJECT_BINARY_DIR}/${MAN_FULL_NAME})
string(TOUPPER "kitnote${KITNOTE_APP_INSTALL_SUFFIX}" MAN_KITNOTE_UPPER) # Command name shown in uppercase in man pages by convention
configure_file(${MAN_TARGET} ${MAN_BUILD})

# Compress man pages if gzip is installed (don't on OpenBSD)
# Note: Compressing man pages is normal on Linux but not OpenBSD
find_program(GZIP_EXECUTABLE gzip DOC "A tool for compressing manpages (optional).")

if(GZIP_EXECUTABLE AND NOT CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
    message(STATUS "Found 'gzip'. Man pages will be compressed.")
    set(MAN_TARGET ${MAN_BUILD})
    set(MAN_EXTENSION ${MAN_EXTENSION}.gz)
    set(MAN_FULL_NAME ${MAN_NAME}${KITNOTE_APP_INSTALL_SUFFIX}${MAN_EXTENSION})
        set(MAN_BUILD ${PROJECT_BINARY_DIR}/${MAN_FULL_NAME})
    add_custom_command(
        OUTPUT ${MAN_BUILD}
        DEPENDS ${MAN_TARGET}
        COMMAND ${GZIP_EXECUTABLE} -9 < ${MAN_TARGET} > ${MAN_BUILD}
    )
    add_custom_target(manpages ALL
        DEPENDS ${MAN_BUILD}
        COMMAND echo "Man pages have been compressed ready for installation."
        VERBATIM
    )
else(GZIP_EXECUTABLE AND NOT CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
    if(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
        message(STATUS "System is OpenBSD: Man pages will not be compressed.")
    else(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")
        message(STATUS "'gzip' not found (it is optional). Man pages will not be compressed.")
    endif(CMAKE_SYSTEM_NAME MATCHES "OpenBSD")

    add_custom_target(manpages ALL
        COMMAND echo "Man pages will be installed uncompressed."
        VERBATIM
    )
endif(GZIP_EXECUTABLE AND NOT CMAKE_SYSTEM_NAME MATCHES "OpenBSD")

# Install man pages in either compressed or uncompressed form
install(FILES ${MAN_BUILD} DESTINATION share/man/man1 COMPONENT doc)


# Add .KITNOTE, .MSCZ, .MSCX and .MSCS to MIME database (.KITNOTE is KitNote's own format; the others are MuseScore files that KitNote opens)
configure_file(${CMAKE_CURRENT_LIST_DIR}/kitnote.xml.in kitnote${KITNOTE_APP_INSTALL_SUFFIX}.xml)
install(FILES ${PROJECT_BINARY_DIR}/kitnote${KITNOTE_APP_INSTALL_SUFFIX}.xml DESTINATION share/mime/packages COMPONENT doc)

# Note: Must now run "update-mime-database" to apply changes.
