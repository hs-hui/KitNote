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
#include "midimodule.h"

#include "modularity/ioc.h"

#include "project/inotationreadersregister.h"
#include "internal/notationmidireader.h"
#include "project/inotationwritersregister.h"
#include "internal/notationmidiwriter.h"
#include "internal/drumtabwriter.h"

#include "internal/midiconfiguration.h"

#include "log.h"

using namespace kfw::modularity;
using namespace kn::iex::midi;
using namespace kn::project;

std::string MidiModule::moduleName() const
{
    return "iex_midi";
}

void MidiModule::registerExports()
{
    m_configuration = std::make_shared<MidiConfiguration>();

    ioc()->registerExport<IMidiImportExportConfiguration>(moduleName(), m_configuration);
}

void MidiModule::resolveImports()
{
    auto readers = ioc()->resolve<INotationReadersRegister>(moduleName());
    if (readers) {
        readers->reg({ "mid", "midi", "kar" }, std::make_shared<NotationMidiReader>());
    }

    auto writers = ioc()->resolve<INotationWritersRegister>(moduleName());
    if (writers) {
        writers->reg({ "mid", "midi", "kar" }, std::make_shared<NotationMidiWriter>(iocContext()));
        writers->reg({ "txt" }, std::make_shared<DrumTabWriter>());     // KitNote: the score as a text drum tab
    }
}

void MidiModule::onInit(const kfw::IApplication::RunMode&)
{
    m_configuration->init();
}
