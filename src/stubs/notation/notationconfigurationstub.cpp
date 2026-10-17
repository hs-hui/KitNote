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
#include "notationconfigurationstub.h"

using namespace kn::notation;

QColor NotationConfigurationStub::notationColor() const
{
    return QColor();
}

QColor NotationConfigurationStub::backgroundColor() const
{
    return QColor();
}

void NotationConfigurationStub::setBackgroundColor(const QColor&)
{
}

kfw::io::path_t NotationConfigurationStub::backgroundWallpaperPath() const
{
    return kfw::io::path_t();
}

const QPixmap& NotationConfigurationStub::backgroundWallpaper() const
{
    static QPixmap p;
    return p;
}

void NotationConfigurationStub::setBackgroundWallpaperPath(const kfw::io::path_t&)
{
}

bool NotationConfigurationStub::backgroundUseColor() const
{
    return false;
}

void NotationConfigurationStub::setBackgroundUseColor(bool)
{
}

void NotationConfigurationStub::resetBackground()
{
}

kfw::async::Notification NotationConfigurationStub::backgroundChanged() const
{
    static kfw::async::Notification n;
    return n;
}

QColor NotationConfigurationStub::foregroundColor() const
{
    return QColor();
}

void NotationConfigurationStub::setForegroundColor(const QColor&)
{
}

kfw::io::path_t NotationConfigurationStub::foregroundWallpaperPath() const
{
    return kfw::io::path_t();
}

const QPixmap& NotationConfigurationStub::foregroundWallpaper() const
{
    static QPixmap p;
    return p;
}

void NotationConfigurationStub::setForegroundWallpaperPath(const kfw::io::path_t&)
{
}

bool NotationConfigurationStub::foregroundUseColor() const
{
    return false;
}

void NotationConfigurationStub::setForegroundUseColor(bool)
{
}

void NotationConfigurationStub::resetForeground()
{
}

kfw::async::Notification NotationConfigurationStub::foregroundChanged() const
{
    static kfw::async::Notification n;
    return n;
}

kfw::io::path_t NotationConfigurationStub::wallpapersDefaultDirPath() const
{
    return kfw::io::path_t();
}

bool NotationConfigurationStub::shouldInvertScore() const
{
    return false;
}

bool NotationConfigurationStub::scoreInversionEnabled() const
{
    return false;
}

void NotationConfigurationStub::setScoreInversionEnabled(bool)
{
}

kfw::async::Notification NotationConfigurationStub::scoreInversionChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::isOnlyInvertInDarkTheme() const
{
    return false;
}

void NotationConfigurationStub::setOnlyInvertInDarkTheme(bool)
{
}

kfw::async::Notification NotationConfigurationStub::isOnlyInvertInDarkThemeChanged() const
{
    static kfw::async::Notification n;
    return n;
}

QColor NotationConfigurationStub::borderColor() const
{
    return QColor();
}

int NotationConfigurationStub::borderWidth() const
{
    return 1;
}

QColor NotationConfigurationStub::playbackCursorColor() const
{
    return QColor();
}

QColor NotationConfigurationStub::loopMarkerColor() const
{
    return QColor();
}

int NotationConfigurationStub::cursorOpacity() const
{
    return 1;
}

bool NotationConfigurationStub::thinNoteInputCursor() const
{
    return false;
}

QColor NotationConfigurationStub::selectionColor(engraving::voice_idx_t) const
{
    return QColor();
}

QColor NotationConfigurationStub::highlightSelectionColor(engraving::voice_idx_t) const
{
    return QColor();
}

QColor NotationConfigurationStub::dropRectColor() const
{
    return QColor();
}

kfw::draw::Color NotationConfigurationStub::noteInputPreviewColor() const
{
    return kfw::draw::Color();
}

bool NotationConfigurationStub::useNoteInputCursorInInputByDuration() const
{
    return false;
}

void NotationConfigurationStub::setUseNoteInputCursorInInputByDuration(bool)
{
}

kfw::async::Notification NotationConfigurationStub::useNoteInputCursorInInputByDurationChanged() const
{
    static kfw::async::Notification n;
    return n;
}

int NotationConfigurationStub::selectionProximity() const
{
    return 1;
}

void NotationConfigurationStub::setSelectionProximity(int)
{
}

kfw::async::Channel<int> NotationConfigurationStub::selectionProximityChanged() const
{
    static kfw::async::Channel<int> ch;
    return ch;
}

ZoomType NotationConfigurationStub::defaultZoomType() const
{
    return ZoomType::PageWidth;
}

void NotationConfigurationStub::setDefaultZoomType(ZoomType)
{
}

int NotationConfigurationStub::defaultZoom() const
{
    return 100;
}

void NotationConfigurationStub::setDefaultZoom(int)
{
}

