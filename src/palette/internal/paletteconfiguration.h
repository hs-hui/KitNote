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
#ifndef KN_PALETTE_PALETTECONFIGURATION_H
#define KN_PALETTE_PALETTECONFIGURATION_H

#include "../ipaletteconfiguration.h"
#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "iglobalconfiguration.h"
#include "ui/iuiconfiguration.h"

namespace kn::palette {
class PaletteConfiguration : public IPaletteConfiguration, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;

public:
    PaletteConfiguration(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    void init();

    double paletteScaling() const override;
    void setPaletteScaling(double scale) override;

    double paletteSpatium() const override;

    kfw::ValCh<bool> isSinglePalette() const override;
    void setIsSinglePalette(bool isSingle) override;

    kfw::ValCh<bool> isSingleClickToOpenPalette() const override;
    void setIsSingleClickToOpenPalette(bool isSingleClick) override;

    kfw::ValCh<bool> isPaletteDragEnabled() const override;
    void setIsPaletteDragEnabled(bool enabled) override;

    QColor elementsBackgroundColor() const override;
    QColor elementsColor() const override;
    QColor gridColor() const override;
    QColor accentColor() const override;
    kfw::async::Notification colorsChanged() const override;

    kfw::io::path_t keySignaturesDirPath() const override;
    kfw::io::path_t timeSignaturesDirPath() const override;

    bool useFactorySettings() const override;
    bool enableExperimental() const override;

    kfw::ValCh<PaletteConfig> paletteConfig(const QString& paletteId) const override;
    void setPaletteConfig(const QString& paletteId, const PaletteConfig& config) override;

    kfw::ValCh<PaletteCellConfig> paletteCellConfig(const QString& cellId) const override;
    void setPaletteCellConfig(const QString& cellId, const PaletteCellConfig& config) override;

private:
    QColor themeColor(kfw::ui::ThemeStyleKey key) const;

    kfw::ValCh<bool> m_isSinglePalette;
    kfw::ValCh<bool> m_isSingleClickToOpenPalette;
    kfw::ValCh<bool> m_isPaletteDragEnabled;

    mutable QHash<QString, kfw::ValCh<PaletteConfig> > m_paletteConfigs;
    mutable QHash<QString, kfw::ValCh<PaletteCellConfig> > m_paletteCellsConfigs;
};
}

#endif // KN_PALETTE_PALETTECONFIGURATION_H
