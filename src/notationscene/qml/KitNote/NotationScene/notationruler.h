/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include "modularity/ioc.h"

#include "notation/inotationconfiguration.h"
#include "ui/iuiconfiguration.h"

namespace kn::notation {
class NotationRuler : public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;

public:
    NotationRuler(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void paint(kfw::draw::Painter* painter, const NoteInputState& state);

private:
    enum LineType {
        CurrentPosition,
        MainBeat,
        Subdivision,
    };

    static LineType lineType(int lineTicks, int inputTicks, size_t lineIdx);

    void paintLine(kfw::draw::Painter* painter, LineType type, const kfw::PointF& point, double spatium, voice_idx_t voiceIdx);
};
}
