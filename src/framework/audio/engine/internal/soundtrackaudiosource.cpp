/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
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
#include "soundtrackaudiosource.h"

#include <cmath>
#include <string>

#include "audio/common/audiosanitizer.h"

#include "log.h"

using namespace kfw;
using namespace kfw::audio;
using namespace kfw::audio::engine;

static msecs_t offsetFromParams(const AudioInputParams& params)
{
    auto it = params.configuration.find(SOUND_TRACK_OFFSET_KEY);
    if (it == params.configuration.end()) {
        return 0;
    }
    try {
        return static_cast<msecs_t>(std::stoll(it->second));
    } catch (...) {
        return 0;
    }
}

//! Catmull-Rom: between b and c, a and d around them
static inline float interpolate(float a, float b, float c, float d, float t)
{
    return b + 0.5f * t * (c - a + t * (2.f * a - 5.f * b + 4.f * c - d + t * (3.f * (b - c) + d - a)));
}

SoundTrackAudioSource::SoundTrackAudioSource(const TrackId trackId, io::IODevice* device, IClockPtr clock,
                                             const modularity::ContextPtr& iocCtx)
    : kfw::Contextable(iocCtx), m_trackId(trackId), m_device(device), m_clock(std::move(clock))
{
    ONLY_AUDIO_ENGINE_THREAD;

    // (sent from the decoding thread, received here on the engine's)
    m_decodingDoneNotification.onNotify(this, [this]() {
        onDecoded();
    });

    m_decoder = std::thread([this]() {
        if (m_device && m_device->open(io::IODevice::ReadOnly)) {
            const uint8_t* bytes = m_device->readData();
            if (!readAudioFile(bytes, m_device->size(), m_data)) {
                m_data = AudioFileData();
            }
            m_device->close();
        }
        m_device.reset();       // the file's bytes are not needed any more
        m_decodingDone.store(true, std::memory_order_release);
        m_decodingDoneNotification.notify();
    });
}

SoundTrackAudioSource::~SoundTrackAudioSource()
{
    if (m_decoder.joinable()) {
        m_decoder.join();
    }
}

void SoundTrackAudioSource::onDecoded()
{
    ONLY_AUDIO_ENGINE_THREAD;

    const bool ok = m_data.isValid();
    if (!ok) {
        LOGE() << "KitNote: the recording of track " << m_trackId << " could not be read";
    }

    m_params.configuration[SOUND_TRACK_STATE_KEY] = ok ? "ready" : "failed";
    m_params.configuration[SOUND_TRACK_DURATION_KEY]
        = std::to_string(ok ? static_cast<long long>(m_data.frames() * 1000000 / m_data.sampleRate) : 0LL);
    m_paramsChanges.send(m_params);
    m_readyToPlayChanged.notify();
}

void SoundTrackAudioSource::waitUntilDecoded()
{
    if (!m_decodingDone.load(std::memory_order_acquire) && m_decoder.joinable()) {
        m_decoder.join();
    }
}

bool SoundTrackAudioSource::isActive() const
{
    return m_isActive;
}

void SoundTrackAudioSource::setIsActive(const bool active)
{
    m_isActive = active;
}

void SoundTrackAudioSource::setOutputSpec(const OutputSpec& spec)
{
    // the frames counted so far were of the old rate (an export at a rate of its own begins with a seek)
    if (m_outputSpec.isValid() && spec.sampleRate != m_outputSpec.sampleRate) {
        m_seekPosition += static_cast<msecs_t>(m_framesSinceSeek * 1000000 / m_outputSpec.sampleRate);
        m_framesSinceSeek = 0;
    }
    m_outputSpec = spec;
}

unsigned int SoundTrackAudioSource::audioChannelsCount() const
{
    return m_outputSpec.audioChannelCount;
}

async::Channel<unsigned int> SoundTrackAudioSource::audioChannelsCountChanged() const
{
    return m_audioChannelsCountChanged;
}

