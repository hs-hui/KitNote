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
#include "io/ifilesystem.h"
#include "ui/iuiconfiguration.h"
#include "engraving/iengravingconfiguration.h"

#include "../inotationconfiguration.h"

namespace kn::notation {
class NotationConfiguration : public INotationConfiguration, public kfw::async::Asyncable
{
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::GlobalInject<kfw::io::IFileSystem> fileSystem;
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::GlobalInject<engraving::IEngravingConfiguration> engravingConfiguration;

public:
    NotationConfiguration() = default;

    void init();

    QColor notationColor() const override;

    QColor backgroundColor() const override;
    void setBackgroundColor(const QColor& color) override;

    kfw::io::path_t backgroundWallpaperPath() const override;
    const QPixmap& backgroundWallpaper() const override;
    void setBackgroundWallpaperPath(const kfw::io::path_t& path) override;

    bool backgroundUseColor() const override;
    void setBackgroundUseColor(bool value) override;

    void resetBackground() override;

    kfw::async::Notification backgroundChanged() const override;

    QColor foregroundColor() const override;
    void setForegroundColor(const QColor& color) override;

    kfw::io::path_t foregroundWallpaperPath() const override;
    const QPixmap& foregroundWallpaper() const override;
    void setForegroundWallpaperPath(const kfw::io::path_t& path) override;

    bool foregroundUseColor() const override;
    void setForegroundUseColor(bool value) override;

    void resetForeground() override;

    kfw::async::Notification foregroundChanged() const override;

    kfw::io::path_t wallpapersDefaultDirPath() const override;

    bool shouldInvertScore() const override;  // Whether score should be inverted now, based on theme.

    bool scoreInversionEnabled() const override;
    void setScoreInversionEnabled(bool value) override;
    kfw::async::Notification scoreInversionChanged() const override;

    bool isOnlyInvertInDarkTheme() const override;
    void setOnlyInvertInDarkTheme(bool value) override;
    kfw::async::Notification isOnlyInvertInDarkThemeChanged() const override;

    QColor borderColor() const override;
    int borderWidth() const override;

    QColor playbackCursorColor() const override;
    QColor loopMarkerColor() const override;
    int cursorOpacity() const override;

    bool thinNoteInputCursor() const override;

    QColor selectionColor(engraving::voice_idx_t voiceIndex = 0) const override;
    QColor highlightSelectionColor(engraving::voice_idx_t voiceIndex = 0) const override;

    QColor dropRectColor() const override;

    kfw::draw::Color noteInputPreviewColor() const override;

    bool useNoteInputCursorInInputByDuration() const override;
    void setUseNoteInputCursorInInputByDuration(bool use) override;
    kfw::async::Notification useNoteInputCursorInInputByDurationChanged() const override;

    int selectionProximity() const override;
    void setSelectionProximity(int proximity) override;
    kfw::async::Channel<int> selectionProximityChanged() const override;

    ZoomType defaultZoomType() const override;
    void setDefaultZoomType(ZoomType zoomType) override;

    int defaultZoom() const override;
    void setDefaultZoom(int zoomPercentage) override;
    kfw::async::Notification defaultZoomChanged() const override;

    qreal scalingFromZoomPercentage(int zoomPercentage) const override;
    int zoomPercentageFromScaling(qreal scaling) const override;

    QList<int> possibleZoomPercentageList() const override;

    int mouseZoomPrecision() const override;
    void setMouseZoomPrecision(int precision) override;
    kfw::async::Notification mouseZoomPrecisionChanged() const override;

    std::string fontFamily() const override;
    int fontSize() const override;

    kfw::io::path_t userStylesPath() const override;
    void setUserStylesPath(const kfw::io::path_t& path) override;
    kfw::async::Channel<kfw::io::path_t> userStylesPathChanged() const override;

    kfw::io::path_t defaultStyleFilePath() const override;
    void setDefaultStyleFilePath(const kfw::io::path_t& path) override;
    kfw::async::Channel<kfw::io::path_t> defaultStyleFilePathChanged() const override;

