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

#include "notationtoolbarmodel.h"

#include "uicomponents/qml/KitNote/UiComponents/toolbaritem.h"

using namespace kn::notation;
using namespace kfw::uicomponents;
using namespace kfw::actions;

void NotationToolBarModel::load()
{
    if (m_loaded) {
        return;
    }

    kfw::actions::ActionCodeList itemsCodes = {
        // KitNote: no "parts" button; a drum chart is a single part.
        "toggle-mixer"
    };

    ToolBarItemList items;
    for (const ActionCode& code : itemsCodes) {
        ToolBarItem* item = makeItem(code);
        item->setShowTitle(true);
        item->setIsTitleBold(true);

        items << item;
    }

    setItems(items);

    context()->currentMasterNotationChanged().onNotify(this, [this]() {
        load();
    });

    AbstractToolBarModel::load();

    m_loaded = true;
}
