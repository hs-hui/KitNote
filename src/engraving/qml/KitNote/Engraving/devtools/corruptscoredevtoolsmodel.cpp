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

#include "corruptscoredevtoolsmodel.h"

using namespace kn::engraving;

CorruptScoreDevToolsModel::CorruptScoreDevToolsModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void CorruptScoreDevToolsModel::corruptOpenScore()
{
    project::INotationProjectPtr project = globalContext()->currentProject();
    if (!project) {
        return;
    }

    LOGW() << "Score corruption on demand!";

    kn::notation::INotationPtr notation = project->masterNotation()->notation();

    //: "Corrupt" is used as a verb here, i.e. "Make the current score corrupted" (for testing purposes).
    notation->undoStack()->prepareChanges(TranslatableString("undoableAction", "Corrupt score"));

    for (engraving::System* system : notation->elements()->msScore()->systems()) {
        for (engraving::MeasureBase* measureBase : system->measures()) {
            if (!measureBase->isMeasure()) {
                continue;
            }

            kn::engraving::Measure* measure = kn::engraving::toMeasure(measureBase);
            kn::engraving::Segment* firstSegment = measure->first(kn::engraving::SegmentType::ChordRest);

            for (kn::engraving::Segment* s = firstSegment; s; s = s->next(kn::engraving::SegmentType::ChordRest)) {
                kn::engraving::EngravingItem* element = s->element(0);
                if (!element) {
                    continue;
                }

                kn::engraving::ChordRest* cr = toChordRest(element);
                cr->undoChangeProperty(kn::engraving::Pid::DURATION, kn::engraving::Fraction::fromString(u"0/4"));
            }
        }
    }

    notation->undoStack()->commitChanges();
}
