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
#include "qmltranslation.h"

#include "translation.h"

using namespace kfw::ui;

QmlTranslation::QmlTranslation(QObject* parent)
    : QObject(parent)
{
}

QString QmlTranslation::translate(const QString& context, const QString& text, const QString& disambiguation, int n) const
{
    return kfw::qtrc(context.toUtf8().constData(),
                      text.toUtf8().constData(),
                      disambiguation.isEmpty() ? nullptr : disambiguation.toUtf8().constData(),
                      n);
}
