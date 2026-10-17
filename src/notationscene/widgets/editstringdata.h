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

#include "ui_editstringdata.h"
#include "engraving/dom/stringdata.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"

namespace kn::notation {
//---------------------------------------------------------
//   EditStringData
//---------------------------------------------------------

class EditStringData : public QDialog, private Ui::EditStringDataBase, public kfw::Contextable
{
    Q_OBJECT

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    EditStringData(QWidget* parent = nullptr, const std::vector<engraving::instrString>& strings = {}, int frets = 0);

    std::vector<kn::engraving::instrString> strings() const;
    int frets() const;

protected:
    QString midiCodeToStr(int midiCode);

private slots:
    void accept() override;
    void deleteStringClicked();
    void editStringClicked();
    void listItemClicked(QTableWidgetItem* item);
    void newStringClicked();

private:
    void init();
    void initStringsData();

    void showEvent(QShowEvent*) override;
    void hideEvent(QHideEvent*) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

    QString openColumnAccessibleText(const QTableWidgetItem* item) const;

    INotationSelectionPtr currentNotationSelection() const;

    int _frets = -1;
    bool _modified = false;
    std::vector<kn::engraving::instrString> _strings;           // pointer to original string list
    std::vector<kn::engraving::instrString> _stringsLoc;         // local working copy of string list

    bool m_updateOnExit = false;
    Instrument* m_instrument = nullptr;
};
}
