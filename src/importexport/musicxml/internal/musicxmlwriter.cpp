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

#include "musicxmlwriter.h"

#include "engraving/dom/score.h"
#include "export/exportmusicxml.h"

#include "log.h"

using namespace kn::iex::musicxml;
using namespace kn::project;
using namespace kfw;
using namespace kfw::io;

std::vector<INotationWriter::UnitType> MusicXmlWriter::supportedUnitTypes() const
{
    return { UnitType::PER_PART };
}

bool MusicXmlWriter::supportsUnitType(UnitType unitType) const
{
    std::vector<UnitType> unitTypes = supportedUnitTypes();
    return std::find(unitTypes.cbegin(), unitTypes.cend(), unitType) != unitTypes.cend();
}

Ret MusicXmlWriter::write(notation::INotationPtr notation, io::IODevice& destinationDevice, const Options&)
{
    IF_ASSERT_FAILED(notation) {
        return make_ret(Ret::Code::UnknownError);
    }
    kn::engraving::Score* score = notation->elements()->msScore();
    IF_ASSERT_FAILED(score) {
        return make_ret(Ret::Code::UnknownError);
    }

    Ret ret = saveXml(score, &destinationDevice);

    return ret;
}

Ret MusicXmlWriter::writeList(const notation::INotationPtrList&, io::IODevice&, const Options&)
{
    NOT_SUPPORTED;
    return Ret(Ret::Code::NotSupported);
}
