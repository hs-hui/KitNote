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

#include <QtEvents>

#include "modularity/ioc.h"

#include "actions/iactionsdispatcher.h"
#include "actions/actionable.h"
#include "async/asyncable.h"

#include "context/iglobalcontext.h"

#include "notation/inotationinteraction.h"
#include "notation/inotationplayback.h"
#include "notation/inotationconfiguration.h"

#include "playback/iplaybackcontroller.h"

#include "global/iglobalconfiguration.h"
#include "ui/idragcontroller.h"
#include "ui/iuiconfiguration.h"

#include "abstractelementpopupmodel.h"

class QQuickItem;

namespace kn::notation {
class IControlledView
{
public:
    virtual ~IControlledView() = default;

    virtual qreal width() const = 0;
    virtual qreal height() const = 0;

    virtual kfw::PointF viewportTopLeft() const = 0;

    //! kfw::Returns true if the canvas has been moved
    virtual bool moveCanvas(qreal dx, qreal dy) = 0;
    virtual void moveCanvasHorizontal(qreal dx) = 0;
    virtual void moveCanvasVertical(qreal dy) = 0;

    virtual kfw::RectF notationContentRect() const = 0;
    virtual qreal currentScaling() const = 0;
    virtual void setScaling(qreal scaling, const kfw::PointF& pos, bool overrideZoomType = true) = 0;

    virtual kfw::PointF toLogical(const kfw::PointF& p) const = 0;
    virtual kfw::PointF toLogical(const QPointF& p) const = 0;
    virtual kfw::PointF fromLogical(const kfw::PointF& r) const = 0;
    virtual kfw::RectF fromLogical(const kfw::RectF& r) const = 0;

    virtual bool isNoteEnterMode() const = 0;

    virtual void showContextMenu(const ElementType& elementType, const QPointF& pos) = 0;
    virtual void hideContextMenu() = 0;

    virtual void showElementPopup(const ElementType& elementType) = 0;
    virtual void hideElementPopup(const ElementType& elementType) = 0;
    virtual void hideElementPopup(PopupModelType modelType = PopupModelType::TYPE_UNDEFINED) = 0;
    virtual void toggleElementPopup(const ElementType& elementType) = 0;

    virtual bool elementPopupIsOpen(const ElementType& elementType) const = 0;

    virtual INotationInteractionPtr notationInteraction() const = 0;
    virtual INotationPlaybackPtr notationPlayback() const = 0;

    virtual QQuickItem* asItem() = 0;
};

class NotationViewInputController : public kfw::actions::Actionable, public kfw::Contextable, public kfw::async::Asyncable
{
public:
    kfw::GlobalInject<kfw::IGlobalConfiguration> globalConfiguration;
    kfw::GlobalInject<INotationConfiguration> configuration;
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<playback::IPlaybackController> playbackController = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<kfw::ui::IDragController> dragController = { this };

public:
    NotationViewInputController(IControlledView* view, const kfw::modularity::ContextPtr& iocCtx);

    void init();

    void initZoom();
    void initCanvasPos();
    void updateZoomAfterSizeChange();
    void zoomIn();
    void zoomOut();
    void nextScreen();
    void previousScreen();
    void nextPage();
    void previousPage();
    void startOfScore();
    void endOfScore();

    bool readonly() const;
    void setReadonly(bool readonly);

