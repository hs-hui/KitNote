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
#include "qmllauncher.h"

using namespace kfw::ui;

QmlLauncher::QmlLauncher(QObject* parent, const modularity::ContextPtr& iocCtx)
    : QObject(parent), Contextable(iocCtx)
{
}

bool QmlLauncher::open(const QString& uri)
{
    interactive()->open(UriQuery(uri.toStdString()));
    return true;
}

bool QmlLauncher::openSync(const QString& uri)
{
    return interactive()->openSync(UriQuery(uri.toStdString())).ret;
}

bool QmlLauncher::openApp(const QString& uri)
{
    interactive()->openApp(UriQuery(uri.toStdString()));
    return true;
}

bool QmlLauncher::openUrl(const QString& url)
{
    return interactive()->openUrl(QUrl(url));
}
