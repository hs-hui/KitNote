/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2024 MuseScore Limited and others
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

#include "notation/inotationconfiguration.h"

namespace kn::notation {
class NotationConfigurationMock : public INotationConfiguration
{
public:
    MOCK_METHOD(QColor, notationColor, (), (const, override));

    MOCK_METHOD(QColor, backgroundColor, (), (const, override));
    MOCK_METHOD(void, setBackgroundColor, (const QColor& color), (override));

    MOCK_METHOD(kfw::io::path_t, backgroundWallpaperPath, (), (const, override));
    MOCK_METHOD(const QPixmap&, backgroundWallpaper, (), (const, override));
    MOCK_METHOD(void, setBackgroundWallpaperPath, (const kfw::io::path_t& path), (override));

    MOCK_METHOD(bool, backgroundUseColor, (), (const, override));
    MOCK_METHOD(void, setBackgroundUseColor, (bool), (override));

    MOCK_METHOD(void, resetBackground, (), (override));

    MOCK_METHOD(kfw::async::Notification, backgroundChanged, (), (const, override));

    MOCK_METHOD(QColor, foregroundColor, (), (const, override));
    MOCK_METHOD(void, setForegroundColor, (const QColor&), (override));

    MOCK_METHOD(kfw::io::path_t, foregroundWallpaperPath, (), (const, override));
    MOCK_METHOD(const QPixmap&, foregroundWallpaper, (), (const, override));
    MOCK_METHOD(void, setForegroundWallpaperPath, (const kfw::io::path_t&), (override));

    MOCK_METHOD(bool, foregroundUseColor, (), (const, override));
    MOCK_METHOD(void, setForegroundUseColor, (bool), (override));

    MOCK_METHOD(void, resetForeground, (), (override));

    MOCK_METHOD(kfw::async::Notification, foregroundChanged, (), (const, override));

    MOCK_METHOD(kfw::io::path_t, wallpapersDefaultDirPath, (), (const, override));

    MOCK_METHOD(bool, shouldInvertScore, (), (const, override));

    MOCK_METHOD(bool, scoreInversionEnabled, (), (const, override));
    MOCK_METHOD(void, setScoreInversionEnabled, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, scoreInversionChanged, (), (const, override));

    MOCK_METHOD(bool, isOnlyInvertInDarkTheme, (), (const, override));
    MOCK_METHOD(void, setOnlyInvertInDarkTheme, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, isOnlyInvertInDarkThemeChanged, (), (const, override));

    MOCK_METHOD(QColor, borderColor, (), (const, override));
    MOCK_METHOD(int, borderWidth, (), (const, override));

    MOCK_METHOD(QColor, playbackCursorColor, (), (const, override));
    MOCK_METHOD(QColor, loopMarkerColor, (), (const, override));
    MOCK_METHOD(int, cursorOpacity, (), (const, override));

    MOCK_METHOD(bool, thinNoteInputCursor, (), (const, override));

    MOCK_METHOD(QColor, selectionColor, (engraving::voice_idx_t), (const, override));
    MOCK_METHOD(QColor, highlightSelectionColor, (engraving::voice_idx_t), (const, override));

    MOCK_METHOD(QColor, dropRectColor, (), (const, override));

    MOCK_METHOD(kfw::draw::Color, noteInputPreviewColor, (), (const, override));

    MOCK_METHOD(bool, useNoteInputCursorInInputByDuration, (), (const, override));
    MOCK_METHOD(void, setUseNoteInputCursorInInputByDuration, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, useNoteInputCursorInInputByDurationChanged, (), (const, override));

    MOCK_METHOD(int, selectionProximity, (), (const, override));
    MOCK_METHOD(void, setSelectionProximity, (int), (override));
    MOCK_METHOD(kfw::async::Channel<int>, selectionProximityChanged, (), (const, override));

    MOCK_METHOD(ZoomType, defaultZoomType, (), (const, override));
    MOCK_METHOD(void, setDefaultZoomType, (ZoomType), (override));

    MOCK_METHOD(int, defaultZoom, (), (const, override));
    MOCK_METHOD(void, setDefaultZoom, (int), (override));
    MOCK_METHOD(kfw::async::Notification, defaultZoomChanged, (), (const, override));

    MOCK_METHOD(QList<int>, possibleZoomPercentageList, (), (const, override));

