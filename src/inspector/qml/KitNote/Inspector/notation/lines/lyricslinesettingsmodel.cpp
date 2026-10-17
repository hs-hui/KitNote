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
#include "lyricslinesettingsmodel.h"

#include "translation.h"

using namespace kn::inspector;

LyricsLineSettingsModel::LyricsLineSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                                 IElementRepositoryService* repository,
                                                 ElementType elementType)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    if (elementType == ElementType::LyricsLine) {
        setTitle(kfw::qtrc("inspector", "Lyrics line"));
        setElementType(kn::engraving::ElementType::LYRICSLINE);
        setModelType(InspectorModelType::TYPE_LYRICS_LINE);
    } else if (elementType == ElementType::PartialLyricsLine) {
        setTitle(kfw::qtrc("inspector", "Partial lyrics line"));
        setElementType(kn::engraving::ElementType::PARTIAL_LYRICSLINE);
        setModelType(InspectorModelType::TYPE_PARTIAL_LYRICS_LINE);
        m_hasVerse = true;
    }
    setIcon(kfw::ui::IconCode::Code::LYRICS);

    createProperties();
}

PropertyItem* LyricsLineSettingsModel::thickness() const
{
    return m_thickness;
}

PropertyItem* LyricsLineSettingsModel::verse() const
{
    return m_verse;
}

bool LyricsLineSettingsModel::hasVerse() const
{
    return m_hasVerse;
}

void LyricsLineSettingsModel::createProperties()
{
    m_thickness = buildPropertyItem(kn::engraving::Pid::LINE_WIDTH);
    m_verse = buildPropertyItem(kn::engraving::Pid::VERSE);
}

void LyricsLineSettingsModel::loadProperties()
{
    loadPropertyItem(m_thickness);
    loadPropertyItem(m_verse);
}

void LyricsLineSettingsModel::resetProperties()
{
    m_thickness->resetToDefault();
    m_verse->resetToDefault();
}
