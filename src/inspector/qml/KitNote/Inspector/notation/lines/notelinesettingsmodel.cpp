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

#include "notelinesettingsmodel.h"

using namespace kn::inspector;

NoteLineSettingsModel::NoteLineSettingsModel(QObject* parent, const kfw::modularity::ContextPtr& iocCtx,
                                             IElementRepositoryService* repository)
    : TextLineSettingsModel(parent, iocCtx, repository, kn::engraving::ElementType::NOTELINE)
{
    setModelType(InspectorModelType::TYPE_NOTELINE);
    setTitle(kfw::qtrc("inspector", "Note-anchored line"));
    setIcon(kfw::ui::IconCode::Code::NOTE_ANCHORED_LINE);

    createProperties();
}

void NoteLineSettingsModel::createProperties()
{
    TextLineSettingsModel::createProperties();

    isLineVisible()->setIsVisible(true);
    allowDiagonal()->setIsVisible(false);
    placement()->setIsVisible(false);
}
