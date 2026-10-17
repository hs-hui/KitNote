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

#ifndef __REALIZEHARMONYDIALOG_H__
#define __REALIZEHARMONYDIALOG_H__

#include "ui_realizeharmonydialog.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"

namespace kn::engraving {
class Harmony;
}

namespace kn::notation {
class RealizeHarmonyDialog : public QDialog, Ui::RealizeHarmonyDialogBase, public kfw::Contextable
{
    Q_OBJECT

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    RealizeHarmonyDialog(QWidget* parent = nullptr);

private slots:
    INotationInteractionPtr interaction() const;

    void accept() override;

    void toggleChordTable();

    void setChordList(const QList<kn::engraving::Harmony*>& hlist);
};
}

#endif // REALIZEHARMONYDIALOG_H
