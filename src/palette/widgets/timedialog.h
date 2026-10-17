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

#ifndef __TIMEDIALOG_H__
#define __TIMEDIALOG_H__

#include "ui_timedialog.h"

#include "modularity/ioc.h"
#include "ipaletteconfiguration.h"
#include "internal/ipaletteprovider.h"
#include "engraving/rendering/isinglerenderer.h"

namespace kn::palette {
class PaletteWidget;
class PaletteScrollArea;

class TimeDialog : public QWidget, Ui::TimeDialogBase, public kfw::Contextable
{
    Q_OBJECT

    Q_PROPERTY(bool showTimePalette READ showTimePalette WRITE setShowTimePalette)

    kfw::GlobalInject<IPaletteConfiguration> configuration;
    kfw::ContextInject<IPaletteProvider> paletteProvider = { this };
    kfw::ContextInject<engraving::rendering::ISingleRenderer> engravingRender = { this };

public:
    TimeDialog(QWidget* parent = 0);

    bool dirty() const;
    bool showTimePalette() const;
    void save();

private slots:
    void addClicked();
    void zChanged();
    void nChanged(int);
    void paletteChanged(int idx);
    void textChanged();
    void setDirty();
    void setShowTimePalette(bool val);

private:
    int denominator() const;
    int denominator2Idx(int) const;

    PaletteScrollArea* _timePalette = nullptr;
    PaletteWidget* sp = nullptr;
    bool _dirty = false;
};
}

#endif
