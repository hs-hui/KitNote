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

#include <gmock/gmock.h>

#include "engraving/iengravingconfiguration.h"

namespace kn::engraving {
class EngravingConfigurationMock : public IEngravingConfiguration
{
public:
    MOCK_METHOD(kfw::io::path_t, appDataPath, (), (const, override));

    MOCK_METHOD(kfw::io::path_t, defaultStyleFilePath, (), (const, override));
    MOCK_METHOD(void, setDefaultStyleFilePath, (const kfw::io::path_t&), (override));
    MOCK_METHOD(kfw::async::Channel<kfw::io::path_t>, defaultStyleFilePathChanged, (), (const, override));

    MOCK_METHOD(kfw::io::path_t, partStyleFilePath, (), (const, override));
    MOCK_METHOD(void, setPartStyleFilePath, (const kfw::io::path_t&), (override));
    MOCK_METHOD(kfw::async::Channel<kfw::io::path_t>, partStyleFilePathChanged, (), (const, override));

    MOCK_METHOD(SizeF, defaultPageSize, (), (const, override));

    MOCK_METHOD(bool, canLayoutIcons, (), (const, override));
    MOCK_METHOD(String, iconsFontFamily, (), (const, override));

    MOCK_METHOD(Color, defaultColor, (), (const, override));
    MOCK_METHOD(Color, scoreInversionColor, (), (const, override));
    MOCK_METHOD(Color, lassoColor, (), (const, override));
    MOCK_METHOD(Color, warningColor, (), (const, override));
    MOCK_METHOD(Color, warningSelectedColor, (), (const, override));
    MOCK_METHOD(Color, criticalColor, (), (const, override));
    MOCK_METHOD(Color, criticalBackgroundColor, (), (const, override));
    MOCK_METHOD(Color, criticalSelectedColor, (), (const, override));
    MOCK_METHOD(Color, thumbnailBackgroundColor, (), (const, override));
    MOCK_METHOD(Color, noteBackgroundColor, (), (const, override));
    MOCK_METHOD(Color, fontPrimaryColor, (), (const, override));
    MOCK_METHOD(Color, voiceColor, (voice_idx_t), (const, override));

    MOCK_METHOD(double, guiScaling, (), (const, override));

    MOCK_METHOD(Color, selectionColor, (engraving::voice_idx_t, bool, bool), (const, override));
    MOCK_METHOD(void, setSelectionColor, (engraving::voice_idx_t, Color), (override));
    MOCK_METHOD((kfw::async::Channel<engraving::voice_idx_t, Color>), selectionColorChanged, (), (const, override));

    MOCK_METHOD(bool, dynamicsApplyToAllVoices, (), (const, override));
    MOCK_METHOD(void, setDynamicsApplyToAllVoices, (bool), (override));
    MOCK_METHOD((kfw::async::Channel<bool>), dynamicsApplyToAllVoicesChanged, (), (const, override));

    MOCK_METHOD(bool, autoUpdateFretboardDiagrams, (), (const, override));
    MOCK_METHOD(void, setAutoUpdateFretboardDiagrams, (bool), (override));
    MOCK_METHOD((kfw::async::Channel<bool>), autoUpdateFretboardDiagramsChanged, (), (const, override));

    MOCK_METHOD(Color, formattingColor, (), (const, override));
    MOCK_METHOD(kfw::async::Channel<Color>, formattingColorChanged, (), (const, override));

    MOCK_METHOD(Color, frameColor, (), (const, override));
    MOCK_METHOD(kfw::async::Channel<Color>, frameColorChanged, (), (const, override));

    MOCK_METHOD(Color, scoreGreyColor, (), (const, override));

    MOCK_METHOD(Color, invisibleColor, (), (const, override));
    MOCK_METHOD(kfw::async::Channel<Color>, invisibleColorChanged, (), (const, override));

    MOCK_METHOD(Color, unlinkedColor, (), (const, override));
    MOCK_METHOD(kfw::async::Channel<Color>, unlinkedColorChanged, (), (const, override));

    MOCK_METHOD(Color, highlightSelectionColor, (engraving::voice_idx_t), (const, override));

    MOCK_METHOD(const DebuggingOptions&, debuggingOptions, (), (const, override));
    MOCK_METHOD(void, setDebuggingOptions, (const DebuggingOptions&), (override));
    MOCK_METHOD(kfw::async::Notification, debuggingOptionsChanged, (), (const, override));

    MOCK_METHOD(bool, isAccessibleEnabled, (), (const, override));

    MOCK_METHOD(bool, doNotSaveEIDsForBackCompat, (), (const, override));
    MOCK_METHOD(void, setDoNotSaveEIDsForBackCompat, (bool), (override));

    MOCK_METHOD(bool, allowReadingImagesFromOutsideMscz, (), (const, override));

    MOCK_METHOD(bool, guitarProImportExperimental, (), (const, override));
    MOCK_METHOD(bool, negativeFretsAllowed, (), (const, override));
    MOCK_METHOD(void, setGuitarProMultivoiceEnabled, (bool), (override));
    MOCK_METHOD(bool, guitarProMultivoiceEnabled, (), (const, override));
    MOCK_METHOD(bool, minDistanceForPartialSkylineCalculated, (), (const, override));
    MOCK_METHOD(bool, specificSlursLayoutWorkaround, (), (const, override));
    MOCK_METHOD(bool, preferSameStringForTranspose, (), (const, override));
    MOCK_METHOD(void, setPreferSameStringForTranspose, (bool), (override));

    MOCK_METHOD(int, maxScaledImageDim, (), (const, override));
    MOCK_METHOD(void, setMaxScaledImageDim, (int), (override));
};
}
