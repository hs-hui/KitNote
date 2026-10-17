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
#ifndef KN_IMPORTEXPORT_ABSTRACTAUDIOWRITER_H
#define KN_IMPORTEXPORT_ABSTRACTAUDIOWRITER_H

#include "async/asyncable.h"
#include "modularity/ioc.h"
#include "audio/main/iplayback.h"
#include "audio/main/istartaudiocontroller.h"
#include "iaudioexportconfiguration.h"
#include "context/iglobalcontext.h"
#include "playback/iplaybackcontroller.h"
#include "global/iapplication.h"

#include "project/inotationwriter.h"

namespace kn::iex::audioexport {
class AbstractAudioWriter : public project::INotationWriter, public kfw::Contextable, public kfw::async::Asyncable
{
public:
    kfw::GlobalInject<IAudioExportConfiguration> configuration;
    kfw::ContextInject<kfw::audio::IPlayback> playback = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::audio::IStartAudioController> startAudioController = { this };
    kfw::ContextInject<playback::IPlaybackController> playbackController  = { this };
    kfw::ContextInject<kfw::IApplication> application  = { this };

public:
    AbstractAudioWriter(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    std::vector<UnitType> supportedUnitTypes() const override;
    bool supportsUnitType(UnitType unitType) const override;

    kfw::Ret write(notation::INotationPtr notation, kfw::io::IODevice& dstDevice, const Options& options = Options()) override;
    kfw::Ret writeList(const notation::INotationPtrList& notations, kfw::io::IODevice& dstDevice,
                        const Options& options = Options()) override;

    kfw::Progress* progress() override;
    void abort() override;

protected:
    kfw::Ret doWriteAndWait(notation::INotationPtr notation, kfw::io::IODevice& dstDevice, const kfw::audio::SoundTrackFormat& format,
                             const Options& options = Options());

private:
    void doWrite(const QString& path, const kfw::audio::SoundTrackFormat& format);

    UnitType unitTypeFromOptions(const Options& options) const;

    kfw::Progress m_progress;
    bool m_isCompleted = false;
    kfw::Ret m_writeRet;
};
}

#endif // KN_IMPORTEXPORT_ABSTRACTAUDIOWRITER_H
