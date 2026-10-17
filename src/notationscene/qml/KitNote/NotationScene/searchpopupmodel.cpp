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
#include "searchpopupmodel.h"

using namespace kn::notation;

SearchPopupModel::SearchPopupModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void SearchPopupModel::classBegin()
{
    init();
}

void SearchPopupModel::init()
{
    dispatcher()->reg(this, "find", [this]() {
        emit showPopupRequested();
    });
}

void SearchPopupModel::search(const QString& text)
{
    INotationPtr notation = globalContext()->currentNotation();
    if (!notation) {
        return;
    }

    notation->interaction()->clearSelection();

    std::vector<EngravingItem*> elements = notation->elements()->search(text);
    if (!elements.empty()) {
        const NoteInputState& inputState = notation->interaction()->noteInput()->state();
        notation->interaction()->select(elements, elements.size() == 1 ? SelectType::SINGLE : SelectType::RANGE,
                                        inputState.isValid() ? inputState.staffIdx() : 0);
        notation->interaction()->showItem(elements.front());
    }
}
