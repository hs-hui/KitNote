/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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
#include "symbolsettingsmodel.h"

#include "engraving/types/symnames.h"

#include "translation.h"

using namespace kn::inspector;

SymbolSettingsModel::SymbolSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx, IElementRepositoryService* repository)
    : AbstractInspectorModel(parent, iocCtx, repository)
{
    setModelType(InspectorModelType::TYPE_SYMBOL);
    setTitle(kfw::qtrc("inspector", "Symbol"));
    setIcon(kfw::ui::IconCode::Code::TRIANGLE_SYMBOL);
    createProperties();
}

void SymbolSettingsModel::createProperties()
{
    m_sym = buildPropertyItem(kn::engraving::Pid::SYMBOL);
    m_scoreFont = buildPropertyItem(kn::engraving::Pid::SCORE_FONT);
    m_symbolSize = buildPropertyItem(kn::engraving::Pid::SYMBOLS_SIZE,
                                     [this](const kn::engraving::Pid pid, const QVariant& newValue) {
        onPropertyValueChanged(pid, newValue.toDouble() / 100);
    },
                                     [this](const kn::engraving::Sid sid, const QVariant& newValue) {
        updateStyleValue(sid, newValue.toDouble() / 100);
        emit requestReloadPropertyItems();
    });
    m_symAngle = buildPropertyItem(kn::engraving::Pid::SYMBOL_ANGLE);
}

void SymbolSettingsModel::requestElements()
{
    m_elementList = m_repository->findElementsByType(kn::engraving::ElementType::SYMBOL);
    auto dividers = m_repository->findElementsByType(kn::engraving::ElementType::SYSTEM_DIVIDER);
    m_elementList.append(dividers);
}

void SymbolSettingsModel::loadProperties()
{
    loadPropertyItem(m_sym);
    loadPropertyItem(m_scoreFont);
    loadPropertyItem(m_symbolSize, [](const QVariant& elementPropertyValue) -> QVariant {
        return kfw::DataFormatter::roundDouble(elementPropertyValue.toDouble()) * 100;
    });
    loadPropertyItem(m_symAngle);
}

void SymbolSettingsModel::resetProperties()
{
    m_sym->resetToDefault();
    m_scoreFont->resetToDefault();
    m_symbolSize->resetToDefault();
    m_symAngle->resetToDefault();
}

PropertyItem* SymbolSettingsModel::sym() const
{
    return m_sym;
}

PropertyItem* SymbolSettingsModel::scoreFont() const
{
    return m_scoreFont;
}

PropertyItem* SymbolSettingsModel::symbolSize() const
{
    return m_symbolSize;
}

PropertyItem* SymbolSettingsModel::symAngle() const
{
    return m_symAngle;
}

QVariantList SymbolSettingsModel::symFonts()
{
    if (m_symFonts.empty()) {
        for (const engraving::IEngravingFontPtr& f : engravingFonts()->fonts()) {
            QVariantMap style;
            style["text"] = QString::fromStdString(f->name());
            style["value"] = QString::fromStdString(f->name());
            m_symFonts << style;
        }
    }
    return m_symFonts;
}
