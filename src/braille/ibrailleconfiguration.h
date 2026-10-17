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
#ifndef KN_BRAILLE_IBRAILLECONFIGURATION_H
#define KN_BRAILLE_IBRAILLECONFIGURATION_H

#include <QString>

#include "modularity/imoduleinterface.h"
#include "async/notification.h"

#include "brailletypes.h"

namespace kn::braille {
class IBrailleConfiguration : MODULE_GLOBAL_INTERFACE
{
    INTERFACE_ID(IBrailleConfiguration)

public:
    virtual ~IBrailleConfiguration() = default;

    virtual kfw::async::Notification braillePanelEnabledChanged() const = 0;
    virtual bool braillePanelEnabled() const = 0;
    virtual void setBraillePanelEnabled(const bool enabled) = 0;

    virtual kfw::async::Notification intervalDirectionChanged() const = 0;
    virtual BrailleIntervalDirection intervalDirection() const = 0;
    virtual void setIntervalDirection(const BrailleIntervalDirection direction) = 0;

    virtual kfw::async::Notification brailleTableChanged() const = 0;
    virtual QString brailleTable() const = 0;
    virtual void setBrailleTable(const QString& table) = 0;
    virtual QStringList brailleTableList() const = 0;
};
}

#endif // KN_BRAILLE_IBRAILLECONFIGURATION_H
