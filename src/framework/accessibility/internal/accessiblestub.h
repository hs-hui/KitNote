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
#ifndef KFW_ACCESSIBILITY_ACCESSIBLESTUB_H
#define KFW_ACCESSIBILITY_ACCESSIBLESTUB_H

#include <QAccessibleInterface>

namespace kfw::accessibility {
class AccessibleStub : public QAccessibleInterface
{
public:
    AccessibleStub(QObject* pObj);

    static QAccessibleInterface* accessibleInterface(QObject* object);

    bool isValid() const override;
    QObject* object() const override;
    QWindow* window() const override;

    QVector<QPair<QAccessibleInterface*, QAccessible::Relation> > relations(QAccessible::Relation) const override;

    QAccessibleInterface* childAt(int, int) const override;

    QAccessibleInterface* parent() const override;
    QAccessibleInterface* child(int) const override;
    int childCount() const override;
    int indexOfChild(const QAccessibleInterface*) const override;

    QString text(QAccessible::Text) const override;
    void setText(QAccessible::Text, const QString&) override;
    QRect rect() const override;
    QAccessible::Role role() const override;
    QAccessible::State state() const override;

    void* interface_cast(QAccessible::InterfaceType) override;

private:
    QObject* m_object = nullptr;
};
}

#endif // KFW_ACCESSIBILITY_ACCESSIBLESTUBT_H
