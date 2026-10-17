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
#ifndef KN_IMPORTEXPORT_IIMPORTEXPORTCONFIGURATION_H
#define KN_IMPORTEXPORT_IIMPORTEXPORTCONFIGURATION_H

#include <string>

#include "async/channel.h"
#include "modularity/imoduleinterface.h"

namespace kn::iex::ove {
class IOveConfiguration : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IOveConfiguration)

public:
    virtual ~IOveConfiguration() = default;

    virtual std::string importOvertureCharset() const = 0;
    virtual void setImportOvertureCharset(const std::string& charset) = 0;
    virtual kfw::async::Channel<std::string> importOvertureCharsetChanged() const = 0;
};
}

#endif // KN_IMPORTEXPORT_IIMPORTEXPORTCONFIGURATION_H
