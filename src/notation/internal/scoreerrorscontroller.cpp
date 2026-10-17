/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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
#include "scoreerrorscontroller.h"

#include "engraving/dom/engravingglobals.h"

using namespace kfw;
using namespace kn::engraving;
using namespace kn::notation;

ScoreErrorsController::ScoreErrorsController(const kfw::modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx)
{
}

void ScoreErrorsController::checkAndShowMScoreError()
{
    TRACEFUNC;

    MsError err = EngravingGlobals::_error;
    if (err == MsError::MS_NO_ERROR || err == m_currentDialogError) {
        return;
    }

    EngravingGlobals::setError(MsError::MS_NO_ERROR); // reset

    if (!configuration()->needToShowMScoreError(EngravingGlobals::errorToString(err))) {
        return;
    }

    std::string title;
    std::string message;

    switch (err) {
    case MsError::MS_NO_ERROR:
        return;
    case MsError::NO_NOTE_SELECTED:
        title = kfw::trc("notation", "No note selected");
        message = kfw::trc("notation", "Please select a note and retry");
        break;
    case MsError::NO_CHORD_REST_SELECTED:
        title = kfw::trc("notation", "No chord/rest selected");
        message = kfw::trc("notation", "Please select a chord or rest and retry");
        break;
    case MsError::NO_LYRICS_SELECTED:
        title = kfw::trc("notation", "No note or lyrics selected");
        message = kfw::trc("notation", "Please select a note or lyrics and retry");
        break;
    case MsError::NO_NOTE_REST_SELECTED:
        title = kfw::trc("notation", "No note or rest selected");
        message = kfw::trc("notation", "Please select a note or rest and retry");
        break;
    case MsError::NO_FLIPPABLE_SELECTED:
        title = kfw::trc("notation", "No flippable element selected");
        message = kfw::trc("notation", "Please select an element that can be flipped and retry");
        break;
    case MsError::NO_MEASURE_SELECTED:
        title = kfw::trc("notation", "No measure selected");
        message = kfw::trc("notation", "Please select a measure and retry");
        break;
    case MsError::NO_STAFF_SELECTED:
        title = kfw::trc("notation", "No staff selected");
        message = kfw::trc("notation", "Please select one or more staves and retry");
        break;
    case MsError::NO_NOTE_FIGUREDBASS_SELECTED:
        title = kfw::trc("notation", "No note or figured bass selected");
        message = kfw::trc("notation", "Please select a note or figured bass and retry");
        break;
    case MsError::NO_NOTE_REST_HARMONY_SELECTED:
        title = kfw::trc("notation", "No note or rest or chord symbol selected");
        message = kfw::trc("notation", "Please select a note or rest or chord symbol and retry");
        break;
    case MsError::CANNOT_INSERT_TUPLET:
        title = kfw::trc("notation", "Cannot insert chord/rest in tuplet");
        break;
    case MsError::CANNOT_SPLIT_TUPLET:
        title = kfw::trc("notation", "Cannot split tuplet");
        break;
    case MsError::CANNOT_SPLIT_MEASURE_FIRST_BEAT:
        title = kfw::trc("notation", "Cannot split measure at the first beat");
        break;
    case MsError::CANNOT_SPLIT_MEASURE_TUPLET:
        title = kfw::trc("notation", "Cannot split measure here");
        message = kfw::trc("notation", "Cannot split tuplet");
        break;
    case MsError::INSUFFICIENT_MEASURES:
        title = kfw::trc("notation", "Cannot create measure repeat here");
        message = kfw::trc("notation", "Insufficient or unequal measures");
        break;
    case MsError::CANNOT_SPLIT_MEASURE_REPEAT:
        title = kfw::trc("notation", "Cannot split measure repeat");
        message = kfw::trc("notation", "Please select a note or rest and retry");
        break;
    case MsError::CANNOT_SPLIT_MEASURE_TOO_SHORT:
        title = kfw::trc("notation", "This measure is too short to be split");
        break;
    case MsError::CANNOT_REMOVE_TIME_TUPLET:
        title = kfw::trc("notation", "Cannot remove time from tuplet");
        message = kfw::trc("notation", "Please select the complete tuplet and retry");
        break;
    case MsError::CANNOT_REMOVE_TIME_MEASURE_REPEAT:
        title = kfw::trc("notation", "Cannot remove time from measure repeat");
        message = kfw::trc("notation", "Please select the complete measure repeat and retry");
        break;
    case MsError::NO_DEST:
        title = kfw::trc("notation", "No destination to paste");
        break;
    case MsError::SOURCE_PARTIAL_TUPLET:
        title = kfw::trc("notation", "This selection cannot be copied");
        message = kfw::trc("notation", "Please select all notes that are part of this tuplet and try again.");
        break;
    case MsError::DEST_TUPLET:
        title = kfw::trc("notation", "Cannot paste into tuplet");
        break;
    case MsError::TUPLET_CROSSES_BAR:
        title = kfw::trc("notation", "Tuplet cannot cross barlines");
        break;
    case MsError::DEST_LOCAL_TIME_SIGNATURE:
        title = kfw::trc("notation", "Cannot paste between different local time signatures");
        break;
    case MsError::SOURCE_PARTIAL_TREMOLO:
        title = kfw::trc("notation", "This selection cannot be copied");
        message = kfw::trc("notation", "Please select all notes that are part of this tremolo and try again.");
        break;
    case MsError::DEST_TREMOLO:
        title = kfw::trc("notation", "Cannot paste in tremolo");
        break;
    case MsError::NO_MIME:
        title = kfw::trc("notation", "Nothing to paste");
        break;
    case MsError::DEST_NO_CR:
        title = kfw::trc("notation", "Destination is not a chord or rest");
        break;
    case MsError::CANNOT_CHANGE_LOCAL_TIMESIG_MEASURE_NOT_EMPTY:
        title = kfw::trc("notation", "Cannot change local time signature");
        message = kfw::trc("notation", "Measure is not empty");
        break;
    case MsError::CANNOT_CHANGE_LOCAL_TIMESIG_HAS_EXCERPTS:
        title = kfw::trc("notation", "Cannot change local time signature");
        message = kfw::trc("notation", "This score already has part scores. Changing local time "
                                        "signatures while part scores are present is not yet supported.");
        break;
    case MsError::CORRUPTED_MEASURE:
        title = kfw::trc("notation", "Cannot change time signature in front of a corrupted measure");
        break;
    case MsError::CANNOT_REMOVE_KEY_SIG:
        title = kfw::trc("notation", "This key signature cannot be deleted");
        message = kfw::trc("notation", "Please replace it with a key signature from the palettes instead.");
        break;
    case MsError::CANNOT_JOIN_MEASURE_STAFFTYPE_CHANGE:
        title = kfw::trc("notation", "These measures cannot be joined");
        message = kfw::trc("notation", "Please remove the staff type change and retry.");
        break;
    case MsError::CANNOT_REPEAT_SELECTION:
        title = kfw::trc("notation", "Can’t repeat this selection");
        message = kfw::trc("notation", "Make a list selection of notes or rests on the same beat or any range selection and retry.");
        break;
    case MsError::TRANSPOSE_NO_FRET_DIAGRAM:
        title = kfw::trc("notation", "Some fretboard diagrams could not be transposed");
        message = kfw::trc("notation",
                            "Fretboard diagrams that could not be transposed have been left blank. You can undo this action if required.");
        break;
    case MsError::CANNOT_EXPLODE_IMPLODE_LOCAL_TIMESIG:
        title = kfw::trc("notation", "Can’t explode or implode");
        message = kfw::trc("notation", "Cannot explode or implode between different local time signatures");
        break;
    }

    m_currentDialogError = err;

    interactive()->info(title, message, {}, 0,
                        IInteractive::Option::WithIcon | IInteractive::Option::WithDontShowAgainCheckBox)
    .onResolve(this, [this, err](const IInteractive::Result& res) {
        if (!res.showAgain()) {
            configuration()->setNeedToShowMScoreError(EngravingGlobals::errorToString(err), false);
        }
        m_currentDialogError = MsError::MS_NO_ERROR;
    });
}
