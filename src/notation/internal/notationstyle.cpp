/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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
#include "notationstyle.h"

#include "io/file.h"
#include "types/translatablestring.h"

#include "engraving/dom/masterscore.h"
#include "engraving/dom/excerpt.h"
#include "engraving/editing/editstyle.h"
#include "engraving/style/defaultstyle.h"

using namespace kn::notation;
using namespace kfw::async;

NotationStyle::NotationStyle(IGetScore* getScore, INotationUndoStackPtr undoStack)
    : m_getScore(getScore), m_undoStack(undoStack)
{
}

const PropertyValue& NotationStyle::styleValue(const StyleId& styleId) const
{
    return score()->style().styleV(styleId);
}

const PropertyValue& NotationStyle::defaultStyleValue(const StyleId& styleId) const
{
    return engraving::DefaultStyle::defaultStyle().value(styleId);
}

void NotationStyle::setStyleValue(const StyleId& styleId, const PropertyValue& newValue)
{
    if (styleValue(styleId) == newValue) {
        return;
    }

    if (styleId == StyleId::concertPitch) {
        score()->cmdConcertPitchChanged(newValue.toBool());
    } else {
        score()->undoChangeStyleVal(styleId, newValue);
        score()->update();
    }

    m_styleChanged.notify();
}

void NotationStyle::resetStyleValue(const StyleId& styleId)
{
    score()->resetStyleValue(styleId);
    score()->update();
    m_styleChanged.notify();
}

void NotationStyle::resetStyleValues(const std::vector<StyleId>& styleIds)
{
    for (StyleId id : styleIds) {
        score()->resetStyleValue(id);
    }
    score()->update();
    m_styleChanged.notify();
}

bool NotationStyle::canApplyToAllParts() const
{
    return !score()->isMaster(); // In parts only
}

void NotationStyle::applyToAllParts()
{
    if (!canApplyToAllParts()) {
        return;
    }

    kn::engraving::MStyle style = m_getScore->score()->style();

    for (kn::engraving::Excerpt* excerpt : score()->masterScore()->excerpts()) {
        excerpt->excerptScore()->undo(new kn::engraving::ChangeStyle(excerpt->excerptScore(), style));
        excerpt->excerptScore()->update();
    }
}

void NotationStyle::resetAllStyleValues(const StyleIdSet& exceptTheseOnes)
{
    score()->cmdResetAllStyles(exceptTheseOnes);
    score()->update();
    m_styleChanged.notify();
}

Notification NotationStyle::styleChanged() const
{
    return m_styleChanged;
}

bool NotationStyle::loadStyle(const kfw::io::path_t& path, bool allowAnyVersion)
{
    m_undoStack->prepareChanges(kfw::TranslatableString("undoableAction", "Load style"));
    kfw::io::File styleFile(path);
    bool result = score()->loadStyle(styleFile, allowAnyVersion);
    m_undoStack->commitChanges();

    if (result) {
        styleChanged().notify();
    }

    return result;
}

bool NotationStyle::saveStyle(const kfw::io::path_t& path)
{
    return score()->saveStyle(path.toQString());
}

kn::engraving::Score* NotationStyle::score() const
{
    return m_getScore->score();
}
