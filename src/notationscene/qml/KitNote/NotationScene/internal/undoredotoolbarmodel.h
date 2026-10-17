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

#include <qqmlintegration.h>

#include "uicomponents/qml/KitNote/UiComponents/abstracttoolbarmodel.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"

namespace kn::notation {
class UndoRedoToolbarModel : public kfw::uicomponents::AbstractToolBarModel
{
    Q_OBJECT
    QML_ELEMENT;

    kfw::ContextInject<context::IGlobalContext> context = { this };

public:
    explicit UndoRedoToolbarModel(QObject* parent = nullptr);

    Q_INVOKABLE void load() override;

private:
    void onActionsStateChanges(const kfw::actions::ActionCodeList& codes) override;

    INotationUndoStackPtr undoStack() const;
    void updateItems();
    void subsribeOnUndoStackChanges();
};
}
