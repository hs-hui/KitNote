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

#ifndef KN_BRAILLE_BRAILLECONFIGURATION_H
#define KN_BRAILLE_BRAILLECONFIGURATION_H

#include "async/asyncable.h"

#include "ibrailleconfiguration.h"

namespace kn::engraving {
class BrailleConfiguration : public kn::braille::IBrailleConfiguration, public kfw::async::Asyncable
{
public:
    void init();

    kfw::async::Notification braillePanelEnabledChanged() const override;
    bool braillePanelEnabled() const override;
    void setBraillePanelEnabled(const bool enabled) override;

    kfw::async::Notification intervalDirectionChanged() const override;
    braille::BrailleIntervalDirection intervalDirection() const override;
    void setIntervalDirection(const braille::BrailleIntervalDirection) override;

    kfw::async::Notification brailleTableChanged() const override;
    QString brailleTable() const override;
    void setBrailleTable(const QString& table) override;
    QStringList brailleTableList() const override;

private:
    kfw::async::Notification m_braillePanelEnabledChanged;
    kfw::async::Notification m_brailleTableChanged;
    kfw::async::Notification m_intervalDirectionChanged;
};
}

#endif // KN_BRAILLE_BRAILLECONFIGURATION_H
