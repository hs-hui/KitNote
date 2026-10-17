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
#ifndef KN_NOTATION_NOTEINPUTCURSOR_H
#define KN_NOTATION_NOTEINPUTCURSOR_H

#include <QColor>

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "notation/inotationconfiguration.h"

namespace kn::notation {
class NoteInputCursor : public kfw::Contextable
{
    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };

public:
    NoteInputCursor(const kfw::modularity::ContextPtr& iocCtx, bool isThinLine = false);

    void paint(kfw::draw::Painter* painter);

private:
    INotationNoteInputPtr currentNoteInput() const;

    bool m_isThinLine = false;
};
}

#endif // KN_NOTATION_NOTEINPUTCURSOR_H