    kfw::io::path_t partStyleFilePath() const override;
    void setPartStyleFilePath(const kfw::io::path_t& path) override;
    kfw::async::Channel<kfw::io::path_t> partStyleFilePathChanged() const override;

    NoteInputMethod defaultNoteInputMethod() const override;
    void setDefaultNoteInputMethod(NoteInputMethod method) override;
    kfw::async::Notification defaultNoteInputMethodChanged() const override;

    bool addAccidentalDotsArticulationsToNextNoteEntered() const override;
    void setAddAccidentalDotsArticulationsToNextNoteEntered(bool value) override;
    kfw::async::Notification addAccidentalDotsArticulationsToNextNoteEnteredChanged() const override;

    kfw::io::path_t userMusicFontsPath() const override;
    void setUserMusicFontsPath(const kfw::io::path_t& path) override;
    kfw::async::Channel<kfw::io::path_t> userMusicFontsPathChanged() const override;

    bool isMidiInputEnabled() const override;
    void setIsMidiInputEnabled(bool enabled) override;
    kfw::async::Notification isMidiInputEnabledChanged() const override;

    bool startNoteInputAtSelectedNoteRestWhenPressingMidiKey() const override;
    void setStartNoteInputAtSelectedNoteRestWhenPressingMidiKey(bool value) override;
    kfw::async::Notification startNoteInputAtSelectedNoteRestWhenPressingMidiKeyChanged() const override;

    bool isAutomaticallyPanEnabled() const override;
    void setIsAutomaticallyPanEnabled(bool enabled) override;
    kfw::async::Notification isAutomaticallyPanEnabledChanged() const override;

    bool isPlayRepeatsEnabled() const override;
    void setIsPlayRepeatsEnabled(bool enabled) override;
    kfw::async::Notification isPlayRepeatsChanged() const override;

    bool isPlayChordSymbolsEnabled() const override;
    void setIsPlayChordSymbolsEnabled(bool enabled) override;
    kfw::async::Notification isPlayChordSymbolsChanged() const override;

    bool isPlayPreviewNotesInInputByDuration() const override;
    void setIsPlayPreviewNotesInInputByDuration(bool play) override;
    kfw::async::Notification isPlayPreviewNotesInInputByDurationChanged() const override;

    bool playPreviewNotesWithScoreDynamics() const override;
    void setPlayPreviewNotesWithScoreDynamics(bool use) override;
    kfw::async::Notification playPreviewNotesWithScoreDynamicsChanged() const override;

    bool isMetronomeEnabled() const override;
    void setIsMetronomeEnabled(bool enabled) override;
    kfw::async::Notification isMetronomeEnabledChanged() const override;

    bool isCountInEnabled() const override;
    void setIsCountInEnabled(bool enabled) override;

    double guiScaling() const override;
    double notationScaling() const override;

    kfw::ValCh<kfw::Orientation> canvasOrientation() const override;
    void setCanvasOrientation(kfw::Orientation orientation) override;

    bool colorNotesOutsideOfUsablePitchRange() const override;
    void setColorNotesOutsideOfUsablePitchRange(bool value) override;
    kfw::async::Channel<bool> colorNotesOutsideOfUsablePitchRangeChanged() const override;

    bool warnGuitarBends() const override;
    void setWarnGuitarBends(bool value) override;
    kfw::async::Channel<bool> warnGuitarBendsChanged() const override;

    int delayBetweenNotesInRealTimeModeMilliseconds() const override;
    void setDelayBetweenNotesInRealTimeModeMilliseconds(int delayMs) override;
    kfw::async::Channel<int> delayBetweenNotesInRealTimeModeMillisecondsChanged() const override;

    bool useMidiVelocityAndDurationDuringNoteInput() const override;
    void setUseMidiVelocityAndDurationDuringNoteInput(bool use) override;
    kfw::async::Channel<bool> useMidiVelocityAndDurationDuringNoteInputChanged() const override;

    int notePlayDurationMilliseconds() const override;
    void setNotePlayDurationMilliseconds(int durationMs) override;
    kfw::async::Channel<int> notePlayDurationMillisecondsChanged() const override;

