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
#ifndef KN_LOGGER_H
#define KN_LOGGER_H

#include "thirdparty/kors_logger/src/logger.h" // IWYU pragma: export
#include "thirdparty/kors_logger/src/logdefdest.h" // IWYU pragma: export
#include "thirdparty/kors_logger/src/log_base.h" // IWYU pragma: export

namespace kfw::logger {
using Logger = kors::logger::Logger;
using Type = kors::logger::Type;
using Level = kors::logger::Level;
using Color = kors::logger::Color;
using LogMsg = kors::logger::LogMsg;
using LogLayout = kors::logger::LogLayout;
using IThreadNameProvider = kors::logger::IThreadNameProvider;
using LogDest = kors::logger::LogDest;
using ConsoleLogDest = kors::logger::ConsoleLogDest;
using FileLogDest = kors::logger::FileLogDest;
}

#endif // KN_LOGGER_H
