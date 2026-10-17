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

#ifndef KN_PALETTE_DRUMSETPALETTE_H
#define KN_PALETTE_DRUMSETPALETTE_H

#include "palettewidget.h"
#include "notation/inotation.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"
#include "playback/iplaybackcontroller.h"
#include "engraving/rendering/isinglerenderer.h"

namespace kn::engraving {
class Drumset;
}

namespace kn::palette {
class DrumsetPalette : public PaletteScrollArea, public kfw::Contextable
{
    Q_OBJECT

    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<playback::IPlaybackController> playback = { this };
    kfw::ContextInject<engraving::rendering::ISingleRenderer> engravingRenderer = { this };

public:
    explicit DrumsetPalette(QWidget* parent = nullptr);

    void setNotation(kn::notation::INotationPtr notation);
    void updateDrumset();
    bool handleEvent(QEvent* event);

    kfw::async::Channel<QString> pitchNameChanged() const;

    PaletteWidget* paletteWidget() const;

private slots:
    void drumNoteClicked(int val);

private:
    void clear();

    void changeEvent(QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

    int selectedDrumNote();
    void retranslate();

    void previewSound(const kn::engraving::Chord* chord, bool newChordSelected, const notation::NoteInputState& inputState);

    kn::notation::INotationNoteInputPtr noteInput() const;

    PaletteWidget* m_drumPalette = nullptr;
    const kn::engraving::Drumset* m_drumset = nullptr;
    kn::notation::INotationPtr m_notation;
    kfw::async::Channel<QString> m_pitchNameChanged;
};
}

#endif // KN_PALETTE_DRUMSETPALETTE_H
