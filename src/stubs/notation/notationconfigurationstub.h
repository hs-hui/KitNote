/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2023 MuseScore Limited and others
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

#include "notation/inotationconfiguration.h"

namespace kn::notation {
class NotationConfigurationStub : public INotationConfiguration
{
public:
    NotationConfigurationStub() = default;

    QColor notationColor() const override;

    QColor backgroundColor() const override;
    void setBackgroundColor(const QColor& color)  override;

    kfw::io::path_t backgroundWallpaperPath() const override;
    const QPixmap& backgroundWallpaper() const override;
    void setBackgroundWallpaperPath(const kfw::io::path_t& path)  override;

    bool backgroundUseColor() const override;
    void setBackgroundUseColor(bool value)  override;

    void resetBackground()  override;

    kfw::async::Notification backgroundChanged() const override;

    QColor foregroundColor() const override;
    void setForegroundColor(const QColor& color)  override;

    kfw::io::path_t foregroundWallpaperPath() const override;
    const QPixmap& foregroundWallpaper() const override;
    void setForegroundWallpaperPath(const kfw::io::path_t& path)  override;

    bool foregroundUseColor() const override;
    void setForegroundUseColor(bool value)  override;

    void resetForeground()  override;

    kfw::async::Notification foregroundChanged() const override;

    kfw::io::path_t wallpapersDefaultDirPath() const override;

    bool shouldInvertScore() const override;

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
    void setSelectionProximity(int proximity)  override;
    kfw::async::Channel<int> selectionProximityChanged() const override;

    ZoomType defaultZoomType() const override;
    void setDefaultZoomType(ZoomType zoomType)  override;

    int defaultZoom() const override;
    void setDefaultZoom(int zoomPercentage)  override;

    kfw::async::Notification defaultZoomChanged() const override;

    QList<int> possibleZoomPercentageList() const override;

    qreal scalingFromZoomPercentage(int zoomPercentage) const override;
    int zoomPercentageFromScaling(qreal scaling) const override;

    int mouseZoomPrecision() const override;
    void setMouseZoomPrecision(int precision)  override;
    kfw::async::Notification mouseZoomPrecisionChanged() const override;

    std::string fontFamily() const override;
    int fontSize() const override;

    kfw::io::path_t userStylesPath() const override;
    void setUserStylesPath(const kfw::io::path_t& path)  override;
    kfw::async::Channel<kfw::io::path_t> userStylesPathChanged() const override;

    kfw::io::path_t defaultStyleFilePath() const override;
    void setDefaultStyleFilePath(const kfw::io::path_t& path)  override;
    kfw::async::Channel<kfw::io::path_t> defaultStyleFilePathChanged() const override;

    kfw::io::path_t partStyleFilePath() const override;
    void setPartStyleFilePath(const kfw::io::path_t& path)  override;
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
    void setIsMidiInputEnabled(bool enabled)  override;
    kfw::async::Notification isMidiInputEnabledChanged() const override;

    bool startNoteInputAtSelectedNoteRestWhenPressingMidiKey() const override;
    void setStartNoteInputAtSelectedNoteRestWhenPressingMidiKey(bool value) override;
    kfw::async::Notification startNoteInputAtSelectedNoteRestWhenPressingMidiKeyChanged() const override;

    bool isAutomaticallyPanEnabled() const override;
    void setIsAutomaticallyPanEnabled(bool enabled)  override;
    kfw::async::Notification isAutomaticallyPanEnabledChanged() const override;

    bool isSmoothPanning() const override;
    void setIsSmoothPanning(bool value) override;
    kfw::async::Notification isSmoothPanningChanged() const override;

    bool isPlayRepeatsEnabled() const override;
    void setIsPlayRepeatsEnabled(bool enabled)  override;
    kfw::async::Notification isPlayRepeatsChanged() const override;

    bool isPlayChordSymbolsEnabled() const override;
    void setIsPlayChordSymbolsEnabled(bool enabled)  override;
    kfw::async::Notification isPlayChordSymbolsChanged() const override;

    bool isPlayPreviewNotesInInputByDuration() const override;
    void setIsPlayPreviewNotesInInputByDuration(bool play) override;
    kfw::async::Notification isPlayPreviewNotesInInputByDurationChanged() const override;

    bool playPreviewNotesWithScoreDynamics() const override;
    void setPlayPreviewNotesWithScoreDynamics(bool use) override;
    kfw::async::Notification playPreviewNotesWithScoreDynamicsChanged() const override;

    bool isMetronomeEnabled() const override;
    void setIsMetronomeEnabled(bool enabled)  override;
    kfw::async::Notification isMetronomeEnabledChanged() const override;