    void pinchToZoom(qreal scaleFactor, const QPointF& pos);
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void hoverMoveEvent(QHoverEvent* event);
    void hoverLeaveEvent(QHoverEvent* event);
    bool shortcutOverrideEvent(QKeyEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    void inputMethodEvent(QInputMethodEvent* event);

    bool canHandleInputMethodQuery(Qt::InputMethodQuery query) const;
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const;

    bool ignoreNextMouseContextMenuEvent() const { return m_ignoreNextMouseContextMenuEvent; }

    void dragEnterEvent(QDragEnterEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dropEvent(QDropEvent* event);

    ElementType selectionType() const;
    kfw::PointF selectionElementPos() const;

private:
    INotationPtr currentNotation() const;
    INotationStylePtr notationStyle() const;
    INotationInteractionPtr viewInteraction() const;
    const INotationInteraction::HitElementContext& hitElementContext() const;

    void onNotationChanged();

    void zoomToPageWidth();
    void doZoomToPageWidth();
    void zoomToWholePage();
    void doZoomToWholePage();
    void zoomToTwoPages();
    void doZoomToTwoPages();
    void moveScreen(int direction);
    void movePage(int direction);

    int currentZoomIndex() const;
    int currentZoomPercentage() const;
    kfw::PointF findZoomFocusPoint() const;
    void setScaling(qreal scaling, const kfw::PointF& pos = kfw::PointF(), bool overrideZoomType = true);
    void setZoom(int zoomPercentage, const kfw::PointF& pos = kfw::PointF());

    qreal scalingFromZoomPercentage(int zoomPercentage) const;
    int zoomPercentageFromScaling(qreal scaling) const;

    void setViewMode(const ViewMode& viewMode);

    void startDragElements(ElementType elementsType, const kfw::PointF& elementsOffset);

    void togglePopupForItemIfSupports(const EngravingItem* item);
    void updateShadowNotePopupVisibility(bool forceHide = false);

    float hitWidth() const;

    struct ClickContext {
        kfw::PointF logicClickPos;
        const QMouseEvent* event = nullptr;
        kn::engraving::EngravingItem* hitElement = nullptr;
        kn::engraving::staff_idx_t hitStaff = kfw::nidx;
        bool isHitGrip = false;
    };

    void handleClickInNoteInputMode(QMouseEvent* event);
    bool mousePress_considerGrip(const ClickContext& ctx); // returns true if event is consumed
    bool mousePress_considerDragOutgoingElement(const ClickContext& ctx);
    void mousePress_considerSelect(const ClickContext& ctx);
    void mousePress_seekSelection(const ClickContext& ctx);
    void cycleOverlappingHitElements(const std::vector<EngravingItem*>& hitElements, staff_idx_t hitStaffIndex);
    bool mousePress_considerDragOutgoingRange(const ClickContext& ctx);
    bool mousePress_considerStartPasteRangeOnRelease(const ClickContext& ctx);
    void handleLeftClick(const ClickContext& ctx);
    void handleRightClick(const ClickContext& ctx);
    void handleLeftClickRelease(const QPointF& releasePoint);

    bool startTextEditingAllowed() const;
    void updateTextCursorPosition();

    bool isAnchorEditingEvent(QKeyEvent* event) const;

    bool tryPercussionShortcut(QKeyEvent* event);

    IControlledView* m_view = nullptr;

    QList<int> m_possibleZoomPercentages;

    bool m_readonly = false;
    bool m_isCanvasDragged = false;
    bool m_tripleClickPending = false;

    struct MouseDownInfo {
        enum DragAction {
            DragOutgoingElement,
            DragOutgoingRange,
            PasteRangeOnRelease,
            Other,
            Nothing
        } dragAction = Other;

        QPointF physicalBeginPoint;
        kfw::PointF logicalBeginPoint;
    } m_mouseDownInfo;

    struct DragMoveEvent {
        QPointF position;
        Qt::KeyboardModifiers modifiers;
        Qt::DropAction dropAction = Qt::DropAction::CopyAction;
        QObject* source = nullptr;
    };

    bool dropEvent(const DragMoveEvent& event, const QMimeData* mimeData = nullptr);

    std::vector<int> pitchesBeingDragged() const;

    DragMoveEvent m_lastDragMoveEvent;

    const kn::engraving::EngravingItem* m_prevSelectedElement = nullptr;
    std::vector<const kn::engraving::EngravingItem*> m_notesBeingDragged;

    bool m_hitElementWasAlreadySingleSelected = false;
    bool m_shouldSelectOnLeftClickRelease = false;
    bool m_shouldStartEditOnLeftClickRelease = false;
    bool m_ignoreNextMouseContextMenuEvent = false;
};
}
