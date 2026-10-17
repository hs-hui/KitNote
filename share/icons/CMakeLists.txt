# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2022 MuseScore Limited and others
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

include(GetPlatformInfo)

if (OS_IS_MAC)
    # See also SetupIconComposerIcon.cmake

    install(FILES MsczIcon/kitnote_MsczIcon.icns RENAME MsczIcon.icns DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME})
    install(FILES MscxIcon/kitnote_MscxIcon.icns RENAME MscxIcon.icns DESTINATION ${KITNOTE_SHARE_NAME}${KITNOTE_INSTALL_NAME})

elseif(OS_IS_WIN)
    # See also windows_icons.rc

elseif(OS_IS_LIN)
    foreach(SIZE 16 24 32 48 64 96 128 512)
        install(FILES AppIcon/kitnote_AppIcon_${SIZE}x${SIZE}.png 
                RENAME kitnote${KITNOTE_APP_INSTALL_SUFFIX}.png 
                DESTINATION share/icons/hicolor/${SIZE}x${SIZE}/apps)
    endforeach()

    install(FILES MsczIcon/kitnote_MsczIcon_512x512.png 
            RENAME application-x-musescore${KITNOTE_APP_INSTALL_SUFFIX}.png
            DESTINATION share/icons/hicolor/512x512/mimetypes)
    install(FILES MscxIcon/kitnote_MscxIcon_512x512.png 
            RENAME application-x-musescore${KITNOTE_APP_INSTALL_SUFFIX}+xml.png
            DESTINATION share/icons/hicolor/512x512/mimetypes)
    install(FILES MsczIcon/kitnote_MsczIcon.svg 
            RENAME application-x-musescore${KITNOTE_APP_INSTALL_SUFFIX}.svg
            DESTINATION share/icons/hicolor/scalable/mimetypes)
    install(FILES MscxIcon/kitnote_MscxIcon.svg 
            RENAME application-x-musescore${KITNOTE_APP_INSTALL_SUFFIX}+xml.svg
            DESTINATION share/icons/hicolor/scalable/mimetypes)

    # Note: Must now run "gtk-update-icon-cache" to set the new icons.
endif()
