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

#include "realizeharmonydialog.h"

#include "engraving/dom/harmony.h"
#include "engraving/dom/staff.h"

#include "translation.h"

using namespace kn::notation;

RealizeHarmonyDialog::RealizeHarmonyDialog(QWidget* parent)
    : QDialog(parent), kfw::Contextable(kfw::iocCtxForQWidget(this))
{
    setObjectName("RealizeHarmonyDialog");
    setupUi(this);

    chordTable->setVisible(false);
    connect(showButton, SIGNAL(clicked()), SLOT(toggleChordTable()));

    voicingSelect->setLiteral(true);

    //make the chord list uneditable
    chordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    INotationInteractionPtr interaction = this->interaction();
    if (!interaction) {
        return;
    }

    const std::vector<kn::engraving::EngravingItem*>& selectedElements = interaction->selection()->elements();
    QList<kn::engraving::Harmony*> selectedHarmonyList;

    for (kn::engraving::EngravingItem* element : selectedElements) {
        if (element->isHarmony()) {
            selectedHarmonyList << kn::engraving::toHarmony(element);
        }
    }

    setChordList(selectedHarmonyList);
}

INotationInteractionPtr RealizeHarmonyDialog::interaction() const
{
    const INotationPtr notation = globalContext()->currentNotation();
    return notation ? notation->interaction() : nullptr;
}

void RealizeHarmonyDialog::accept()
{
    INotationInteractionPtr interaction = this->interaction();
    if (!interaction) {
        return;
    }

    bool optionsOverride = optionsBox->isChecked();
    Voicing voicing = optionsOverride ? Voicing(voicingSelect->getVoicing())
                      : Voicing::INVALID;
    HarmonyDurationType durationType = optionsOverride ? HarmonyDurationType(voicingSelect->getDuration())
                                       : HarmonyDurationType::INVALID;

    interaction->realizeSelectedChordSymbols(voicingSelect->getLiteral(), voicing, durationType);

    QDialog::accept();
}

void RealizeHarmonyDialog::toggleChordTable()
{
    int visible = chordTable->isVisible();
    chordTable->setVisible(!visible);
    showButton->setText(!visible ? kfw::qtrc("global", "Show less…") : kfw::qtrc("global", "Show more…"));
}

//---------------------------------------------------------
//   setChordList
///   fill the chord list with the list of harmonies
//---------------------------------------------------------
void RealizeHarmonyDialog::setChordList(const QList<Harmony*>& hlist)
{
    static const QStringList header = { "ID", "Name", "Intervals", "Notes" };
    QString s;    //chord label string
    int rows = hlist.size();

    chordTable->setRowCount(rows);
    chordTable->setColumnCount(header.size());
    chordTable->setHorizontalHeaderLabels(header);
    for (int i = 0; i < rows; ++i) {
        Harmony* h = hlist.at(i);
        if (!h->isRealizable()) {
            continue;
        }

        s += h->harmonyName() + u" ";
        QString intervals;
        QString noteNames;
        int rootTpc = h->rootTpc();

        noteNames = tpc2name(rootTpc, kn::engraving::NoteSpellingType::STANDARD, kn::engraving::NoteCaseType::AUTO);
        const kn::engraving::RealizedHarmony::PitchMap& notes = h->getRealizedHarmony().notes();

        for (const auto& [pitch, tpc] : notes) {
            intervals += QString::number((pitch - kn::engraving::tpc2pitch(rootTpc)) % 128 % 12) + " ";
            noteNames += u", " + kn::engraving::tpc2name(tpc, kn::engraving::NoteSpellingType::STANDARD, kn::engraving::NoteCaseType::AUTO);
        }

        chordTable->setItem(i, 0, new QTableWidgetItem(QString::number(h->id())));
        chordTable->setItem(i, 1, new QTableWidgetItem(h->harmonyName()));
        chordTable->setItem(i, 2, new QTableWidgetItem(intervals));
        chordTable->setItem(i, 3, new QTableWidgetItem(noteNames));
    }
    chordLabel->setText(s);

    //set table uneditable again
    chordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}
