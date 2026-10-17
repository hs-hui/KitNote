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
#ifndef KN_CONTEXT_UICONTEXT_H
#define KN_CONTEXT_UICONTEXT_H

#include "ui/uiaction.h"

namespace kn::context {
//! NOTE Determines where to be, what the user is doing

// common ui (re declared for convenience)
static constexpr kfw::ui::UiContext UiCtxUnknown = kfw::ui::UiCtxUnknown;
static constexpr kfw::ui::UiContext UiCtxAny = kfw::ui::UiCtxAny;

static constexpr kfw::ui::UiContext UiCtxHomeOpened = kfw::ui::UiCtxHomeOpened;
static constexpr kfw::ui::UiContext UiCtxProjectOpened = kfw::ui::UiCtxProjectOpened;

static constexpr kfw::ui::UiContext UiCtxProjectFocused = kfw::ui::UiCtxProjectFocused;
static constexpr kfw::ui::UiContext UiCtxBrailleFocused = kfw::ui::UiCtxBrailleFocused;

static constexpr kfw::ui::UiContext UiCtxDialogOpened = kfw::ui::UiCtxDialogOpened;

// application-specific contexts
static constexpr kfw::ui::UiContext UiCtxDevToolsOpened = "UiCtxDevToolsOpened";
}

#endif // KN_CONTEXT_UICONTEXT_H
