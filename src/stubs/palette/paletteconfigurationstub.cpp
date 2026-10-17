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
#include "paletteconfigurationstub.h"

using namespace kn::palette;
using namespace kfw;

double PaletteConfigurationStub::paletteSpatium() const
{
    return 0.f;
}

double PaletteConfigurationStub::paletteScaling() const
{
    return 1.f;
}

void PaletteConfigurationStub::setPaletteScaling(double)
{
}

ValCh<bool> PaletteConfigurationStub::isSinglePalette() const
{
    return ValCh<bool>();
}

void PaletteConfigurationStub::setIsSinglePalette(bool)
{
}

ValCh<bool> PaletteConfigurationStub::isSingleClickToOpenPalette() const
{
    return ValCh<bool>();
}

void PaletteConfigurationStub::setIsSingleClickToOpenPalette(bool)
{
}

ValCh<bool> PaletteConfigurationStub::isPaletteDragEnabled() const
{
    return ValCh<bool>();
}

void PaletteConfigurationStub::setIsPaletteDragEnabled(bool)
{
}

QColor PaletteConfigurationStub::elementsBackgroundColor() const
{
    return QColor();
}

QColor PaletteConfigurationStub::elementsColor() const
{
    return QColor();
}

QColor PaletteConfigurationStub::gridColor() const
{
    return QColor();
}

QColor PaletteConfigurationStub::accentColor() const
{
    return QColor();
}

kfw::async::Notification PaletteConfigurationStub::colorsChanged() const
{
    return kfw::async::Notification();
}

kfw::io::path_t PaletteConfigurationStub::keySignaturesDirPath() const
{
    return kfw::io::path_t();
}

kfw::io::path_t PaletteConfigurationStub::timeSignaturesDirPath() const
{
    return kfw::io::path_t();
}

bool PaletteConfigurationStub::useFactorySettings() const
{
    return false;
}

bool PaletteConfigurationStub::enableExperimental() const
{
    return false;
}

ValCh<IPaletteConfiguration::PaletteConfig> PaletteConfigurationStub::paletteConfig(const QString&) const
{
    return ValCh<IPaletteConfiguration::PaletteConfig>();
}

void PaletteConfigurationStub::setPaletteConfig(const QString&, const IPaletteConfiguration::PaletteConfig&)
{
}

ValCh<IPaletteConfiguration::PaletteCellConfig> PaletteConfigurationStub::paletteCellConfig(const QString&) const
{
    return ValCh<IPaletteConfiguration::PaletteCellConfig>();
}

void PaletteConfigurationStub::setPaletteCellConfig(const QString&, const IPaletteConfiguration::PaletteCellConfig&)
{
}