samples_t SoundTrackAudioSource::process(float* buffer, samples_t samplesPerChannel)
{
    if (!m_isActive || !m_outputSpec.isValid()) {
        return 0;
    }

    // an export does not wait for the file in the real-time way: it is decoded first
    if (!m_decodingDone.load(std::memory_order_acquire) && audioEngine() && audioEngine()->mode() == RenderMode::OfflineMode) {
        waitUntilDecoded();
    }

    const audioch_t outChannels = m_outputSpec.audioChannelCount;

    // a count-in: the clock (moved on before the tracks are processed) stands while the metronome counts,
    // and the score waits for it - so does the recording
    if (m_clock && m_clock->isRunning()) {
        const msecs_t clockTime = m_clock->currentTime();
        const bool countingIn = clockTime == m_clockTime;
        m_clockTime = clockTime;
        if (countingIn) {
            std::fill(buffer, buffer + samplesPerChannel * outChannels, 0.f);
            return samplesPerChannel;
        }
    }

    const bool ready = m_decodingDone.load(std::memory_order_acquire) && m_data.isValid();

    // the position moves on by the frames rendered, counted from the last seek (as the synthesizers'
    // notes come out: measured against them, sample for sample, by tools/kitnote/smoke_test.py)
    const uint64_t firstFrame = m_framesSinceSeek;
    m_framesSinceSeek += samplesPerChannel;

    if (!ready) {
        std::fill(buffer, buffer + samplesPerChannel * outChannels, 0.f);
        return samplesPerChannel;
    }

    const unsigned int inChannels = m_data.channels;
    const int64_t frames = static_cast<int64_t>(m_data.frames());
    const int16_t* data = m_data.samples.data();
    const double step = static_cast<double>(m_data.sampleRate) / m_outputSpec.sampleRate;
    // the frame of the recording at the last seek, and from there one step per output frame
    const double start = static_cast<double>(m_seekPosition + m_offset) * 1e-6 * m_data.sampleRate
                         + static_cast<double>(firstFrame) * step;

    auto at = [data, frames, inChannels](int64_t frame, unsigned int channel) -> float {
        if (frame < 0 || frame >= frames) {
            return 0.f;
        }
        return data[frame * inChannels + channel] * (1.f / 32768.f);
    };

    for (samples_t s = 0; s < samplesPerChannel; ++s) {
        const double pos = start + static_cast<double>(s) * step;
        const double whole = std::floor(pos);
        const int64_t i = static_cast<int64_t>(whole);
        const float t = static_cast<float>(pos - whole);

        for (audioch_t ch = 0; ch < outChannels; ++ch) {
            // mono to both sides; of more than two channels the first two
            const unsigned int c = std::min<unsigned int>(ch, inChannels - 1);
            float value = 0.f;
            if (i + 2 >= 0 && i - 1 < frames) {
                value = t == 0.f ? at(i, c) : interpolate(at(i - 1, c), at(i, c), at(i + 1, c), at(i + 2, c), t);
            }
            buffer[s * outChannels + ch] = value;
        }
    }

    return samplesPerChannel;
}

void SoundTrackAudioSource::seek(const msecs_t newPositionMsecs, const bool)
{
    m_seekPosition = newPositionMsecs;
    m_framesSinceSeek = 0;
    m_clockTime = m_clock ? m_clock->currentTime() : 0;
}

void SoundTrackAudioSource::flush()
{
}

const AudioInputParams& SoundTrackAudioSource::inputParams() const
{
    return m_params;
}

void SoundTrackAudioSource::applyInputParams(const AudioInputParams& requiredParams)
{
    m_offset = offsetFromParams(requiredParams);

    // what the engine knows of the file stays
    AudioInputParams params = requiredParams;
    for (const char* key : { SOUND_TRACK_STATE_KEY, SOUND_TRACK_DURATION_KEY }) {
        params.configuration.erase(key);
        auto it = m_params.configuration.find(key);
        if (it != m_params.configuration.end()) {
            params.configuration[key] = it->second;
        }
    }

    if (params == m_params) {
        return;
    }

    m_params = params;
    m_paramsChanges.send(m_params);
}

async::Channel<AudioInputParams> SoundTrackAudioSource::inputParamsChanged() const
{
    return m_paramsChanges;
}

void SoundTrackAudioSource::prepareToPlay()
{
}

bool SoundTrackAudioSource::readyToPlay() const
{
    return m_decodingDone.load(std::memory_order_acquire);
}

async::Notification SoundTrackAudioSource::readyToPlayChanged() const
{
    return m_readyToPlayChanged;
}

bool SoundTrackAudioSource::hasPendingChunks() const
{
    return false;
}

void SoundTrackAudioSource::processInput()
{
}

InputProcessingProgress SoundTrackAudioSource::inputProcessingProgress() const
{
    return {};
}

void SoundTrackAudioSource::clearCache()
{
}
