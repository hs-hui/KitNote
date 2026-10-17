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

#pragma once

#include <QObject>
#include <QVariantList>
#include <qqmlintegration.h>

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "playback/iplaybackcontroller.h"

namespace kn::appshell {
//! KitNote: the step grid - the selected measure of a drum staff as a drum machine shows it, a row for
//! every drum and a step for every sixteenth note. A click on a step writes the measure again.
//! (The reading and writing is INotationInteraction::drumStepGrid / toggleDrumStep.)
class StepGridModel : public QObject, public kfw::Contextable, public kfw::async::Asyncable
{
    Q_OBJECT

    //! The grid is on screen. While it is not, the score is not read at every edit and selection.
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

    Q_PROPERTY(bool available READ available NOTIFY gridChanged)
    Q_PROPERTY(bool repeatSign READ repeatSign NOTIFY gridChanged)
    //! The steps are triplet eighths. An empty measure can be switched between the two kinds of grid.
    Q_PROPERTY(bool triplets READ triplets NOTIFY gridChanged)
    Q_PROPERTY(bool empty READ empty NOTIFY gridChanged)
    Q_PROPERTY(bool fitsGrid READ fitsGrid NOTIFY gridChanged)
    Q_PROPERTY(int measureNumber READ measureNumber NOTIFY gridChanged)
    Q_PROPERTY(int stepCount READ stepCount NOTIFY gridChanged)
    Q_PROPERTY(int stepsPerBeat READ stepsPerBeat NOTIFY gridChanged)
    Q_PROPERTY(QVariantList rows READ rows NOTIFY gridChanged)
    //! The step playback is at, while it plays the measure of the grid; -1 otherwise.
    Q_PROPERTY(int playStep READ playStep NOTIFY playStepChanged)

    QML_ELEMENT

    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<playback::IPlaybackController> playbackController = { this };

public:
    explicit StepGridModel(QObject* parent = nullptr);

    bool active() const { return m_active; }
    void setActive(bool active);

    bool available() const { return m_available; }
    bool repeatSign() const { return m_repeatSign; }
    bool triplets() const { return m_triplets; }
    bool empty() const { return m_empty; }
    bool fitsGrid() const { return m_fitsGrid; }
    int measureNumber() const { return m_measureNumber; }
    int stepCount() const { return m_stepCount; }
    int stepsPerBeat() const { return m_stepsPerBeat; }
    QVariantList rows() const { return m_rows; }
    int playStep() const { return m_playStep; }

    Q_INVOKABLE void load();
    Q_INVOKABLE void toggle(int pitch, int step);
    Q_INVOKABLE void cycleKind(int pitch, int step);
    Q_INVOKABLE void goToMeasure(bool next);
    Q_INVOKABLE void setTriplets(bool triplets);

signals:
    void gridChanged();
    void playStepChanged();
    void activeChanged();

private:
    void onNotationChanged();
    void update();
    void setPlayStep(int step);

    int m_playStep = -1;
    int m_measureTick = 0;
    int m_measureTicks = 0;
    bool m_active = false;
    bool m_available = false;
    bool m_repeatSign = false;
    bool m_triplets = false;
    bool m_empty = true;
    bool m_fitsGrid = true;
    int m_measureNumber = 0;
    int m_stepCount = 0;
    int m_stepsPerBeat = 4;
    QVariantList m_rows;
};
}