    bool isCountInEnabled() const override;
    void setIsCountInEnabled(bool enabled)  override;

    double guiScaling() const override;
    double notationScaling() const override;

    kfw::ValCh<kfw::Orientation> canvasOrientation() const override;
    void setCanvasOrientation(kfw::Orientation orientation)  override;

    bool isLimitCanvasScrollArea() const override;
    void setIsLimitCanvasScrollArea(bool limited)  override;
    kfw::async::Notification isLimitCanvasScrollAreaChanged() const override;

    bool colorNotesOutsideOfUsablePitchRange() const override;
    void setColorNotesOutsideOfUsablePitchRange(bool value)  override;
    kfw::async::Channel<bool> colorNotesOutsideOfUsablePitchRangeChanged() const override;

    bool warnGuitarBends() const override;
    void setWarnGuitarBends(bool value) override;
    kfw::async::Channel<bool> warnGuitarBendsChanged() const override;

    int delayBetweenNotesInRealTimeModeMilliseconds() const override;
    void setDelayBetweenNotesInRealTimeModeMilliseconds(int delayMs)  override;
    kfw::async::Channel<int> delayBetweenNotesInRealTimeModeMillisecondsChanged() const override;

    bool useMidiVelocityAndDurationDuringNoteInput() const override;
    void setUseMidiVelocityAndDurationDuringNoteInput(bool use) override;
    kfw::async::Channel<bool> useMidiVelocityAndDurationDuringNoteInputChanged() const override;

    int notePlayDurationMilliseconds() const override;
    void setNotePlayDurationMilliseconds(int durationMs)  override;
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
    void setIsSnappedToGrid(kfw::Orientation gridOrientation, bool isSnapped)  override;

    int gridSizeSpatium(kfw::Orientation gridOrientation) const override;
    void setGridSize(kfw::Orientation gridOrientation, int sizeSpatium)  override;

    bool needToShowAddTextErrorMessage() const override;
    void setNeedToShowAddTextErrorMessage(bool show)  override;

    bool needToShowAddFiguredBassErrorMessage() const override;
    void setNeedToShowAddFiguredBassErrorMessage(bool show)  override;

    bool needToShowAddGuitarBendErrorMessage() const override;
    void setNeedToShowAddGuitarBendErrorMessage(bool show) override;

    bool needToShowMScoreError(const std::string& errorKey) const override;
    void setNeedToShowMScoreError(const std::string& errorKey, bool show)  override;

    kfw::ValCh<int> pianoKeyboardNumberOfKeys() const override;
    void setPianoKeyboardNumberOfKeys(int number)  override;

    kfw::ValCh<bool> midiUseWrittenPitch() const override;
    void setMidiUseWrittenPitch(bool value)  override;

    bool useNewPercussionPanel() const override;
    void setUseNewPercussionPanel(bool use) override;
    kfw::async::Notification useNewPercussionPanelChanged() const override;

    bool percussionPanelUseNotationPreview() const override;
    void setPercussionPanelUseNotationPreview(bool use) override;
    kfw::async::Notification percussionPanelUseNotationPreviewChanged() const override;

    PercussionPanelAutoShowMode percussionPanelAutoShowMode() const override;
    void setPercussionPanelAutoShowMode(PercussionPanelAutoShowMode autoShowMode) override;
    kfw::async::Notification percussionPanelAutoShowModeChanged() const override;

    bool autoClosePercussionPanel() const override;
    void setAutoClosePercussionPanel(bool autoClose) override;
    kfw::async::Notification autoClosePercussionPanelChanged() const override;

    bool showPercussionPanelPadSwapDialog() const override;
    void setShowPercussionPanelPadSwapDialog(bool show) override;
    kfw::async::Notification showPercussionPanelPadSwapDialogChanged() const override;

    bool percussionPanelMoveMidiNotesAndShortcuts() const override;
    void setPercussionPanelMoveMidiNotesAndShortcuts(bool move) override;
    kfw::async::Notification percussionPanelMoveMidiNotesAndShortcutsChanged() const override;

    kfw::io::path_t styleFileImportPath() const override;
    void setStyleFileImportPath(const kfw::io::path_t& path)  override;
    kfw::async::Channel<std::string> styleFileImportPathChanged() const override;

    int styleDialogLastPageIndex() const override;
    void setStyleDialogLastPageIndex(int value) override;

    int styleDialogLastSubPageIndex() const override;
    void setStyleDialogLastSubPageIndex(int value) override;

    void resetStyleDialogPageIndices() override;
};
}
