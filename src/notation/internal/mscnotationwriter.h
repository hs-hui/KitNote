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

#ifndef KN_NOTATION_MSCNOTATIONWRITER_H
#define KN_NOTATION_MSCNOTATIONWRITER_H

#include "project/inotationwriter.h"

#include "engraving/infrastructure/mscio.h"

namespace kn::notation {
class MscNotationWriter : public project::INotationWriter
{
public:

    explicit MscNotationWriter(engraving::MscIoMode mode);

    std::vector<UnitType> supportedUnitTypes() const override;
    bool supportsUnitType(UnitType unitType) const override;

    kfw::Ret write(notation::INotationPtr notation, kfw::io::IODevice& device, const Options& options = Options()) override;
    kfw::Ret writeList(const INotationPtrList& notations, kfw::io::IODevice& device, const Options& options = Options()) override;

private:
    engraving::MscIoMode m_mode;
};
}

#endif // KN_NOTATION_MSCNOTATIONWRITER_H
