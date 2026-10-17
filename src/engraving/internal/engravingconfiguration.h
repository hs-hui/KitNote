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

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "global/iglobalconfiguration.h"
#include "ui/iuiconfiguration.h"
#include "accessibility/iaccessibilityconfiguration.h"
#include "importexport/guitarpro/iguitarproconfiguration.h"

#include "../iengravingconfiguration.h"

namespace kn::engraving {
class EngravingConfiguration : public IEngravingConfiguration, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::GlobalInject<kfw::accessibility::IAccessibilityConfiguration> accessibilityConfiguration;
    kfw::GlobalInject<iex::guitarpro::IGuitarProConfiguration> guitarProConfiguration;

public:
    EngravingConfiguration(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();

    kfw::io::path_t appDataPath() const override;

    kfw::io::path_t defaultStyleFilePath() const override;
    void setDefaultStyleFilePath(const kfw::io::path_t& path) override;
    kfw::async::Channel<kfw::io::path_t> defaultStyleFilePathChanged() const override;

    kfw::io::path_t partStyleFilePath() const override;
    void setPartStyleFilePath(const kfw::io::path_t& path) override;
    kfw::async::Channel<kfw::io::path_t> partStyleFilePathChanged() const override;

    SizeF defaultPageSize() const override;

    bool canLayoutIcons() const override;
    String iconsFontFamily() const override;

    Color defaultColor() const override;
    Color scoreInversionColor() const override;
    Color lassoColor() const override;
    Color warningColor() const override;
    Color warningSelectedColor() const override;
    Color criticalColor() const override;
    Color criticalBackgroundColor() const override;
    Color criticalSelectedColor() const override;
    Color thumbnailBackgroundColor() const override;
    Color noteBackgroundColor() const override;
    Color fontPrimaryColor() const override;
    Color voiceColor(voice_idx_t voiceIdx) const override;

    double guiScaling() const override;

    Color selectionColor(voice_idx_t voiceIndex = 0, bool itemVisible = true, bool itemIsUnlinkedFromScore = false) const override;
    void setSelectionColor(voice_idx_t voiceIndex, Color color) override;
    kfw::async::Channel<voice_idx_t, Color> selectionColorChanged() const override;

    Color highlightSelectionColor(voice_idx_t voice = 0) const override;

    bool dynamicsApplyToAllVoices() const override;
    void setDynamicsApplyToAllVoices(bool v) override;
    kfw::async::Channel<bool> dynamicsApplyToAllVoicesChanged() const override;

    bool autoUpdateFretboardDiagrams() const override;
    void setAutoUpdateFretboardDiagrams(bool v) override;
    kfw::async::Channel<bool> autoUpdateFretboardDiagramsChanged() const override;

    Color formattingColor() const override;
    kfw::async::Channel<Color> formattingColorChanged() const override;

    Color frameColor() const override;
    kfw::async::Channel<Color> frameColorChanged() const override;

    Color scoreGreyColor() const override;

    Color invisibleColor() const override;
    kfw::async::Channel<Color> invisibleColorChanged() const override;

    Color unlinkedColor() const override;
    kfw::async::Channel<Color> unlinkedColorChanged() const override;

    const DebuggingOptions& debuggingOptions() const override;
    void setDebuggingOptions(const DebuggingOptions& options) override;
    kfw::async::Notification debuggingOptionsChanged() const override;

    bool isAccessibleEnabled() const override;

    bool doNotSaveEIDsForBackCompat() const override;
    void setDoNotSaveEIDsForBackCompat(bool doNotSave) override;

    bool allowReadingImagesFromOutsideMscz() const override;

    bool guitarProImportExperimental() const override;
    bool negativeFretsAllowed() const override;
    void setGuitarProMultivoiceEnabled(bool multiVoice) override;
    bool guitarProMultivoiceEnabled() const override;
    bool minDistanceForPartialSkylineCalculated() const override;
    bool specificSlursLayoutWorkaround() const override;
    bool preferSameStringForTranspose() const override;
    void setPreferSameStringForTranspose(bool preferSameString) override;

    int maxScaledImageDim() const override;
    void setMaxScaledImageDim(int maxDim) override;

private:
    int m_maxScaledImageDim = 4096;
    kfw::async::Channel<voice_idx_t, Color> m_voiceColorChanged;
    kfw::async::Channel<bool> m_dynamicsApplyToAllVoicesChanged;
    kfw::async::Channel<bool> m_fretboardDiagramsAutoUpdateChanged;
    kfw::async::Channel<Color> m_formattingColorChanged;
    kfw::async::Channel<Color> m_frameColorChanged;
    kfw::async::Channel<Color> m_invisibleColorChanged;
    kfw::async::Channel<Color> m_unlinkedColorChanged;
    kfw::async::Channel<kfw::io::path_t> m_defaultStyleFilePathChanged;
    kfw::async::Channel<kfw::io::path_t> m_partStyleFilePathChanged;

    kfw::ValNt<DebuggingOptions> m_debuggingOptions;

    bool m_multiVoice = false;
};
}
