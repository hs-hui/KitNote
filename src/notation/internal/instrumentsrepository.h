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

#include "modularity/ioc.h"

#include "iinstrumentsrepository.h"
#include "async/asyncable.h"

#include "io/ifilesystem.h"
#include "inotationconfiguration.h"

namespace kn::notation {
class InstrumentsRepository : public IInstrumentsRepository, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;
    kfw::GlobalInject<INotationConfiguration> configuration;

public:
    InstrumentsRepository(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();

    const InstrumentTemplateList& instrumentTemplates() const override;
    const InstrumentTemplate& instrumentTemplate(const kfw::String& instrumentId) const override;

    const ScoreOrderList& orders() const override;
    const ScoreOrder& order(const kfw::String& orderId) const override;

    const InstrumentGenreList& genres() const override;
    const InstrumentGroupList& groups() const override;

    const InstrumentStringTuningsMap& stringTuningsPresets() const override;

private:
    using InstrumentTemplateMap = std::unordered_map<kfw::String, const InstrumentTemplate*>;

    void load();
    void clear();

    bool loadStringTuningsPresets(const kfw::io::path_t& path);

    InstrumentTemplateList m_instrumentTemplateList;
    InstrumentGroupList m_percussionGroups;
    InstrumentTemplateMap m_instrumentTemplateMap;
    InstrumentStringTuningsMap m_stringTuningsPresets;
};
}
