/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2026 MuseScore Limited and others
 * Modified for KitNote, 2026
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "async/asyncable.h"
#include "context/iglobalcontext.h"
#include "modularity/ioc.h"

#include "inotationsceneconfiguration.h"

namespace kn::notation {
class NotationSceneConfiguration : public INotationSceneConfiguration, public kfw::async::Asyncable, public kfw::Contextable
{
    kfw::ContextInject<context::IGlobalContext> context = { this };

public:
    explicit NotationSceneConfiguration(const kfw::modularity::ContextPtr& ctx);

    void init();

    bool isSmoothPanning() const override;
    void setIsSmoothPanning(bool value) override;
    kfw::async::Notification isSmoothPanningChanged() const override;

    bool isLimitCanvasScrollArea() const override;
    void setIsLimitCanvasScrollArea(bool limited) override;
    kfw::async::Notification isLimitCanvasScrollAreaChanged() const override;

    kfw::ValCh<int> pianoKeyboardNumberOfKeys() const override;
    void setPianoKeyboardNumberOfKeys(int number) override;

    bool useNewPercussionPanel() const override;
    void setUseNewPercussionPanel(bool use) override;
    kfw::async::Notification useNewPercussionPanelChanged() const override;

    bool percussionPanelUseNotationPreview() const override;
    void setPercussionPanelUseNotationPreview(bool use) override;
    kfw::async::Notification percussionPanelUseNotationPreviewChanged() const override;

    PercussionPanelAutoShowMode percussionPanelAutoShowMode() const override;
    void setPercussionPanelAutoShowMode(PercussionPanelAutoShowMode percussionPanelAutoShowMode) override;
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

    int styleDialogLastPageIndex() const override;
    void setStyleDialogLastPageIndex(int value) override;

    int styleDialogLastSubPageIndex() const override;
    void setStyleDialogLastSubPageIndex(int value) override;

    void resetStyleDialogPageIndices() override;

private:
    kfw::async::Notification m_isSmoothPanningChanged;
    kfw::async::Notification m_isLimitCanvasScrollAreaChanged;
    kfw::ValCh<int> m_pianoKeyboardNumberOfKeys;
    kfw::async::Notification m_useNewPercussionPanelChanged;
    kfw::async::Notification m_percussionPanelUseNotationPreviewChanged;
    kfw::async::Notification m_percussionPanelAutoShowModeChanged;
    kfw::async::Notification m_autoClosePercussionPanelChanged;
    kfw::async::Notification m_showPercussionPanelPadSwapDialogChanged;
    kfw::async::Notification m_percussionPanelMoveMidiNotesAndShortcutsChanged;

    int m_styleDialogLastPageIndex = 0;
    int m_styleDialogLastSubPageIndex = 0;
};
}
