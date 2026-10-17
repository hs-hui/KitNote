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

#pragma once

#include <qqmlintegration.h>

#include <QString>
#include <QValidator>

namespace kfw::uicomponents {
class IntInputValidator : public QValidator
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int top READ top WRITE setTop)
    Q_PROPERTY(int bottom READ bottom WRITE setBottom)

public:
    explicit IntInputValidator(QObject* parent = nullptr);

    void fixup(QString& string) const override;
    State validate(QString& inputStr, int& cursorPos) const override;

    int top() const;
    int bottom() const;

public slots:
    void setTop(int);
    void setBottom(int);

private:
    int m_top = 999;
    int m_bottom = -999;
};
}
