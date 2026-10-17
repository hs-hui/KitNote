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
#include "glissandosettingsmodel.h"
#include "types/linetypes.h"

#include "translation.h"

using namespace kn::inspector;

GlissandoSettingsModel::GlissandoSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                               IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository, kn::engraving::ElementType::GLISSANDO)
{
    setModelType(InspectorModelType::TYPE_GLISSANDO);
    setTitle(kfw::qtrc("inspector", "Glissando"));
    setIcon(kfw::ui::IconCode::Code::GLISSANDO);

    createProperties();
}

PropertyItem* GlissandoSettingsModel::lineType() const
{
    return m_lineType;
}

PropertyItem* GlissandoSettingsModel::showText() const
{
    return m_showText;
}

PropertyItem* GlissandoSettingsModel::text() const
{
    return m_text;
}

PropertyItem* GlissandoSettingsModel::thickness() const
{
    return m_thickness;
}

PropertyItem* GlissandoSettingsModel::lineStyle() const
{
    return m_lineStyle;
}

PropertyItem* GlissandoSettingsModel::dashLineLength() const
{
    return m_dashLineLength;
}

PropertyItem* GlissandoSettingsModel::dashGapLength() const
{
    return m_dashGapLength;
}

QVariantList GlissandoSettingsModel::possibleLineTypes() const
{
    QMap<kn::engraving::GlissandoType, QString> types {
        { kn::engraving::GlissandoType::STRAIGHT, kfw::qtrc("inspector", "Straight") },
        { kn::engraving::GlissandoType::WAVY, kfw::qtrc("inspector", "Wavy") }
    };

    QVariantList result;

    for (kn::engraving::GlissandoType type : types.keys()) {
        QVariantMap obj;

        obj["text"] = types[type];
        obj["value"] = static_cast<int>(type);

        result << obj;
    }

    return result;
}

void GlissandoSettingsModel::createProperties()
{
    auto applyPropertyValueAndUpdateAvailability = [this](const kn::engraving::Pid pid, const QVariant& newValue) {
        onPropertyValueChanged(pid, newValue);
        onUpdateGlissPropertiesAvailability();
    };

    m_lineType = buildPropertyItem(kn::engraving::Pid::GLISS_TYPE, applyPropertyValueAndUpdateAvailability);
    m_showText = buildPropertyItem(kn::engraving::Pid::GLISS_SHOW_TEXT);
    m_text = buildPropertyItem(kn::engraving::Pid::GLISS_TEXT, applyPropertyValueAndUpdateAvailability);

    m_thickness = buildPropertyItem(kn::engraving::Pid::LINE_WIDTH);

    m_lineStyle = buildPropertyItem(kn::engraving::Pid::LINE_STYLE, applyPropertyValueAndUpdateAvailability);
    m_dashLineLength = buildPropertyItem(kn::engraving::Pid::DASH_LINE_LEN);
    m_dashGapLength = buildPropertyItem(kn::engraving::Pid::DASH_GAP_LEN);
}

void GlissandoSettingsModel::loadProperties()
{
    loadPropertyItem(m_lineType);
    loadPropertyItem(m_showText);
    loadPropertyItem(m_text);

    loadPropertyItem(m_thickness);
    loadPropertyItem(m_lineStyle);
    loadPropertyItem(m_dashLineLength);
    loadPropertyItem(m_dashGapLength);

    onUpdateGlissPropertiesAvailability();
}

void GlissandoSettingsModel::resetProperties()
{
    m_lineType->resetToDefault();
    m_showText->resetToDefault();
    m_text->resetToDefault();

    m_thickness->resetToDefault();
    m_lineStyle->resetToDefault();
    m_dashLineLength->resetToDefault();
    m_dashGapLength->resetToDefault();
}

void GlissandoSettingsModel::onUpdateGlissPropertiesAvailability()
{
    bool isStraightLine = m_lineType->value().value<kn::engraving::GlissandoType>() == kn::engraving::GlissandoType::STRAIGHT;
    m_lineStyle->setIsEnabled(isStraightLine);
    m_thickness->setIsEnabled(isStraightLine);

    auto currentStyle = static_cast<LineTypes::LineStyle>(m_lineStyle->value().toInt());
    bool areDashPropertiesAvailable = currentStyle == LineTypes::LineStyle::LINE_STYLE_DASHED;

    m_dashLineLength->setIsEnabled(isStraightLine && areDashPropertiesAvailable);
    m_dashGapLength->setIsEnabled(isStraightLine && areDashPropertiesAvailable);
}
