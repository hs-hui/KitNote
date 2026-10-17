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
#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "global/async/asyncable.h"
#include "global/async/notification.h"
#include "global/io/iodevice.h"
#include "global/modularity/ioc.h"

#include "audio/common/audiotypes.h"
#include "audio/common/audiofilereader.h"

#include "../iaudioengine.h"
#include "../iclock.h"
#include "track.h"

namespace kfw::audio::engine {
//! KitNote: the input of a sound track - a recording (wav, flac, mp3, ogg) played along with the score.
//! It follows the sequence's clock like a synthesizer does: a seek sets the position, process() moves it
//! on. Recording time = playback time + offset; the offset comes in the input params
//! (configuration[SOUND_TRACK_OFFSET_KEY], audiotypes.h), so it can change without a reload. The file is
//! decoded in a thread of its own; then the input params say how that went (SOUND_TRACK_STATE_KEY).
//! While the metronome counts in, the sequence's clock stands, and the recording waits with it.
class SoundTrackAudioSource : public ITrackAudioInput, public kfw::Contextable, public async::Asyncable
{
    ContextInject<IAudioEngine> audioEngine = { this };

public:
    SoundTrackAudioSource(const TrackId trackId, io::IODevice* device, IClockPtr clock, const kfw::modularity::ContextPtr& iocCtx);
    ~SoundTrackAudioSource() override;

    bool isActive() const override;
    void setIsActive(const bool active) override;

    void setOutputSpec(const OutputSpec& spec) override;
    unsigned int audioChannelsCount() const override;
    async::Channel<unsigned int> audioChannelsCountChanged() const override;
    samples_t process(float* buffer, samples_t samplesPerChannel) override;

    void seek(const msecs_t newPositionMsecs, const bool flushSound = true) override;
    void flush() override;

    const AudioInputParams& inputParams() const override;
    void applyInputParams(const AudioInputParams& requiredParams) override;
    async::Channel<AudioInputParams> inputParamsChanged() const override;

    void prepareToPlay() override;
    bool readyToPlay() const override;
    async::Notification readyToPlayChanged() const override;

    bool hasPendingChunks() const override;
    void processInput() override;
    InputProcessingProgress inputProcessingProgress() const override;

    void clearCache() override;

private:
    void onDecoded();
    void waitUntilDecoded();

    TrackId m_trackId = -1;
    std::unique_ptr<io::IODevice> m_device;

    AudioFileData m_data;
    std::thread m_decoder;
    std::atomic<bool> m_decodingDone = false;
    bool m_decoded = false;      // m_data may be read (engine threads only)
    async::Notification m_decodingDoneNotification;
    async::Notification m_readyToPlayChanged;
    async::Channel<unsigned int> m_audioChannelsCountChanged;

    AudioInputParams m_params;
    async::Channel<AudioInputParams> m_paramsChanges;
    OutputSpec m_outputSpec;
    bool m_isActive = false;

    IClockPtr m_clock;              // the sequence's
    msecs_t m_clockTime = 0;        // where it was at the last block (or seek)

    msecs_t m_offset = 0;           // microseconds: recording time - playback time
    msecs_t m_seekPosition = 0;     // the playback position of the last seek, microseconds
    uint64_t m_framesSinceSeek = 0; // output frames rendered since then
};

using SoundTrackAudioSourcePtr = std::shared_ptr<SoundTrackAudioSource>;
}
