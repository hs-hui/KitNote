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

#ifndef KN_PROJECT_IPROJECTWRITER_H
#define KN_PROJECT_IPROJECTWRITER_H

#include "types/ret.h"
#include "types/val.h"

#include "async/channel.h"
#include "global/progress.h"
#include "inotationproject.h"

namespace kn::project {
class IProjectWriter
{
public:

    virtual ~IProjectWriter() = default;

    enum class UnitType {
        PER_PAGE,
        PER_PART,
        MULTI_PART
    };

    enum class OptionKey {
        UNIT_TYPE,
        PAGE_NUMBER,
        TRANSPARENT_BACKGROUND,
        NOTES_COLORS
    };

    using Options = QMap<OptionKey, kfw::Val>;

    virtual std::vector<UnitType> supportedUnitTypes() const = 0;
    virtual bool supportsUnitType(UnitType unitType) const = 0;

    virtual kfw::Ret write(project::INotationProjectPtr project, QIODevice& device, const Options& options = Options()) = 0;
    virtual kfw::Ret write(project::INotationProjectPtr project, const kfw::io::path_t& filePath, const Options& options = Options()) = 0;
};

using IProjectWriterPtr = std::shared_ptr<IProjectWriter>;
}

#endif // KN_PROJECT_IPROJECTWRITER_H
