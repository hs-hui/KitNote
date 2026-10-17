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

#include "editscoreproperties.h"

#include "../dom/score.h"
#include "../dom/scoreorder.h"

using namespace kn::engraving;

//---------------------------------------------------------
//   ChangeMetaTags
//---------------------------------------------------------

void ChangeMetaTags::flip(EditData*)
{
    std::map<String, String> t = score->metaTags();
    score->setMetaTags(metaTags);
    score->setLayoutAll();
    metaTags = t;
}

//---------------------------------------------------------
//   ChangeMetaText
//---------------------------------------------------------

void ChangeMetaText::flip(EditData*)
{
    String s = score->metaTag(id);
    score->setMetaTag(id, text);
    score->setLayoutAll();
    text = s;
}

//---------------------------------------------------------
//   ChangeScoreOrder
//---------------------------------------------------------

void ChangeScoreOrder::flip(EditData*)
{
    ScoreOrder s = score->scoreOrder();
    score->setScoreOrder(order);
    order = s;
}

//---------------------------------------------------------
//   ChangePageNumberOffset
//---------------------------------------------------------

void ChangePageNumberOffset::flip(EditData*)
{
    int po = score->pageNumberOffset();

    score->setPageNumberOffset(pageOffset);
    score->setLayoutAll();

    pageOffset = po;
}
