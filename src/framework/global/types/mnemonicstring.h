/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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
#ifndef KFW_GLOBAL_MNEMONICSTRING_H
#define KFW_GLOBAL_MNEMONICSTRING_H

#ifndef NO_QT_SUPPORT
#include <QString>
#endif

#include "translatablestring.h"

namespace kfw {
class MnemonicString
{
public:
    MnemonicString() = default;
    MnemonicString(const TranslatableString& raw)
        : m_raw(raw) {}

    inline bool isEmpty() const
    {
        return m_raw.isEmpty();
    }

    inline const TranslatableString& raw() const
    {
        return m_raw;
    }

#ifndef NO_QT_SUPPORT
    inline QString qTranslatedWithMnemonicAmpersand() const
    {
        return m_raw.qTranslated();
    }

    QString qTranslatedWithMnemonicUnderline() const;

    QString qTranslatedWithoutMnemonic() const;
#endif

    inline bool operator ==(const MnemonicString& other) const
    {
        return m_raw == other.m_raw;
    }

    inline bool operator !=(const MnemonicString& other) const
    {
        return m_raw != other.m_raw;
    }

private:
    TranslatableString m_raw;
};
}

#endif // KFW_GLOBAL_MNEMONICSTRING_H
