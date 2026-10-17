#!/bin/sh
# SPDX-License-Identifier: GPL-3.0-only
#
# KitNote
# Drum notation, built on MuseScore Studio
#
# Copyright (C) 2021 MuseScore Limited and others
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
cd "$(dirname "$0")"

case $1 in
(portable|p)
	portable=''
	;;
(*)
	portable='.\\"'
	;;
esac

sed \
    -e 's!@MAN_KITNOTE_UPPER@!KITNOTE!g' \
    -e 's!@Variables_substituted_by_CMAKE_on_installation@!!g' \
    -e 's!@KITNOTE_APP_INSTALL_SUFFIX@!!g' \
    -e 's!@KITNOTE_APP_NAME_VERSION@!MuseScore 4!g' \
    -e 's!@MAN_PORTABLE@!'"$portable"'!g' \
    -e 's!@PORTABLE_INSTALL_NAME@!MuseScorePortable!g' \
    -e 's!@CMAKE_INSTALL_PREFIX@!/usr!g' \
    -e 's!@KITNOTE_SHARE_NAME@!share/!g' \
    -e 's!@KITNOTE_INSTALL_NAME@!kitnote-4.0/!g' \
    <kitnote.1.in | man -l -
