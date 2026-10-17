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

#include "ui_editstaff.h"
#include "engraving/dom/stafftype.h"

#include "global/async/asyncable.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "global/iinteractive.h"
#include "iselectinstrumentscenario.h"

namespace kn::notation {
class EditStaffType;

class EditStaff : public QDialog, private Ui::EditStaffBase, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
    kfw::ContextInject<ISelectInstrumentsScenario> selectInstrumentsScenario = { this };

public:
    EditStaff(QWidget* parent = nullptr);

private:
    void showEvent(QShowEvent*) override;
    void hideEvent(QHideEvent*) override;
    void apply();
    void setStaff(kn::engraving::Staff*, const kn::engraving::Fraction& tick);
    void updateInterval(const kn::engraving::Interval&);
    void updateStaffType(const kn::engraving::StaffType& staffType);
    void updateInstrument();
    void updateNextPreviousButtons();

private slots:
    void bboxClicked(QAbstractButton* button);
    void editStringDataClicked();
    void showReplaceInstrumentDialog();
    void showStaffTypeDialog();
    void minPitchAClicked();
    void maxPitchAClicked();
    void minPitchPClicked();
    void maxPitchPClicked();
    void lineDistanceChanged();
    void numOfLinesChanged();
    void showClefChanged();
    void showTimeSigChanged();
    void showBarlinesChanged();
    void gotoNextStaff();
    void gotoPreviousStaff();
    void invisibleChanged();
    void colorChanged();
    void magChanged(double newValue);
    void transpositionChanged();

signals:
    void instrumentChanged();

private:
    INotationPtr notation() const;
    IMasterNotationPtr masterNotation() const;
    INotationPartsPtr notationParts() const;
    INotationPartsPtr masterNotationParts() const;

    void initStaff();

    Staff* staff(int staffIndex) const;
    Instrument instrument() const;

    void applyStaffProperties();
    void applyPartProperties();

    QString midiCodeToStr(int midiCode);

    kn::engraving::Staff* m_staff = nullptr;
    kn::engraving::Staff* m_orgStaff = nullptr;
    Instrument m_instrument;
    Instrument m_orgInstrument;
    InstrumentKey m_instrumentKey;
    int m_minPitchA, m_maxPitchA, m_minPitchP, m_maxPitchP;
    kn::engraving::Fraction m_tickStart, m_tickEnd;

    EditStaffType* editStaffTypeDialog = nullptr;
};
}
