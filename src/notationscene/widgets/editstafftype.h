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

#include "ui_editstafftype.h"

#include "modularity/ioc.h"
#include "iinteractive.h"

#include "engraving/dom/stafftype.h"

#include "notation/notationtypes.h"

namespace kn::notation {
class EditStaffType : public QDialog, private Ui::EditStaffType, public kfw::Contextable
{
    Q_OBJECT

    kfw::ContextInject<kfw::IInteractive> interactive = { this };

    kn::engraving::StaffType staffType;

    virtual void showEvent(QShowEvent*);
    virtual void hideEvent(QHideEvent*);
    void blockSignals(bool block);

    void setFromDlg();

    void tabStemsCompatibility(bool checked);
    void tabMinimShortCompatibility(bool checked);
    void tabStemThroughCompatibility(bool checked);
    QString createUniqueStaffTypeName(kn::engraving::StaffGroup group);
    void setValues();

private slots:
    void nameEdited(const QString&);
    void durFontNameChanged(int idx);
    void fretFontNameChanged(int idx);
    void textStylesToggled(bool checked);
    void presetsToggled(bool checked);
    void tabStemThroughToggled(bool checked);
    void tabMinimShortToggled(bool checked);
    void tabStemsToggled(bool checked);
    void updatePreview();

    void savePresets();
    void loadPresets();
    void resetToTemplateClicked();
    void addToTemplatesClicked();

public:
    EditStaffType(QWidget* parent = nullptr);
    ~EditStaffType() {}
    void setStaffType(const kn::engraving::StaffType* staffType);
    kn::engraving::StaffType getStaffType() const { return staffType; }

    void setInstrument(const Instrument& instrument);

private:
    kfw::Ret loadScore(kn::engraving::MasterScore* score, const kfw::io::path_t& path);

    void enablePresets();
    void enableTextStyles();

    std::vector<QString> textStyleNames() const;
    engraving::TextStyleType getTextStyle(const QString& name) const;
};
}
