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

#include "chordbracketsettingsmodel.h"

#include "engraving/dom/arpeggio.h"

using namespace kn::inspector;
using namespace kn::engraving;

ChordBracketSettingsModel::ChordBracketSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                                     IElementRepositoryService* repository)
    : AbstractInspectorModel{parent, iocCtx, repository}
{
    setModelType(InspectorModelType::TYPE_CHORD_BRACKET);
    setElementType(kn::engraving::ElementType::CHORD_BRACKET);
    setTitle(kfw::qtrc("inspector", "Chord bracket"));

    createProperties();
}

void ChordBracketSettingsModel::createProperties()
{
    m_bracketRightSide = buildPropertyItem(Pid::BRACKET_RIGHT_SIDE);
    m_hookPos = buildPropertyItem(Pid::BRACKET_HOOK_POS);
    m_hookLen = buildPropertyItem(Pid::BRACKET_HOOK_LEN);
}

void ChordBracketSettingsModel::loadProperties()
{
    loadPropertyItem(m_bracketRightSide);
    loadPropertyItem(m_hookPos);
    loadPropertyItem(m_hookLen);

    updateIsBracket();
}

void ChordBracketSettingsModel::resetProperties()
{
    m_bracketRightSide->resetToDefault();
    m_hookPos->resetToDefault();
}

void ChordBracketSettingsModel::updateIsBracket()
{
    bool isBracket = true;
    for (EngravingItem* item : m_elementList) {
        if (item->isArpeggio() && toArpeggio(item)->arpeggioType() != ArpeggioType::BRACKET) {
            isBracket = false;
            break;
        }
    }

    if (m_isBracket != isBracket) {
        m_isBracket = isBracket;
        emit isBracketChanged(m_isBracket);
    }
}
