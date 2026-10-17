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
#ifndef KN_NOTATION_ALIGNSELECT_H
#define KN_NOTATION_ALIGNSELECT_H

#include "modularity/ioc.h"
#include "ui_align_select.h"
#include "engraving/types/types.h"

namespace kn::notation {
class AlignSelect : public QWidget, public Ui::AlignSelect, public kfw::Contextable
{
    Q_OBJECT

public:
    AlignSelect(QWidget* parent);
    kn::engraving::Align align() const;
    kn::engraving::AlignH position() const;

    void setAlign(kn::engraving::Align);
    void setPosition(kn::engraving::AlignH);

signals:
    void alignChanged(kn::engraving::Align);
    void positionChanged(kn::engraving::AlignH);

private:
    QButtonGroup* horizontalAlignButtons;
    QButtonGroup* verticalAlignButtons;
    QButtonGroup* positionButtons;

    void blockAlign(bool val);
    void blockPosition(bool val);

private slots:
    void _alignChanged();
    void _positionChanged();
};
}

#endif // KN_NOTATION_ALIGNSELECT_H