kfw::async::Notification NotationConfigurationStub::defaultZoomChanged() const
{
    static kfw::async::Notification n;
    return n;
}

QList<int> NotationConfigurationStub::possibleZoomPercentageList() const
{
    return QList<int>();
}

qreal NotationConfigurationStub::scalingFromZoomPercentage(int) const
{
    return 0.0;
}

int NotationConfigurationStub::zoomPercentageFromScaling(qreal) const
{
    return 1;
}

int NotationConfigurationStub::mouseZoomPrecision() const
{
    return 1;
}

void NotationConfigurationStub::setMouseZoomPrecision(int)
{
}

kfw::async::Notification NotationConfigurationStub::mouseZoomPrecisionChanged() const
{
    static kfw::async::Notification n;
    return n;
}

std::string NotationConfigurationStub::fontFamily() const
{
    return std::string();
}

int NotationConfigurationStub::fontSize() const
{
    return 20;
}

kfw::io::path_t NotationConfigurationStub::userStylesPath() const
{
    return kfw::io::path_t();
}

void NotationConfigurationStub::setUserStylesPath(const kfw::io::path_t&)
{
}

kfw::async::Channel<kfw::io::path_t> NotationConfigurationStub::userStylesPathChanged() const
{
    static kfw::async::Channel<kfw::io::path_t> ch;
    return ch;
}

kfw::io::path_t NotationConfigurationStub::defaultStyleFilePath() const
{
    return kfw::io::path_t();
}

void NotationConfigurationStub::setDefaultStyleFilePath(const kfw::io::path_t&)
{
}

kfw::async::Channel<kfw::io::path_t> NotationConfigurationStub::defaultStyleFilePathChanged() const
{
    static kfw::async::Channel<kfw::io::path_t> ch;
    return ch;
}

kfw::io::path_t NotationConfigurationStub::partStyleFilePath() const
{
    return kfw::io::path_t();
}

void NotationConfigurationStub::setPartStyleFilePath(const kfw::io::path_t&)
{
}

kfw::async::Channel<kfw::io::path_t> NotationConfigurationStub::partStyleFilePathChanged() const
{
    static kfw::async::Channel<kfw::io::path_t> ch;
    return ch;
}

NoteInputMethod NotationConfigurationStub::defaultNoteInputMethod() const
{
    return NoteInputMethod::UNKNOWN;
}

void NotationConfigurationStub::setDefaultNoteInputMethod(NoteInputMethod)
{
}

kfw::async::Notification NotationConfigurationStub::defaultNoteInputMethodChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::addAccidentalDotsArticulationsToNextNoteEntered() const
{
    return false;
}

void NotationConfigurationStub::setAddAccidentalDotsArticulationsToNextNoteEntered(bool)
{
}

kfw::async::Notification NotationConfigurationStub::addAccidentalDotsArticulationsToNextNoteEnteredChanged() const
{
    static kfw::async::Notification n;
    return n;
}

kfw::io::path_t NotationConfigurationStub::userMusicFontsPath() const
{
    return kfw::io::path_t();
}

void NotationConfigurationStub::setUserMusicFontsPath(const kfw::io::path_t&)
{
}

kfw::async::Channel<kfw::io::path_t> NotationConfigurationStub::userMusicFontsPathChanged() const
{
    static kfw::async::Channel<kfw::io::path_t> ch;
    return ch;
}

bool NotationConfigurationStub::isMidiInputEnabled() const
{
    return false;
}

void NotationConfigurationStub::setIsMidiInputEnabled(bool)
{
}

kfw::async::Notification NotationConfigurationStub::isMidiInputEnabledChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::startNoteInputAtSelectedNoteRestWhenPressingMidiKey() const
{
    return false;
}

void NotationConfigurationStub::setStartNoteInputAtSelectedNoteRestWhenPressingMidiKey(bool)
{
}

kfw::async::Notification NotationConfigurationStub::startNoteInputAtSelectedNoteRestWhenPressingMidiKeyChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::isAutomaticallyPanEnabled() const
{
    return false;
}

void NotationConfigurationStub::setIsAutomaticallyPanEnabled(bool)
{
}

kfw::async::Notification NotationConfigurationStub::isAutomaticallyPanEnabledChanged() const
{
    return {};
}

bool NotationConfigurationStub::isSmoothPanning() const
{
    return false;
}

void NotationConfigurationStub::setIsSmoothPanning(bool)
{
}

kfw::async::Notification isSmoothPanningChanged() const
{
    return {};
}

bool NotationConfigurationStub::isPlayRepeatsEnabled() const
{
    return false;
}

void NotationConfigurationStub::setIsPlayRepeatsEnabled(bool)
{
}

kfw::async::Notification NotationConfigurationStub::isPlayRepeatsChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::isPlayChordSymbolsEnabled() const
{
    return false;
}

