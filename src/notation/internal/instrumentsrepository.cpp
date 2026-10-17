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
#include "instrumentsrepository.h"

#include "global/serialization/json.h"

#include "engraving/dom/instrtemplate.h"
#include "engraving/types/types.h"

#include "io/path.h"
#include "mpe/playbacksetupdata.h"

#include "log.h"
#include "translation.h"

using namespace kfw;
using namespace kfw::io;
using namespace kn::notation;

void InstrumentsRepository::init()
{
    configuration()->userInstrumentsFolderChanged().onReceive(this, [this](const auto&) {
        load();
    });

    load();
}

const InstrumentTemplateList& InstrumentsRepository::instrumentTemplates() const
{
    return m_instrumentTemplateList;
}

const InstrumentTemplate& InstrumentsRepository::instrumentTemplate(const String& instrumentId) const
{
    auto it = m_instrumentTemplateMap.find(instrumentId);
    if (it == m_instrumentTemplateMap.end()) {
        static const InstrumentTemplate dummy;
        return dummy;
    }

    return *it->second;
}

const ScoreOrderList& InstrumentsRepository::orders() const
{
    return kn::engraving::instrumentOrders;
}

const ScoreOrder& InstrumentsRepository::order(const String& orderId) const
{
    const ScoreOrderList& orders = kn::engraving::instrumentOrders;

    auto it = std::find_if(orders.begin(), orders.end(), [orderId](const ScoreOrder& order) {
        return order.id == orderId;
    });

    if (it == orders.cend()) {
        static ScoreOrder dummy;
        return dummy;
    }

    return *it;
}

const InstrumentGenreList& InstrumentsRepository::genres() const
{
    return kn::engraving::instrumentGenres;
}

const InstrumentGroupList& InstrumentsRepository::groups() const
{
    // KitNote: only the families that contain unpitched percussion.
    return m_percussionGroups;
}

const InstrumentStringTuningsMap& InstrumentsRepository::stringTuningsPresets() const
{
    return m_stringTuningsPresets;
}

void InstrumentsRepository::load()
{
    TRACEFUNC;

    m_instrumentTemplateList.clear();
    m_instrumentTemplateMap.clear();
    m_percussionGroups.clear();
    kn::engraving::clearInstrumentTemplates();

    const path_t instrumentsXmlPath = configuration()->instrumentsXmlPath();
    if (!kn::engraving::loadInstrumentTemplates(instrumentsXmlPath)) {
        LOGE() << "Could not load instruments from " << instrumentsXmlPath;
    }

    const path_t scoreOrdersXmlPath = configuration()->scoreOrdersXmlPath();
    if (!kn::engraving::loadInstrumentTemplates(scoreOrdersXmlPath)) {
        LOGE() << "Could not load score orders from " << scoreOrdersXmlPath;
    }

    for (const path_t& path : configuration()->userInstrumentsAndScoreOrdersPaths()) {
        if (!kn::engraving::loadInstrumentTemplates(path)) {
            LOGE() << "Could not load user instruments and score orders from " << path;
        }
    }


    for (const InstrumentGroup* group : kn::engraving::instrumentGroups) {
        for (const InstrumentTemplate* templ : group->instrumentTemplates) {
            if (templ->useDrumset && !templ->trackName.isEmpty() && !templ->longNames.empty()) {
                m_percussionGroups.push_back(group);
                break;
            }
        }

        for (const InstrumentTemplate* templ : group->instrumentTemplates) {
            if (templ->trackName.isEmpty() || templ->longNames.empty()) {
                continue;
            }

            // KitNote: only unpitched percussion is offered when choosing instruments.
            // Every template stays in the map so that existing scores still open.
            if (templ->useDrumset) {
                m_instrumentTemplateList.push_back(templ);
            }
            m_instrumentTemplateMap.insert_or_assign(templ->id, templ);
        }
    }

    path_t stringTuningsPresetsPath = configuration()->stringTuningsPresetsPath();
    if (!loadStringTuningsPresets(stringTuningsPresetsPath)) {
        LOGE() << "Could not load string tunings presets from " << stringTuningsPresetsPath << "!";
    }
}

bool InstrumentsRepository::loadStringTuningsPresets(const path_t& path)
{
    TRACEFUNC;

    Ret ret = fileSystem()->exists(path);
    if (!ret) {
        LOGE() << ret.toString();
        return false;
    }

    RetVal<ByteArray> retVal = fileSystem()->readFile(path);
    if (!retVal.ret) {
        LOGE() << retVal.ret.toString();
        return false;
    }

    std::string err;
    JsonArray arr = JsonDocument::fromJson(retVal.val, &err).rootArray();
    if (!err.empty()) {
        LOGE() << "failed parse string tunings presets, err: " << err;
        return false;
    }

    for (size_t i = 0; i < arr.size(); ++i) {
        const JsonValue& presetInfoVal = arr.at(i);
        JsonObject presetInfoObj = presetInfoVal.toObject();

        std::vector<StringTuningsInfo> strings;

        JsonArray stringsArr = presetInfoObj.value("strings").toArray();
        for (size_t j = 0; j < stringsArr.size(); ++j) {
            StringTuningsInfo info;

            const JsonValue& stringVal = stringsArr.at(j);
            JsonObject stringObj = stringVal.toObject();

            info.number = stringObj.value("number").toInt();

            JsonArray presetsArr = stringObj.value("presets").toArray();
            for (size_t k = 0; k < presetsArr.size(); ++k) {
                const JsonValue& presetVal = presetsArr.at(k);
                JsonObject presetObj = presetVal.toObject();

                StringTuningPreset preset;
                preset.name = kfw::trc("instruments/stringTunings", presetObj.value("name").toStdString().c_str());

                JsonArray valuesArr = presetObj.value("value").toArray();
                for (size_t l = 0; l < valuesArr.size(); ++l) {
                    const JsonValue& valueVal = valuesArr.at(l);
                    preset.value.push_back(valueVal.toInt());
                }

                if (info.number != preset.value.size()) {
                    LOGE() << "Invalid preset " << preset.name;
                    continue;
                }

                if (presetObj.contains("useFlats")) {
                    preset.useFlats = presetObj.value("useFlats").toBool();
                }

                info.presets.emplace_back(std::move(preset));
            }

            strings.emplace_back(std::move(info));
        }

        std::string id = presetInfoObj.contains("familyId") ? presetInfoObj.value("familyId").toStdString()
                         : presetInfoObj.value("instrumentId").toStdString();

        m_stringTuningsPresets.emplace(std::move(id), std::move(strings));
    }

    return true;
}

