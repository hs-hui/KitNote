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

kfw_create_module(kfw_draw ALIAS kfw::draw)

target_sources(kfw_draw PRIVATE
    drawmodule.cpp
    drawmodule.h

    types/color.h
    types/geometry.h
    types/transform.h
    types/transform.cpp
    types/matrix.h
    types/painterpath.h
    types/painterpath.cpp
    types/bezier.h
    types/bezier.cpp
    types/drawtypes.h
    types/pen.h
    types/brush.h
    types/pixmap.h
    types/font.cpp
    types/font.h
    types/drawdata.h
    types/fontstypes.h

    painter.cpp
    painter.h
    ipaintprovider.h
    bufferedpaintprovider.cpp
    bufferedpaintprovider.h
    svgrenderer.cpp
    svgrenderer.h
    ifontprovider.h
    iimageprovider.h
    fontmetrics.cpp
    fontmetrics.h

    utils/drawlogger.cpp
    utils/drawlogger.h
    utils/drawdatajson.cpp
    utils/drawdatajson.h
    utils/drawdatacomp.cpp
    utils/drawdatacomp.h
    utils/drawdatarw.cpp
    utils/drawdatarw.h
    utils/drawdatapaint.cpp
    utils/drawdatapaint.h
)

if (DRAW_NO_INTERNAL)
    target_compile_definitions(kfw_draw PRIVATE DRAW_NO_INTERNAL DRAW_NO_QSVGRENDER)
else()
    target_sources(kfw_draw PRIVATE
        internal/qpainterprovider.cpp
        internal/qpainterprovider.h
        internal/qimageprovider.h
        internal/qimageprovider.cpp
        internal/qimagepainterprovider.cpp
        internal/qimagepainterprovider.h
    )

    # fonts
    target_sources(kfw_draw PRIVATE
        internal/fontproviderdispatcher.cpp
        internal/fontproviderdispatcher.h

        internal/qfontprovider.cpp
        internal/qfontprovider.h
        #internal/fontengineft.cpp
        #internal/fontengineft.h

        internal/ifontsengine.h
        internal/ifontsdatabase.h
        internal/ifontface.h
        internal/fontprovider.cpp
        internal/fontprovider.h
        internal/fontsdatabase.cpp
        internal/fontsdatabase.h
        internal/fontsengine.cpp
        internal/fontsengine.h
        internal/fontfaceft.cpp
        internal/fontfaceft.h
        internal/fontfacedu.cpp
        internal/fontfacedu.h
    )

    include(cmake/SetupFreeType.cmake)
    include(cmake/SetupHarfBuzz.cmake)

    target_compile_definitions(kfw_draw PUBLIC KFW_MODULE_DRAW_USE_QTTEXTDRAW)

    target_link_libraries(kfw_draw PRIVATE
        ${FREETYPE_LIBRARIES}
        ${HARFBUZZ_LIBRARIES}
        #msdfgen
    )

    target_include_directories(kfw_draw PRIVATE
        ${FREETYPE_INCLUDE_DIRS}
        ${HARFBUZZ_INCLUDE_DIRS}
        #${MSDFGEN_INCLUDE_DIRS}
    )

endif()

if (KFW_MODULE_DRAW_TRACE)
    target_compile_definitions(kfw_draw PUBLIC KFW_MODULE_DRAW_TRACE)
endif()

if (KFW_QT_SUPPORT)
    target_link_libraries(kfw_draw PRIVATE Qt::Gui Qt::Svg)
endif()

if (KFW_MODULE_DRAW_TESTS)
    add_subdirectory(tests)
endif()
