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

#ifndef KN_NOTATION_NOTATIONACCESSIBILITY_H
#define KN_NOTATION_NOTATIONACCESSIBILITY_H

#include "inotationaccessibility.h"
#include "notationtypes.h"

#include "async/asyncable.h"
#include "async/notification.h"

namespace kn::notation {
class IGetScore;
class Notation;
class NotationAccessibility : public INotationAccessibility, public kfw::async::Asyncable
{
public:
    NotationAccessibility(const Notation* notation);

    kfw::ValCh<std::string> accessibilityInfo() const override;

    void setMapToScreenFunc(const kn::engraving::AccessibleMapToScreenFunc& func) override;

    void setEnabled(bool enabled) override;

private:
    const engraving::Score* score() const;
    const engraving::Selection* selection() const;

    void updateAccessibilityInfo();

    void setAccessibilityInfo(const QString& info);

    QString rangeAccessibilityInfo() const;
    QString singleElementAccessibilityInfo() const;

    const IGetScore* m_getScore = nullptr;
    kfw::ValCh<std::string> m_accessibilityInfo;
};
}

#endif // KN_NOTATION_NOTATIONACCESSIBILITY_H
