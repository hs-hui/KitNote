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

#ifndef KN_ENGRAVING_PLAYINGTECHNIQUEANNOTATION_H
#define KN_ENGRAVING_PLAYINGTECHNIQUEANNOTATION_H

#include "stafftextbase.h"
#include "../types/types.h"

namespace kn::engraving {
class PlayTechAnnotation final : public StaffTextBase
{
    OBJECT_ALLOCATOR(engraving, PlayTechAnnotation)
    DECLARE_CLASSOF(ElementType::PLAYTECH_ANNOTATION)

public:
    PlayTechAnnotation(Segment* parent = nullptr, PlayingTechniqueType techniqueType = PlayingTechniqueType::Natural,
                       TextStyleType tid = TextStyleType::STAFF);

    PlayingTechniqueType techniqueType() const;
    void setTechniqueType(const PlayingTechniqueType techniqueType);
    bool playPlayTechAnnotation() const { return m_playPlayTechAnnotation; }
    void setPlayPlayTechAnnotation(bool play) { m_playPlayTechAnnotation = play; }

    PlayTechAnnotation* clone() const override;
    TranslatableString subtypeUserName() const override;

    bool isHandbellsSymbol() const;
    bool isEditable() const override { return !isHandbellsSymbol(); }

    PropertyValue getProperty(Pid id) const override;
    bool setProperty(Pid propertyId, const PropertyValue& val) override;
    PropertyValue propertyDefault(Pid id) const override;

private:
    PlayingTechniqueType m_techniqueType = PlayingTechniqueType::Undefined;
    bool m_playPlayTechAnnotation = true;
};
}

#endif // KN_ENGRAVING_PLAYINGTECHNIQUEANNOTATION_H
