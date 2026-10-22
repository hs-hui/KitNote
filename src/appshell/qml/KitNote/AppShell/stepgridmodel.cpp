/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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

#include "stepgridmodel.h"

#include "notation/inotation.h"
#include "notation/inotationinteraction.h"
#include "notation/imasternotation.h"
#include "notation/inotationplayback.h"
#include "notation/notationtypes.h"

using namespace kn::appshell;
using namespace kn::notation;

//! The kind of drum, for the colour of its row (as on the pads of the percussion panel):
//! 0 other, 1 bass drum, 2 snare, 3 hi-hat, 4 cymbal, 5 tom
static int drumFamily(int pitch)
{
    switch (pitch) {        // General MIDI percussion
    case 35: case 36:
        return 1;
    case 37: case 38: case 39: case 40:
        return 2;
    case 42: case 44: case 46:
        return 3;
    case 49: case 51: case 52: case 53: case 55: case 57: case 59:
        return 4;
    case 41: case 43: case 45: case 47: case 48: case 50:
        return 5;
    default:
        return 0;
    }
}

StepGridModel::StepGridModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void StepGridModel::setPlayStep(int step)
{
    if (m_playStep != step) {
        m_playStep = step;
        emit playStepChanged();
    }
}

void StepGridModel::load()
{
    // the playhead: a light on the step that sounds
    playbackController()->currentPlaybackPositionChanged().onReceive(this, [this](kfw::audio::secs_t secs, kfw::midi::tick_t) {
        const IMasterNotationPtr notation = globalContext()->currentMasterNotation();
        if (!m_active || !m_available || m_stepCount <= 0 || !notation || !playbackController()->isPlaying()) {
            setPlayStep(-1);
            return;
        }

        // the place in the score (a repeat plays a measure again; measure numbers can be shared)
        const int inMeasure = static_cast<int>(notation->playback()->secToTick(secs)) - m_measureTick;
        if (inMeasure < 0 || inMeasure >= m_measureTicks) {
            setPlayStep(-1);
            return;
        }

        setPlayStep(std::clamp(inMeasure * m_stepCount / m_measureTicks, 0, m_stepCount - 1));
    });

    playbackController()->isPlayingChanged().onNotify(this, [this]() {
        if (!playbackController()->isPlaying()) {
            setPlayStep(-1);
        }
    });

    globalContext()->currentNotationChanged().onNotify(this, [this]() {
        onNotationChanged();
    });

    onNotationChanged();
}

void StepGridModel::onNotationChanged()
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        notation->interaction()->selectionChanged().onNotify(this, [this]() {
            update();
        }, kfw::async::Asyncable::Mode::SetReplace);

        // an edit made elsewhere (the pads, undo) changes what the measure holds
        notation->notationChanged().onNotify(this, [this]() {
            update();
        }, kfw::async::Asyncable::Mode::SetReplace);
    }

    update();
}

void StepGridModel::setActive(bool active)
{
    if (m_active == active) {
        return;
    }

    m_active = active;
    emit activeChanged();

    update();
}

void StepGridModel::update()
{
    if (!m_active) {
        return;
    }

    const INotationPtr notation = globalContext()->currentNotation();
    const DrumStepGrid grid = notation ? notation->interaction()->drumStepGrid() : DrumStepGrid();

    m_available = grid.valid;
    m_fitsGrid = grid.fitsGrid;
    m_repeatSign = grid.repeatSign;
    m_triplets = grid.triplets;
    m_empty = grid.empty;
    m_measureNumber = grid.measureNumber;
    m_measureTick = grid.measureTick;
    m_measureTicks = grid.measureTicks;
    m_stepCount = grid.stepCount;
    m_stepsPerBeat = grid.stepsPerBeat;

    m_rows.clear();
    for (const DrumStepGrid::Row& row : grid.rows) {
        QVariantList steps;     // 0 off, 1 stroke, 2 accented, 3 ghost note
        for (int state : row.states) {
            steps << state;
        }

        QVariantMap item;
        item["pitch"] = row.pitch;
        item["name"] = row.name;
        item["family"] = drumFamily(row.pitch);
        item["steps"] = steps;
        m_rows << item;
    }

    emit gridChanged();
}

void StepGridModel::toggle(int pitch, int step)
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (!notation) {
        return;
    }

    // a drum that is put on a step is heard, as on a drum machine
    const DrumStepGrid before = notation->interaction()->drumStepGrid();
    bool wasOn = false;
    for (const DrumStepGrid::Row& row : before.rows) {
        if (row.pitch == pitch && step >= 0 && step < static_cast<int>(row.steps.size())) {
            wasOn = row.steps[static_cast<size_t>(step)];
        }
    }

    // (only when the step can be written: a measure that does not fit the grid, or a measure-repeat sign,
    // is left as it is, so nothing is heard either)
    const bool editable = before.valid && before.fitsGrid && !before.repeatSign && step >= 0 && step < before.stepCount;
    if (!wasOn && editable && before.firstSegment && playbackController()) {
        playbackController()->playNotes({ NoteVal(pitch) }, before.staffIdx, before.firstSegment);
    }

    notation->interaction()->toggleDrumStep(pitch, step);
}

void StepGridModel::cycleKind(int pitch, int step)
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        notation->interaction()->cycleDrumStepKind(pitch, step);
    }
}

void StepGridModel::setTriplets(bool triplets)
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        notation->interaction()->setDrumStepTriplets(triplets);
        update();
    }
}

void StepGridModel::goToMeasure(bool next)
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (notation) {
        notation->interaction()->selectNextDrumStepMeasure(next);
    }
}
