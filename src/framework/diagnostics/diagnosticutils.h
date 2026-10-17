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
#ifndef KFW_DIAGNOSTICS_DIAGNOSTICUTILS_H
#define KFW_DIAGNOSTICS_DIAGNOSTICUTILS_H

#include <QObject>

#include "types/uri.h"
#include "log.h"

namespace kfw::diagnostics {
inline bool isDiagnosticObject(const QObject* obj, bool print = false)
{
    if (print) {
        LOGI() << "objectName: " << obj->objectName() << ", className: " << obj->metaObject()->className();
    }
    if (obj->objectName().toLower().contains("diagnostic")) {
        return true;
    }
    return false;
}

inline bool isDiagnosticChild(const QObject* obj, bool print = false)
{
    for (const QObject* ch : obj->children()) {
        if (isDiagnosticObject(ch, print)) {
            return true;
        }

        if (isDiagnosticChild(ch, print)) {
            return true;
        }
    }

    return false;
}

inline bool isDiagnosticParent(const QObject* obj, bool print = false)
{
    QObject* prn = obj->parent();
    if (!prn) {
        return false;
    }

    if (isDiagnosticObject(prn, print)) {
        return true;
    }

    return isDiagnosticParent(prn, print);
}

inline bool isDiagnosticHierarchy(const QObject* obj, bool print = false)
{
    if (isDiagnosticObject(obj, print)) {
        return true;
    }
    if (isDiagnosticParent(obj, print)) {
        return true;
    }
    if (isDiagnosticChild(obj, print)) {
        return true;
    }
    return false;
}

inline bool isDiagnosticsUri(const kfw::Uri& uri)
{
    return QString::fromStdString(uri.toString()).startsWith("kitnote://diagnostics");
}

inline std::vector<kfw::Uri> removeDiagnosticsUri(const std::vector<kfw::Uri>& uris)
{
    std::vector<kfw::Uri> nuris;
    for (const kfw::Uri& uri : uris) {
        if (!isDiagnosticsUri(uri)) {
            nuris.push_back(uri);
        }
    }
    return nuris;
}

inline kfw::Uri diagnosticCurrentUri(const std::vector<kfw::Uri>& stack)
{
    std::vector<kfw::Uri> uris = removeDiagnosticsUri(stack);
    if (!uris.empty()) {
        return uris.back();
    }
    return kfw::Uri();
}
}

#endif // KFW_DIAGNOSTICS_DIAGNOSTICUTILS_H
