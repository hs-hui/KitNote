/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2021 MuseScore Limited and others
 * Modified for KitNote, 2026
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
 
import QtQuick 

import KitNote.Ui

Rectangle {
    id: root

    property AbstractNavigation navigationCtrl: null
    property bool drawOutsideParent: true

    //! NOTE: sometimes, the contrast between the item and the navigation focus
    //! border might be very low, for example when the item is an image.
    //! Therefore, we sometimes add some padding between the item and the border,
    //! to keep the border easily distinguishable.
    property real padding: 0

    anchors.fill: parent
    anchors.margins: drawOutsideParent ? -border.width - padding : 0

    visible: navigationCtrl ? navigationCtrl.highlight : false

    color: "transparent"

    border.color: ui.theme.fontPrimaryColor
    border.width: ui.theme.navCtrlBorderWidth
    radius: Number(parent.radius) > 0
            ? parent.radius - anchors.margins
            : 0
}
