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

#include "abstractkeynavdevitem.h"

namespace kfw::diagnostics {
class KeyNavDevSubSection : public AbstractKeyNavDevItem
{
    Q_OBJECT

    Q_PROPERTY(QString direction READ direction CONSTANT)
    Q_PROPERTY(QVariantList controls READ controls NOTIFY controlsChanged)
    Q_PROPERTY(int controlsCount READ controlsCount NOTIFY controlsCountChanged)

    QML_ELEMENT;
    QML_UNCREATABLE("Must be created in C++ only");

public:
    explicit KeyNavDevSubSection(kfw::ui::INavigationPanel* subsection);

    QString direction() const;
    QVariantList controls() const;
    int controlsCount() const;

public slots:
    void setControls(const QVariantList& controls);

signals:
    void controlsChanged();
    void controlsCountChanged();

private:
    kfw::ui::INavigationPanel* m_subsection = nullptr;
    QVariantList m_controls;
};
}
