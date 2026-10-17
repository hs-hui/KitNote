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
#ifndef KN_LOGREMOVER_H
#define KN_LOGREMOVER_H

#include "kfw_framework_config.h"

#ifdef KFW_ENABLE_UNIT_TESTS
#include <gtest/gtest_prod.h>
#endif

#include "types/string.h"
#include "types/datetime.h"
#include "io/path.h"

namespace kfw {
class LogRemover
{
public:

    static void removeLogs(const io::path_t& logsDir, int olderThanDays, const String& pattern);

private:

#ifdef KFW_ENABLE_UNIT_TESTS
    FRIEND_TEST(Global_LogRemoverTests, ParseDate);
#endif

    static void scanDir(const io::path_t& logsDir, io::paths_t& files);
    static Date parseDate(const String& fileName);
    static void removeFiles(const io::paths_t& files);
};
}

#endif // KN_LOGREMOVER_H
