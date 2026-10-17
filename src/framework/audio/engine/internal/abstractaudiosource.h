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
#ifndef KFW_AUDIO_ABSTRACTAUDIOSOURCE_H
#define KFW_AUDIO_ABSTRACTAUDIOSOURCE_H

#include "../iaudiosource.h"

namespace kfw::audio::engine {
class AbstractAudioSource : public IAudioSource
{
public:
    virtual ~AbstractAudioSource() = default;

    virtual void setOutputSpec(const OutputSpec& spec) override;

    bool isActive() const override;
    void setIsActive(bool arg) override;

    async::Channel<unsigned int> audioChannelsCountChanged() const override;

protected:
    OutputSpec m_outputSpec;
    async::Channel<unsigned int> m_streamsCountChanged;
    bool m_isActive = false;
};
}

#endif // KFW_AUDIO_ABSTRACTAUDIOSOURCE_H
