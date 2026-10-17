# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
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

# Vendored zlib 1.3.2 (https://zlib.net/zlib-1.3.2.tar.gz, SHA-256
# bb329a0a2cd0274d05519d61c667c062e06990d72e125ee2dfa8de64f0119d16).
# Licence: zlib, see LICENSE in this folder.
# Built as the static library zlibstat, which is what KitNote links on MSVC.

add_library(zlibstat STATIC
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    infback.c
    inffast.c
    inflate.c
    inftrees.c
    trees.c
    uncompr.c
    zutil.c
)

target_include_directories(zlibstat PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(zlibstat PRIVATE
    _CRT_SECURE_NO_DEPRECATE
    _CRT_NONSTDC_NO_DEPRECATE
)

set_target_properties(zlibstat PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC OFF
)
