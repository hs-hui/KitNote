/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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
#include "toursservice.h"

using namespace kfw::tours;

void ToursService::registerTour(const String& eventCode, const Tour& tour)
{
    if (kfw::contains(m_eventsMap, eventCode)) {
        return;
    }

    m_eventsMap.insert({ eventCode, tour });
}

void ToursService::onEvent(const String& eventCode)
{
    if (!kfw::contains(m_eventsMap, eventCode)) {
        return;
    }

    const Tour& tour = m_eventsMap[eventCode];

    String lastTourId = toursConfiguration()->lastShownTourIdForEvent(eventCode);
    if (lastTourId == tour.id) {
        return;
    }

    toursProvider()->showTour(tour);

    toursConfiguration()->setLastShownTourIdForEvent(eventCode, tour.id);
}
