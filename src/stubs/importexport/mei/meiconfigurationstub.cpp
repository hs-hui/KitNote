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
#include "meiconfigurationstub.h"

#include "log.h"
#include "thirdparty/kors_logger/src/log_base.h"

using namespace kfw;
using namespace kn::iex::mei;

bool MeiConfigurationStub::meiImportLayout() const
{
    return false;
}

void MeiConfigurationStub::setMeiImportLayout(bool)
{
    NOT_IMPLEMENTED;
}

async::Channel<bool> MeiConfigurationStub::meiImportLayoutChanged() const
{
    return {};
}

bool MeiConfigurationStub::meiExportLayout() const
{
    return false;
}

void MeiConfigurationStub::setMeiExportLayout(bool)
{
    NOT_IMPLEMENTED;
}

bool MeiConfigurationStub::meiUseKitNoteIds() const
{
    return false;
}

void MeiConfigurationStub::setMeiUseKitNoteIds(bool)
{
    NOT_IMPLEMENTED;
}
