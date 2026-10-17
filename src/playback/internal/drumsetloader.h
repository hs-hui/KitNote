/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include "async/asyncable.h"

#include "audio/common/audiotypes.h"

#include "notation/iinstrumentsrepository.h"
#include "notation/inotation.h"

#include "engraving/dom/drumset.h"

namespace kn::playback {
class DrumsetLoader : public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::ContextInject<notation::IInstrumentsRepository> instrumentsRepository = { this };

public:

    DrumsetLoader(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    void loadDrumset(notation::INotationPtr notation, const kn::engraving::InstrumentTrackId& trackId,
                     const kfw::audio::AudioResourceMeta& resourceMeta);

private:
    void replaceDrumset(notation::INotationPtr notation, const kn::engraving::InstrumentTrackId& trackId,
                        const kn::engraving::Drumset& drumset);
};
}
