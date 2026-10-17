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

#include <QDialog>

#include "ui_measureproperties.h"
#include "engraving/dom/sig.h"
#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "notation/inotation.h"

namespace kn::engraving {
class Measure;
}

namespace kn::notation {
class MeasurePropertiesDialog : public QDialog, private Ui::MeasurePropertiesBase, public kfw::Contextable
{
    Q_OBJECT

    kfw::ContextInject<kn::context::IGlobalContext> context = { this };

public:
    MeasurePropertiesDialog(QWidget* parent = nullptr);

private slots:
    void bboxClicked(QAbstractButton* button);
    void gotoNextMeasure();
    void gotoPreviousMeasure();

private:
    bool eventFilter(QObject* obj, QEvent* event) override;

    void initMeasure();

    void apply();
    kn::engraving::Fraction len() const;
    bool isIrregular() const;
    int repeatCount() const;
    bool visible(int staffIdx);
    bool stemless(int staffIdx);
    void setMeasure(kn::engraving::Measure* measure);

    void showEvent(QShowEvent*) override;
    void hideEvent(QHideEvent*) override;

    kn::engraving::Measure* m_measure = nullptr;
    int m_measureIndex = -1;

    std::shared_ptr<INotation> m_notation;
};
}
