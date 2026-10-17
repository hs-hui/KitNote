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

#include "mscnotationwriter.h"

#include "io/buffer.h"
#include "io/file.h"

#include "engraving/engravingproject.h"
#include "engraving/dom/masterscore.h"

#include "log.h"

using namespace kfw;
using namespace kfw::io;
using namespace kn::engraving;
using namespace kn::notation;
using namespace kn::project;

MscNotationWriter::MscNotationWriter(engraving::MscIoMode mode)
    : m_mode(mode)
{
}

std::vector<INotationWriter::UnitType> MscNotationWriter::supportedUnitTypes() const
{
    return { UnitType::MULTI_PART };
}

bool MscNotationWriter::supportsUnitType(UnitType unitType) const
{
    std::vector<UnitType> unitTypes = supportedUnitTypes();
    return std::find(unitTypes.cbegin(), unitTypes.cend(), unitType) != unitTypes.cend();
}

Ret MscNotationWriter::write(INotationPtr notation, io::IODevice& device, const Options&)
{
    IF_ASSERT_FAILED(notation) {
        return make_ret(Ret::Code::UnknownError);
    }

    kn::engraving::Score* score = notation->elements()->msScore();

    IF_ASSERT_FAILED(score) {
        return make_ret(Ret::Code::UnknownError);
    }

    MscWriter::Params params;
    params.mode = m_mode;
    params.filePath = device.meta("file_path");
    if (m_mode == MscIoMode::Dir) {
        IF_ASSERT_FAILED(!params.filePath.empty()) {
            return make_ret(Ret::Code::InternalError);
        }

        if (File::exists(params.filePath)) {
            File::remove(params.filePath);
        }
    } else {
        params.device = &device;
    }

    MscWriter msczWriter(params);
    if (!msczWriter.open()) {
        LOGE() << "MscWriter is not opened";
        return Ret(Ret::Code::UnknownError);
    }

    notation->elements()->msScore()->masterScore()->project().lock()->writeMscz(msczWriter, true);

    msczWriter.close();

    if (msczWriter.hasError()) {
        LOGE() << "MscWriter has error";
        return Ret(Ret::Code::UnknownError);
    }

    return Ret(Ret::Code::Ok);
}

Ret MscNotationWriter::writeList(const INotationPtrList&, io::IODevice&, const Options&)
{
    NOT_SUPPORTED;
    return Ret(Ret::Code::NotSupported);
}
