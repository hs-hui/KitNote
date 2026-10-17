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
#ifndef KN_ENGRAVING_ACCESSIBLEROOT_H
#define KN_ENGRAVING_ACCESSIBLEROOT_H

#include "accessibleitem.h"
#include "../dom/rootitem.h"

namespace kn::engraving {
using AccessibleMapToScreenFunc = std::function<RectF (const RectF&)>;

class AccessibleRoot : public AccessibleItem
{
    OBJECT_ALLOCATOR(engraving, AccessibleRoot)
public:
    AccessibleRoot(RootItem* e, Role role);
    ~AccessibleRoot() override;

    void setFocusedElement(AccessibleItemPtr e, bool voiceStaffInfoChange = true);
    AccessibleItemWeakPtr focusedElement() const;

    void notifyAboutFocusedElementNameChanged();

    void setMapToScreenFunc(const AccessibleMapToScreenFunc& func);
    RectF toScreenRect(const RectF& rect, bool* ok = nullptr) const;

    const kfw::accessibility::IAccessible* accessibleParent() const override;
    kfw::accessibility::IAccessible::Role accessibleRole() const override;
    QString accessibleName() const override;

    bool enabled() const;
    void setEnabled(bool enabled);

    QString staffInfo() const;
    void updateStaffInfo(const AccessibleItemWeakPtr newAccessibleItem, const AccessibleItemWeakPtr oldAccessibleItem,
                         bool voiceStaffInfoChange = true);

    bool isRangeSelection() const;
    QString rangeSelectionInfo();

private:

    bool m_enabled = false;

    AccessibleItemWeakPtr m_focusedElement;

    AccessibleMapToScreenFunc m_accessibleMapToScreenFunc;

    QString m_staffInfo;
};
}

#endif // KN_ENGRAVING_ACCESSIBLEROOT_H
