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

#include "ui_timesignaturepropertiesdialog.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "ui/iuiconfiguration.h"

namespace kn::engraving {
class TimeSig;
}

namespace kn::palette {
class TimeSignaturePropertiesDialog : public QDialog, public Ui::TimeSigProperties, public kfw::Contextable
{
    Q_OBJECT

    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::ContextInject<kn::context::IGlobalContext> globalContext = { this };

public:
    TimeSignaturePropertiesDialog(QWidget* parent = nullptr);
    ~TimeSignaturePropertiesDialog() override;

private slots:
    void accept() override;

private:
    void showEvent(QShowEvent*) override;
    void hideEvent(QHideEvent*) override;

    kn::notation::INotationPtr notation() const;

    engraving::TimeSig* m_originTimeSig = nullptr;
    engraving::TimeSig* m_editedTimeSig = nullptr;
};
}
