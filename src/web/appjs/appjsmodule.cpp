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
#include "appjsmodule.h"

#include <emscripten/val.h>

#include "webapi.h"

using namespace kfw;
using namespace kn::appjs;

std::string AppJsModule::moduleName() const
{
    return "appjs";
}

void AppJsModule::onInit(const kfw::IApplication::RunMode&)
{
    WebApi::instance()->init();
}

void AppJsModule::onDeinit()
{
    WebApi::instance()->deinit();
}

void AppJsModule::onStartApp()
{
    emscripten::val::module_property("onStartApp")();
}
