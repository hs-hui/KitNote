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

#ifndef KN_PALETTE_NOTEGROUPS_H
#define KN_PALETTE_NOTEGROUPS_H

#include "ui_note_groups.h"

#include "engraving/dom/groups.h"

#include "modularity/ioc.h"
#include "ipaletteconfiguration.h"

namespace kn::engraving {
class Chord;
class Score;
}

namespace kn::palette {
class NoteGroups : public QGroupBox, Ui::NoteGroups, public kfw::Contextable
{
    Q_OBJECT

    kfw::GlobalInject<IPaletteConfiguration> paletteConfiguration;

    std::vector<engraving::Chord*> chords8;
    std::vector<engraving::Chord*> chords16;
    std::vector<engraving::Chord*> chords32;
    engraving::Groups _groups;
    engraving::Fraction _sig;
    QString _z, _n;

    engraving::Score* createScore(int n, engraving::DurationType t, std::vector<engraving::Chord*>* chords);
    void updateBeams(engraving::Chord*, engraving::BeamMode);

private slots:
    void resetClicked();
    void noteClicked(engraving::Note*);
    void beamPropertyDropped(engraving::Chord*, engraving::ActionIcon*);

public:
    NoteGroups(QWidget* parent);
    void setSig(engraving::Fraction sig, const engraving::Groups&, const QString& zText, const QString& nText);
    engraving::Groups groups();
};
}

#endif // KN_PALETTE_NOTEGROUPS_H