    MOCK_METHOD(qreal, scalingFromZoomPercentage, (int), (const, override));
    MOCK_METHOD(int, zoomPercentageFromScaling, (qreal), (const, override));

    MOCK_METHOD(int, mouseZoomPrecision, (), (const, override));
    MOCK_METHOD(void, setMouseZoomPrecision, (int), (override));
    MOCK_METHOD(kfw::async::Notification, mouseZoomPrecisionChanged, (), (const, override));

    MOCK_METHOD(std::string, fontFamily, (), (const, override));
    MOCK_METHOD(int, fontSize, (), (const, override));

    MOCK_METHOD(kfw::io::path_t, userStylesPath, (), (const, override));
    MOCK_METHOD(void, setUserStylesPath, (const kfw::io::path_t&), (override));
    MOCK_METHOD(kfw::async::Channel<kfw::io::path_t>, userStylesPathChanged, (), (const, override));

    MOCK_METHOD(kfw::io::path_t, defaultStyleFilePath, (), (const, override));
    MOCK_METHOD(void, setDefaultStyleFilePath, (const kfw::io::path_t&), (override));
    MOCK_METHOD(kfw::async::Channel<kfw::io::path_t>, defaultStyleFilePathChanged, (), (const, override));

    MOCK_METHOD(kfw::io::path_t, partStyleFilePath, (), (const, override));
    MOCK_METHOD(void, setPartStyleFilePath, (const kfw::io::path_t&), (override));
    MOCK_METHOD(kfw::async::Channel<kfw::io::path_t>, partStyleFilePathChanged, (), (const, override));

    MOCK_METHOD(NoteInputMethod, defaultNoteInputMethod, (), (const, override));
    MOCK_METHOD(void, setDefaultNoteInputMethod, (NoteInputMethod), (override));
    MOCK_METHOD(kfw::async::Notification, defaultNoteInputMethodChanged, (), (const, override));

    MOCK_METHOD(bool, addAccidentalDotsArticulationsToNextNoteEntered, (), (const, override));
    MOCK_METHOD(void, setAddAccidentalDotsArticulationsToNextNoteEntered, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, addAccidentalDotsArticulationsToNextNoteEnteredChanged, (), (const, override));

    MOCK_METHOD(kfw::io::path_t, userMusicFontsPath, (), (const, override));
    MOCK_METHOD(void, setUserMusicFontsPath, (const kfw::io::path_t&), (override));
    MOCK_METHOD(kfw::async::Channel<kfw::io::path_t>, userMusicFontsPathChanged, (), (const, override));

    MOCK_METHOD(bool, isMidiInputEnabled, (), (const, override));
    MOCK_METHOD(void, setIsMidiInputEnabled, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, isMidiInputEnabledChanged, (), (const, override));

    MOCK_METHOD(bool, startNoteInputAtSelectedNoteRestWhenPressingMidiKey, (), (const, override));
    MOCK_METHOD(void, setStartNoteInputAtSelectedNoteRestWhenPressingMidiKey, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, startNoteInputAtSelectedNoteRestWhenPressingMidiKeyChanged, (), (const, override));

    MOCK_METHOD(bool, isAutomaticallyPanEnabled, (), (const, override));
    MOCK_METHOD(void, setIsAutomaticallyPanEnabled, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, isAutomaticallyPanEnabledChanged, (), (const, override));

    MOCK_METHOD(bool, isPlayRepeatsEnabled, (), (const, override));
    MOCK_METHOD(void, setIsPlayRepeatsEnabled, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, isPlayRepeatsChanged, (), (const, override));

    MOCK_METHOD(bool, isPlayChordSymbolsEnabled, (), (const, override));
    MOCK_METHOD(void, setIsPlayChordSymbolsEnabled, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, isPlayChordSymbolsChanged, (), (const, override));

    MOCK_METHOD(bool, isPlayPreviewNotesInInputByDuration, (), (const, override));
    MOCK_METHOD(void, setIsPlayPreviewNotesInInputByDuration, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, isPlayPreviewNotesInInputByDurationChanged, (), (const, override));

    MOCK_METHOD(bool, playPreviewNotesWithScoreDynamics, (), (const, override));
    MOCK_METHOD(void, setPlayPreviewNotesWithScoreDynamics, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, playPreviewNotesWithScoreDynamicsChanged, (), (const, override));

    MOCK_METHOD(bool, isMetronomeEnabled, (), (const, override));
    MOCK_METHOD(void, setIsMetronomeEnabled, (bool), (override));
    MOCK_METHOD(kfw::async::Notification, isMetronomeEnabledChanged, (), (const, override));

