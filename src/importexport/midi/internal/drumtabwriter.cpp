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

#include "drumtabwriter.h"

#include "notation/inotation.h"
#include "notation/inotationinteraction.h"

#include "log.h"

using namespace kn::iex::midi;
using namespace kfw;
using namespace kn::project;
using namespace kn::notation;

std::vector<INotationWriter::UnitType> DrumTabWriter::supportedUnitTypes() const
{
    return { UnitType::PER_PART };
}

bool DrumTabWriter::supportsUnitType(UnitType unitType) const
{
    return unitType == UnitType::PER_PART;
}

Ret DrumTabWriter::write(INotationPtr notation, io::IODevice& destinationDevice, const Options&)
{
    IF_ASSERT_FAILED(notation) {
        return make_ret(Ret::Code::UnknownError);
    }

    // The whole score: a selection the user happens to have is for "Copy as drum tab", not for a file.
    const INotationInteractionPtr interaction = notation->interaction();
    const QString text = interaction->drumTabText(true);

    // A score without drum notes gives an empty file rather than an error: there is nothing wrong with it.
    const QByteArray utf8 = text.toUtf8();
    destinationDevice.write(ByteArray::fromQByteArrayNoCopy(utf8));

    return make_ret(Ret::Code::Ok);
}

Ret DrumTabWriter::writeList(const INotationPtrList&, io::IODevice&, const Options&)
{
    NOT_SUPPORTED;
    return Ret(Ret::Code::NotSupported);
}