void NotationConfigurationStub::setIsPlayChordSymbolsEnabled(bool)
{
}

kfw::async::Notification NotationConfigurationStub::isPlayChordSymbolsChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::isPlayPreviewNotesInInputByDuration() const
{
    return false;
}

void NotationConfigurationStub::setIsPlayPreviewNotesInInputByDuration(bool)
{
}

kfw::async::Notification NotationConfigurationStub::isPlayPreviewNotesInInputByDurationChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::playPreviewNotesWithScoreDynamics() const
{
    return false;
}

void NotationConfigurationStub::setPlayPreviewNotesWithScoreDynamics(bool)
{
}

kfw::async::Notification NotationConfigurationStub::playPreviewNotesWithScoreDynamicsChanged() const
{
    return {};
}

bool NotationConfigurationStub::isMetronomeEnabled() const
{
    return false;
}

void NotationConfigurationStub::setIsMetronomeEnabled(bool)
{
}

kfw::async::Notification NotationConfigurationStub::isMetronomeEnabledChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::isCountInEnabled() const
{
    return false;
}

void NotationConfigurationStub::setIsCountInEnabled(bool)
{
}

double NotationConfigurationStub::guiScaling() const
{
    return 1.0;
}

double NotationConfigurationStub::notationScaling() const
{
    return 1.0;
}

kfw::ValCh<kfw::Orientation> NotationConfigurationStub::canvasOrientation() const
{
    static kfw::ValCh<kfw::Orientation> vch;
    return vch;
}

void NotationConfigurationStub::setCanvasOrientation(kfw::Orientation)
{
}

bool NotationConfigurationStub::isLimitCanvasScrollArea() const
{
    return true;
}

void NotationConfigurationStub::setIsLimitCanvasScrollArea(bool)
{
}

kfw::async::Notification NotationConfigurationStub::isLimitCanvasScrollAreaChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::colorNotesOutsideOfUsablePitchRange() const
{
    return false;
}

void NotationConfigurationStub::setColorNotesOutsideOfUsablePitchRange(bool)
{
}

kfw::async::Channel<bool> NotationConfigurationStub::colorNotesOutsideOfUsablePitchRangeChanged() const
{
    static kfw::async::Channel<bool> ch;
    return ch;
}

bool NotationConfigurationStub::warnGuitarBends() const
{
    return false;
}

void NotationConfigurationStub::setWarnGuitarBends(bool)
{
}

kfw::async::Channel<bool> NotationConfigurationStub::warnGuitarBendsChanged() const
{
    static kfw::async::Channel<bool> ch;
    return ch;
}

int NotationConfigurationStub::delayBetweenNotesInRealTimeModeMilliseconds() const
{
    return 100;
}

void NotationConfigurationStub::setDelayBetweenNotesInRealTimeModeMilliseconds(int)
{
}

kfw::async::Channel<int> NotationConfigurationStub::delayBetweenNotesInRealTimeModeMillisecondsChanged() const
{
    static kfw::async::Channel<int> ch;
    return ch;
}

bool NotationConfigurationStub::useMidiVelocityAndDurationDuringNoteInput() const
{
    return false;
}

void NotationConfigurationStub::setUseMidiVelocityAndDurationDuringNoteInput(bool)
{
}

kfw::async::Channel<bool> NotationConfigurationStub::useMidiVelocityAndDurationDuringNoteInputChanged() const
{
    return {};
}

int NotationConfigurationStub::notePlayDurationMilliseconds() const
{
    return 100;
}

void NotationConfigurationStub::setNotePlayDurationMilliseconds(int)
{
}

kfw::async::Channel<int> NotationConfigurationStub::notePlayDurationMillisecondsChanged() const
{
    static kfw::async::Channel<int> ch;
    return ch;
}

void NotationConfigurationStub::setTemplateModeEnabled(std::optional<bool>)
{
}

void NotationConfigurationStub::setTestModeEnabled(std::optional<bool>)
{
}

kfw::io::path_t NotationConfigurationStub::instrumentsXmlPath() const
{
    return kfw::io::path_t();
}

kfw::io::path_t NotationConfigurationStub::scoreOrdersXmlPath() const
{
    return kfw::io::path_t();
}

kfw::io::path_t NotationConfigurationStub::userInstrumentsFolder() const
{
    return kfw::io::path_t();
}

kfw::io::paths_t NotationConfigurationStub::userInstrumentsAndScoreOrdersPaths() const
{
    return kfw::io::paths_t();
}

void NotationConfigurationStub::setUserInstrumentsFolder(const kfw::io::path_t&)
{
}

