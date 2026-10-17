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
#include "graphicsinfomodel.h"

#include <QClipboard>
#include <QGuiApplication>

using namespace kfw::diagnostics;

GraphicsInfoModel::GraphicsInfoModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void GraphicsInfoModel::init()
{
    if (!uiengine()) {
        return;
    }

    QString gApi = uiengine()->graphicsApiName();

    m_info = "\n";
    m_info += "Gui platform:  " + QGuiApplication::platformName() + "\n";
    m_info += "Graphics Api:  " + gApi + "\n";

    emit infoChanged();
}

void GraphicsInfoModel::copyToClipboard()
{
    QGuiApplication::clipboard()->setText(m_info);
}

QString GraphicsInfoModel::info() const
{
    return m_info;
}
