/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#pragma once

#include "ui/iuiactionsregister.h"
#include "shortcuts/ishortcutsregister.h"

#include "iinteractive.h"

#include "engraving/rendering/isinglerenderer.h"
#include "engraving/rw/xmlreader.h"

#include "engraving/dom/chord.h"
#include "engraving/dom/drumset.h"

#include "types/retval.h"

namespace kn::notation {
class PercussionUtilities : public kfw::Contextable
{
    kfw::ContextInject<kfw::ui::IUiActionsRegister> uiactionsRegister = { this };
    kfw::ContextInject<kfw::shortcuts::IShortcutsRegister> shortcutsRegister = { this };

    kfw::ContextInject<kfw::IInteractive> interactive = { this };

    kfw::ContextInject<kn::engraving::rendering::ISingleRenderer> engravingRender = { this };

public:

    PercussionUtilities(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx)
    {
    }

    void readDrumset(const kfw::ByteArray& drumMapping, kn::engraving::Drumset& drumset);
    std::shared_ptr<kn::engraving::Chord> getDrumNoteForPreview(const kn::engraving::Drumset* drumset, int pitch);
    bool editPercussionShortcut(kn::engraving::Drumset& drumset, int originPitch);

private:
    kfw::RetVal<kfw::Val> openPercussionShortcutDialog(const kn::engraving::Drumset& drumset, int originPitch);
    QVariantMap drumToQVariantMap(int pitch, const engraving::DrumInstrument& drum);
};
}