    MOCK_METHOD(bool, isCountInEnabled, (), (const, override));
    MOCK_METHOD(void, setIsCountInEnabled, (bool), (override));

    MOCK_METHOD(double, guiScaling, (), (const, override));
    MOCK_METHOD(double, notationScaling, (), (const, override));

    MOCK_METHOD(kfw::ValCh<kfw::Orientation>, canvasOrientation, (), (const, override));
    MOCK_METHOD(void, setCanvasOrientation, (kfw::Orientation), (override));

    MOCK_METHOD(bool, colorNotesOutsideOfUsablePitchRange, (), (const, override));
    MOCK_METHOD(void, setColorNotesOutsideOfUsablePitchRange, (bool), (override));
    MOCK_METHOD((kfw::async::Channel<bool>), colorNotesOutsideOfUsablePitchRangeChanged, (), (const, override));

    MOCK_METHOD(bool, warnGuitarBends, (), (const, override));
    MOCK_METHOD(void, setWarnGuitarBends, (bool), (override));
    MOCK_METHOD((kfw::async::Channel<bool>), warnGuitarBendsChanged, (), (const, override));

    MOCK_METHOD(int, delayBetweenNotesInRealTimeModeMilliseconds, (), (const, override));
    MOCK_METHOD(void, setDelayBetweenNotesInRealTimeModeMilliseconds, (int), (override));
    MOCK_METHOD((kfw::async::Channel<int>), delayBetweenNotesInRealTimeModeMillisecondsChanged, (), (const, override));

    MOCK_METHOD(bool, useMidiVelocityAndDurationDuringNoteInput, (), (const, override));
    MOCK_METHOD(void, setUseMidiVelocityAndDurationDuringNoteInput, (bool), (override));
    MOCK_METHOD(kfw::async::Channel<bool>, useMidiVelocityAndDurationDuringNoteInputChanged, (), (const, override));

    MOCK_METHOD(int, notePlayDurationMilliseconds, (), (const, override));
    MOCK_METHOD(void, setNotePlayDurationMilliseconds, (int), (override));
    MOCK_METHOD((kfw::async::Channel<int>), notePlayDurationMillisecondsChanged, (), (const, override));

    MOCK_METHOD(void, setTemplateModeEnabled, (std::optional<bool>), (override));
    MOCK_METHOD(void, setTestModeEnabled, (std::optional<bool>), (override));

    MOCK_METHOD(kfw::io::path_t, instrumentsXmlPath, (), (const, override));
    MOCK_METHOD(kfw::io::path_t, scoreOrdersXmlPath, (), (const, override));

    MOCK_METHOD(kfw::io::path_t, userInstrumentsFolder, (), (const, override));
    MOCK_METHOD(kfw::io::paths_t, userInstrumentsAndScoreOrdersPaths, (), (const, override));
    MOCK_METHOD(void, setUserInstrumentsFolder, (const kfw::io::path_t&), (override));
    MOCK_METHOD(kfw::async::Channel<kfw::io::path_t>, userInstrumentsFolderChanged, (), (const, override));

    MOCK_METHOD(kfw::io::path_t, stringTuningsPresetsPath, (), (const, override));

    MOCK_METHOD(bool, isSnappedToGrid, (kfw::Orientation), (const, override));
    MOCK_METHOD(void, setIsSnappedToGrid, (kfw::Orientation, bool), (override));

    MOCK_METHOD(int, gridSizeSpatium, (kfw::Orientation), (const, override));
    MOCK_METHOD(void, setGridSize, (kfw::Orientation, int), (override));

    MOCK_METHOD(bool, needToShowMScoreError, (const std::string&), (const, override));
    MOCK_METHOD(void, setNeedToShowMScoreError, (const std::string&, bool), (override));

    MOCK_METHOD(kfw::ValCh<bool>, midiUseWrittenPitch, (), (const, override));
    MOCK_METHOD(void, setMidiUseWrittenPitch, (bool), (override));

    MOCK_METHOD(kfw::io::path_t, styleFileImportPath, (), (const, override));
    MOCK_METHOD(void, setStyleFileImportPath, (const kfw::io::path_t&), (override));
    MOCK_METHOD((kfw::async::Channel<std::string>), styleFileImportPathChanged, (), (const, override));
};
}
