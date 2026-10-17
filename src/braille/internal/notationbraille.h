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

#ifndef KN_BRAILLE_NOTATIONBRAILLE_H
#define KN_BRAILLE_NOTATIONBRAILLE_H

#include "accessibility/iaccessibilitycontroller.h"
#include "async/asyncable.h"
#include "async/notification.h"
#include "context/iglobalcontext.h"
#include "global/iglobalconfiguration.h"
#include "ibrailleconfiguration.h"
#include "inotationbraille.h"
#include "modularity/ioc.h"
#include "notation/notationtypes.h"
#include "playback/iplaybackcontroller.h"
#include "actions/iactionsdispatcher.h"

#include "braille.h"
#include "brailleinput.h"

namespace kn::engraving {
class Score;
class Selection;

class NotationBraille : public kn::braille::INotationBraille, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<braille::IBrailleConfiguration> brailleConfiguration;
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::ContextInject<kfw::accessibility::IAccessibilityController> accessibilityController = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<playback::IPlaybackController> playbackController = { this };
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };

public:
    NotationBraille(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();
    void doBraille(bool force = false);

    bool addNote();
    bool setVoice(bool new_voice = false);
    bool addSlurStart();
    bool addSlurEnd();
    bool addTie();
    bool addSlur();
    bool addLongSlur();
    bool addTuplet(const kn::notation::TupletOptions& options);
    bool incDuration();
    bool decDuration();
    bool setArticulation();
    void setInputNoteDuration(notation::Duration d);
    void setTupletDuration(int tuplet, notation::Duration d);

    kfw::ValCh<std::string> brailleInfo() const override;
    kfw::ValCh<int> cursorPosition() const override;
    kfw::ValCh<int> currentItemPositionStart() const override;
    kfw::ValCh<int> currentItemPositionEnd() const override;
    kfw::ValCh<std::string> keys() const override;
    kfw::ValCh<bool> enabled() const override;
    kfw::ValCh<braille::BrailleIntervalDirection> intervalDirection() const override;
    kfw::ValCh<int> mode() const override;
    kfw::ValCh<std::string> cursorColor() const override;

    void setEnabled(const bool enabled) override;
    void setIntervalDirection(const braille::BrailleIntervalDirection direction) override;

    void setCursorPosition(const int pos) override;
    void setCurrentItemPosition(const int, const int) override;
    void setKeys(const QString&) override;

    void setMode(const braille::BrailleMode) override;
    void toggleMode() override;
    bool isNavigationMode() override;
    bool isBrailleInputMode() override;

    void setCursorColor(const QString color) override;

    EngravingItem* currentEngravingItem();
    Measure* currentMeasure();

    notation::INotationPtr notation();
    notation::INotationInteractionPtr interaction();

    BrailleEngravingItemList* brailleEngravingItemList();
    QString getBrailleStr();

    BrailleInputState* brailleInput();

private:
    Score* score();
    Selection* selection();

    void setBrailleInfo(const QString& info);
    void setCurrentEngravingItem(EngravingItem* el, bool select);

    void updateTableForLyricsFromPreferences();
    kfw::io::path_t tablesDefaultDirPath() const;

    IntervalDirection currentIntervalDirection();

    Measure* current_measure = nullptr;
    EngravingItem* current_engraving_item = nullptr;
    BrailleEngravingItem* current_bei = nullptr;
    BrailleEngravingItemList m_beil;
    BrailleInputState m_braille_input;

    kfw::ValCh<std::string> m_brailleInfo;
    kfw::ValCh<int> m_cursorPosition;
    kfw::ValCh<int> m_currentItemPositionStart;
    kfw::ValCh<int> m_currentItemPositionEnd;
    kfw::ValCh<std::string> m_keys;
    kfw::ValCh<bool> m_enabled;
    kfw::ValCh<int> m_mode;
    kfw::ValCh<braille::BrailleIntervalDirection> m_intervalDirection;
    kfw::ValCh<std::string> m_cursorColor;

    kfw::async::Notification m_selectionChanged;
};
}

#endif // KN_BRAILLE_NOTATIONBRAILLE_H