    void setTemplateModeEnabled(std::optional<bool> enabled) override;
    void setTestModeEnabled(std::optional<bool> enabled) override;

    kfw::io::path_t instrumentsXmlPath() const override;
    kfw::io::path_t scoreOrdersXmlPath() const override;

    kfw::io::path_t userInstrumentsFolder() const override;
    kfw::io::paths_t userInstrumentsAndScoreOrdersPaths() const override;
    void setUserInstrumentsFolder(const kfw::io::path_t& path) override;
    kfw::async::Channel<kfw::io::path_t> userInstrumentsFolderChanged() const override;

    kfw::io::path_t stringTuningsPresetsPath() const override;

    bool isSnappedToGrid(kfw::Orientation gridOrientation) const override;
    void setIsSnappedToGrid(kfw::Orientation gridOrientation, bool isSnapped) override;

    int gridSizeSpatium(kfw::Orientation gridOrientation) const override;
    void setGridSize(kfw::Orientation gridOrientation, int sizeSpatium) override;

    bool needToShowMScoreError(const std::string& errorKey) const override;
    void setNeedToShowMScoreError(const std::string& errorKey, bool show) override;

    kfw::ValCh<bool> midiUseWrittenPitch() const override;
    void setMidiUseWrittenPitch(bool useWrittenPitch) override;

    kfw::io::path_t styleFileImportPath() const override;
    void setStyleFileImportPath(const kfw::io::path_t& path) override;
    kfw::async::Channel<std::string> styleFileImportPathChanged() const override;

private:
    kfw::async::Notification m_backgroundChanged;
    kfw::async::Notification m_foregroundChanged;
    kfw::async::Notification m_scoreInversionChanged;
    kfw::async::Notification m_isOnlyInvertInDarkThemeChanged;

    kfw::async::Notification m_defaultNoteInputMethodChanged;
    kfw::async::Notification m_addAccidentalDotsArticulationsToNextNoteEnteredChanged;
    kfw::async::Notification m_useNoteInputCursorInInputByDurationChanged;
    kfw::async::Notification m_isMidiInputEnabledChanged;
    kfw::async::Notification m_startNoteInputAtSelectedNoteRestWhenPressingMidiKeyChanged;

    kfw::async::Notification m_defaultZoomChanged;
    kfw::async::Notification m_mouseZoomPrecisionChanged;
    kfw::async::Channel<kfw::Orientation> m_canvasOrientationChanged;
    kfw::async::Channel<kfw::io::path_t> m_userStylesPathChanged;
    kfw::async::Channel<kfw::io::path_t> m_userMusicFontsPathChanged;
    kfw::async::Channel<kfw::io::path_t> m_userInstrumentsFolderChanged;
    kfw::async::Channel<int> m_selectionProximityChanged;
    kfw::async::Channel<bool> m_colorNotesOutsideOfUsablePitchRangeChanged;
    kfw::async::Channel<bool> m_warnGuitarBendsChanged;
    kfw::async::Channel<int> m_delayBetweenNotesInRealTimeModeMillisecondsChanged;
    kfw::async::Channel<int> m_notePlayDurationMillisecondsChanged;
    kfw::async::Channel<bool> m_useMidiVelocityAndDurationDuringNoteInputChanged;
    kfw::async::Channel<std::string> m_styleFileImportPathChanged;
    kfw::async::Notification m_isPlayRepeatsChanged;
    kfw::async::Notification m_isPlayChordSymbolsChanged;
    kfw::async::Notification m_isPlayNotesPreviewInInputByDurationChanged;
    kfw::async::Notification m_playPreviewNotesWithScoreDynamicsChanged;
    kfw::async::Notification m_isMetronomeEnabledChanged;
    kfw::ValCh<bool> m_midiInputUseWrittenPitch;
    kfw::async::Channel<QColor> m_anchorColorChanged;

    kfw::async::Notification m_isAutomaticallyPanEnabledChanged;
};
}
