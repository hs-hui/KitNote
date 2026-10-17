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
#pragma once

#include "ui_specialcharactersdialog.h"

#include "draw/types/font.h"
#include "uicomponents/view/topleveldialog.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"

class QListWidget;

namespace kn::engraving {
class TextBase;
}

namespace kn::palette {
class PaletteWidget;

class SpecialCharactersDialog : public kfw::uicomponents::TopLevelDialog, public Ui::SpecialCharactersDialog
{
    Q_OBJECT

    kfw::ContextInject<kn::context::IGlobalContext> globalContext = { this };

public:
    SpecialCharactersDialog(QWidget* parent = nullptr);

private slots:
    void populateSmufl();
    void populateUnicode();

private:
    void showEvent(QShowEvent*) override;
    void hideEvent(QHideEvent*) override;

    void setFont(const kfw::draw::Font& font);
    void populateCommon();

    kfw::draw::Font m_font;
    PaletteWidget* m_pCommon = nullptr;
    PaletteWidget* m_pSmufl = nullptr;
    PaletteWidget* m_pUnicode = nullptr;
    QListWidget* m_lws = nullptr;
    QListWidget* m_lwu = nullptr;
};
}
