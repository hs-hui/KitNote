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

#ifndef KN_ENGRAVING_STAFFTEXT_H
#define KN_ENGRAVING_STAFFTEXT_H

#include "stafftextbase.h"

namespace kn::engraving {
class StaffText final : public StaffTextBase
{
    OBJECT_ALLOCATOR(engraving, StaffText)
    DECLARE_CLASSOF(ElementType::STAFF_TEXT)

public:
    StaffText(Segment* parent = nullptr, TextStyleType = TextStyleType::STAFF);
    StaffText(const StaffText&);

    bool isEditAllowed(EditData&) const override;

    StaffText* clone() const override { return new StaffText(*this); }
    EngravingItem* linkedClone() override;

    void scanElements(std::function<void(EngravingItem*)> func) override;

    void add(EngravingItem*) override;
    void remove(EngravingItem*) override;

    void setTrack(track_idx_t idx) override;

    bool hasSoundFlag() const;
    SoundFlag* soundFlag() const;
    void setSoundFlag(SoundFlag* flag);

private:
    PropertyValue propertyDefault(Pid id) const override;

    SoundFlag* m_soundFlag = nullptr;
};
} // namespace kn::engraving
#endif // KN_ENGRAVING_STAFFTEXT_H
