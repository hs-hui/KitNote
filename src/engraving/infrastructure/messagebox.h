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
#ifndef KN_ENGRAVING_MESSAGEBOX_H
#define KN_ENGRAVING_MESSAGEBOX_H

#include <set>
#include <string>

#include "modularity/ioc.h"

#ifndef ENGRAVING_NO_INTERACTIVE
#include "iinteractive.h"
#endif

namespace kn::engraving {
class MessageBox : public kfw::Contextable
{
#ifndef ENGRAVING_NO_INTERACTIVE
    kfw::ContextInject<kfw::IInteractive> interactive = { this };
#endif
public:

    MessageBox(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    enum Button {
        Ok,
        Cancel
    };

    Button warning(const std::string& title, const std::string& text, const std::set<Button>& buttons = { Ok, Cancel });
};
}

#endif // KN_ENGRAVING_MESSAGEBOX_H
