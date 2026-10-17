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
#ifndef KFW_AUDIO_EQUALISER_H
#define KFW_AUDIO_EQUALISER_H

#include "../../ifxprocessor.h"

namespace kfw::audio::fx {
class Equaliser : public IFxProcessor
{
public:
    Equaliser();

    void setOutputSpec(const OutputSpec& spec) override;

    bool active() const override;
    void setActive(bool active) override;

    void setFrequency(float value);
    void setGain(float value);
    void setQ(float value);

    void setPlaying(bool playing) override;
    bool shouldProcessDuringSilence() const override;

    void process(float* buffer, samples_t sampleCount, samples_t playbackPositionSamples = 0) override;

private:
    void calculate();

    OutputSpec m_outputSpec;
    bool m_active = true;

    float m_gain = 0, m_frequency = 1000.f, m_q = 1.f;
    float m_a[3] = { 0, 0, 0 };
    float m_b[3] = { 0, 0, 0 };
    float m_x[3] = { 0, 0, 0 };
    float m_y[3] = { 0, 0, 0 };
};
}

#endif // KFW_AUDIO_EQUALISER_H
