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
#include "vibratosettingsmodel.h"

#include "translation.h"

#include "engraving/dom/vibrato.h"

using namespace kn::inspector;

VibratoSettingsModel::VibratoSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                           IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository, kn::engraving::ElementType::VIBRATO)
{
    setModelType(InspectorModelType::TYPE_VIBRATO);
    setTitle(kfw::qtrc("inspector", "Vibrato"));
    setIcon(kfw::ui::IconCode::Code::VIBRATO);

    createProperties();
}

PropertyItem* VibratoSettingsModel::lineType() const
{
    return m_lineType;
}

PropertyItem* VibratoSettingsModel::placement() const
{
    return m_placement;
}

QVariantList VibratoSettingsModel::possibleLineTypes() const
{
    QMap<kn::engraving::VibratoType, QString> types {
        { kn::engraving::VibratoType::GUITAR_VIBRATO, kfw::qtrc("inspector", "Vibrato") },
        { kn::engraving::VibratoType::GUITAR_VIBRATO_WIDE, kfw::qtrc("inspector", "Vibrato wide") },
        { kn::engraving::VibratoType::VIBRATO_SAWTOOTH, kfw::qtrc("inspector", "Vibrato sawtooth") },
        { kn::engraving::VibratoType::VIBRATO_SAWTOOTH_WIDE, kfw::qtrc("inspector", "Vibrato sawtooth wide") }
    };

    QVariantList result;

    for (kn::engraving::VibratoType type : types.keys()) {
        QVariantMap obj;

        obj["text"] = types[type];
        obj["value"] = static_cast<int>(type);

        result << obj;
    }

    return result;
}

void VibratoSettingsModel::createProperties()
{
    m_lineType = buildPropertyItem(kn::engraving::Pid::VIBRATO_TYPE);
    m_placement = buildPropertyItem(kn::engraving::Pid::PLACEMENT);
}

void VibratoSettingsModel::loadProperties()
{
    loadPropertyItem(m_lineType);
    loadPropertyItem(m_placement);
}

void VibratoSettingsModel::resetProperties()
{
    m_lineType->resetToDefault();
    m_placement->resetToDefault();
}
