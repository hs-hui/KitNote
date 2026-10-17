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

#pragma once

#include "audio/engine/internal/fx/abstractfxresolver.h"

#include "modularity/ioc.h"
#include "../../ivstinstancesregister.h"
#include "../../ivstmodulesrepository.h"

namespace kfw::vst {
class VstFxResolver : public audio::fx::AbstractFxResolver, public kfw::Contextable
{
    kfw::ContextInject<IVstModulesRepository> pluginModulesRepo { this };
    kfw::ContextInject<IVstInstancesRegister> instancesRegister { this };
public:
    VstFxResolver(const kfw::modularity::ContextPtr& ctx)
        : kfw::Contextable(ctx) {}

    // IFxResolver::IResolver interface
    audio::AudioResourceMetaList resolveResources() const override;
    void refresh() override;
    void clearAllFx() override;

private:
    audio::IFxProcessorPtr createMasterFx(const audio::AudioFxParams& fxParams, const audio::OutputSpec& outputSpec) const override;
    audio::IFxProcessorPtr createTrackFx(const audio::TrackId trackId, const audio::AudioFxParams& fxParams,
                                         const audio::OutputSpec& outputSpec) const override;

    void removeMasterFx(const audio::AudioResourceId& resoureId, audio::AudioFxChainOrder order) override;
    void removeTrackFx(const audio::TrackId trackId, const audio::AudioResourceId& resoureId, audio::AudioFxChainOrder order) override;
};
}