kfw::async::Channel<kfw::io::path_t> NotationConfigurationStub::userInstrumentsFolderChanged() const
{
    static kfw::async::Channel<kfw::io::path_t> ch;
    return ch;
}

kfw::io::path_t NotationConfigurationStub::stringTuningsPresetsPath() const
{
    return kfw::io::path_t();
}

bool NotationConfigurationStub::isSnappedToGrid(kfw::Orientation) const
{
    return false;
}

void NotationConfigurationStub::setIsSnappedToGrid(kfw::Orientation, bool)
{
}

int NotationConfigurationStub::gridSizeSpatium(kfw::Orientation) const
{
    return 20;
}

void NotationConfigurationStub::setGridSize(kfw::Orientation, int)
{
}

bool NotationConfigurationStub::needToShowAddTextErrorMessage() const
{
    return false;
}

void NotationConfigurationStub::setNeedToShowAddTextErrorMessage(bool)
{
}

bool NotationConfigurationStub::needToShowAddFiguredBassErrorMessage() const
{
    return false;
}

void NotationConfigurationStub::setNeedToShowAddFiguredBassErrorMessage(bool)
{
}

bool NotationConfigurationStub::needToShowAddGuitarBendErrorMessage() const
{
    return false;
}

void NotationConfigurationStub::setNeedToShowAddGuitarBendErrorMessage(bool)
{
}

bool NotationConfigurationStub::needToShowMScoreError(const std::string&) const
{
    return false;
}

void NotationConfigurationStub::setNeedToShowMScoreError(const std::string&, bool)
{
}

kfw::ValCh<int> NotationConfigurationStub::pianoKeyboardNumberOfKeys() const
{
    static kfw::ValCh<int> vch;
    return vch;
}

void NotationConfigurationStub::setPianoKeyboardNumberOfKeys(int)
{
}

kfw::ValCh<bool> NotationConfigurationStub::midiUseWrittenPitch() const
{
    static kfw::ValCh<bool> vch;
    return vch;
}

void NotationConfigurationStub::setMidiUseWrittenPitch(bool)
{
}

bool NotationConfigurationStub::useNewPercussionPanel() const
{
    return false;
}

void NotationConfigurationStub::setUseNewPercussionPanel(bool)
{
}

kfw::async::Notification NotationConfigurationStub::useNewPercussionPanelChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::percussionPanelUseNotationPreview() const
{
    return false;
}

void NotationConfigurationStub::setPercussionPanelUseNotationPreview(bool)
{
}

kfw::async::Notification NotationConfigurationStub::percussionPanelUseNotationPreviewChanged() const
{
    static kfw::async::Notification n;
    return n;
}

PercussionPanelAutoShowMode NotationConfigurationStub::percussionPanelAutoShowMode() const
{
    return PercussionPanelAutoShowMode::UNPITCHED_STAFF;
}

void NotationConfigurationStub::setPercussionPanelAutoShowMode(PercussionPanelAutoShowMode)
{
}

kfw::async::Notification NotationConfigurationStub::percussionPanelAutoShowModeChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::autoClosePercussionPanel() const
{
    return false;
}

void NotationConfigurationStub::setAutoClosePercussionPanel(bool)
{
}

kfw::async::Notification NotationConfigurationStub::autoClosePercussionPanelChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::showPercussionPanelPadSwapDialog() const
{
    return true;
}

void NotationConfigurationStub::setShowPercussionPanelPadSwapDialog(bool)
{
}

kfw::async::Notification NotationConfigurationStub::showPercussionPanelPadSwapDialogChanged() const
{
    static kfw::async::Notification n;
    return n;
}

bool NotationConfigurationStub::percussionPanelMoveMidiNotesAndShortcuts() const
{
    return true;
}

void NotationConfigurationStub::setPercussionPanelMoveMidiNotesAndShortcuts(bool)
{
}

kfw::async::Notification NotationConfigurationStub::percussionPanelMoveMidiNotesAndShortcutsChanged() const
{
    static kfw::async::Notification n;
    return n;
}

kfw::io::path_t NotationConfigurationStub::styleFileImportPath() const
{
    return kfw::io::path_t();
}

void NotationConfigurationStub::setStyleFileImportPath(const kfw::io::path_t&)
{
}

kfw::async::Channel<std::string> NotationConfigurationStub::styleFileImportPathChanged() const
{
    static kfw::async::Channel<std::string> ch;
    return ch;
}

int NotationConfigurationStub::styleDialogLastPageIndex() const
{
    return 0;
}

void NotationConfigurationStub::setStyleDialogLastPageIndex(int)
{
}

int NotationConfigurationStub::styleDialogLastSubPageIndex() const
{
    return 0;
}

void NotationConfigurationStub::setStyleDialogLastSubPageIndex(int)
{
}

void NotationConfigurationStub::resetStyleDialogPageIndices()
{
}
