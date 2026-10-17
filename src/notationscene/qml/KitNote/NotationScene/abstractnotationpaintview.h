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

#include <QTimer>
#include <qqmlintegration.h>

#include "modularity/ioc.h"

#include "notation/inotationconfiguration.h"

#include "actions/actionable.h"
#include "actions/iactionsdispatcher.h"
#include "async/asyncable.h"
#include "context/iglobalcontext.h"
#include "playback/iplaybackcontroller.h"
#include "ui/imainwindow.h"
#include "ui/iuiactionsregister.h"
#include "ui/iuiconfiguration.h"
#include "ui/iuicontextresolver.h"
#include "uicomponents/qml/KitNote/UiComponents/quickpaintedview.h"

#include "notationscene/inotationsceneconfiguration.h"
#include "notationviewinputcontroller.h"
#include "noteinputcursor.h"
#include "notationruler.h"
#include "playbackcursor.h"
#include "loopmarker.h"
#include "continuouspanel.h"
#include "abstractelementpopupmodel.h"

namespace kn::notation {
class AbstractNotationPaintView : public kfw::uicomponents::QuickPaintedView, public IControlledView, public kfw::Contextable,
    public kfw::async::Asyncable, public kfw::actions::Actionable
{
    Q_OBJECT
    QML_ELEMENT;
    QML_UNCREATABLE("Not creatable as it is an abstract base class")

    Q_PROPERTY(qreal startHorizontalScrollPosition READ startHorizontalScrollPosition NOTIFY horizontalScrollChanged)
    Q_PROPERTY(qreal horizontalScrollbarSize READ horizontalScrollbarSize NOTIFY horizontalScrollChanged)
    Q_PROPERTY(qreal startVerticalScrollPosition READ startVerticalScrollPosition NOTIFY verticalScrollChanged)
    Q_PROPERTY(qreal verticalScrollbarSize READ verticalScrollbarSize NOTIFY verticalScrollChanged)

    Q_PROPERTY(QVariant matrix READ matrix NOTIFY matrixChanged)
    Q_PROPERTY(QRectF viewport READ viewport_property NOTIFY viewportChanged)

    Q_PROPERTY(bool publishMode READ publishMode WRITE setPublishMode NOTIFY publishModeChanged)

    Q_PROPERTY(bool isMainView READ isMainView WRITE setIsMainView NOTIFY isMainViewChanged)

    kfw::GlobalInject<INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<INotationSceneConfiguration> configuration;
    kfw::GlobalInject<engraving::IEngravingConfiguration> engravingConfiguration;
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::ContextInject<kfw::actions::IActionsDispatcher> dispatcher = { this };
    kfw::ContextInject<context::IGlobalContext> globalContext = { this };
    kfw::ContextInject<playback::IPlaybackController> playbackController = { this };
    kfw::ContextInject<kfw::ui::IUiContextResolver> uiContextResolver = { this };
    kfw::ContextInject<kfw::ui::IMainWindow> mainWindow = { this };
    kfw::ContextInject<kfw::ui::IUiActionsRegister> actionsRegister = { this };

public:
    explicit AbstractNotationPaintView(QQuickItem* parent = nullptr);
    ~AbstractNotationPaintView() override;

    Q_INVOKABLE void load();

    Q_INVOKABLE void scrollHorizontal(qreal position);
    Q_INVOKABLE void scrollVertical(qreal position);

    Q_INVOKABLE void zoomIn();
    Q_INVOKABLE void zoomOut();

    Q_INVOKABLE void selectOnNavigationActive();

    Q_INVOKABLE void forceFocusIn();

    Q_INVOKABLE void onContextMenuIsOpenChanged(bool open);
    Q_INVOKABLE void onElementPopupIsOpenChanged(const PopupModelType& popupType = PopupModelType::TYPE_UNDEFINED);

    Q_INVOKABLE void setPlaybackCursorItem(QQuickItem* cursor);

    qreal width() const override;
    qreal height() const override;

    kfw::PointF toLogical(const kfw::PointF& point) const override;
    kfw::PointF toLogical(const QPointF& point) const override;
    kfw::RectF toLogical(const kfw::RectF& rect) const;

    kfw::PointF fromLogical(const kfw::PointF& point) const override;
    kfw::RectF fromLogical(const kfw::RectF& rect) const override;

    Q_INVOKABLE bool moveCanvas(qreal dx, qreal dy) override;
    void moveCanvasVertical(qreal dy) override;
    void moveCanvasHorizontal(qreal dx) override;

    qreal currentScaling() const override;
    void setScaling(qreal scaling, const kfw::PointF& pos, bool overrideZoomType = true) override;
    void scale(qreal factor, const kfw::PointF& pos, bool overrideZoomType = true);

    Q_INVOKABLE void pinchToZoom(qreal scaleFactor, const QPointF& pos);

    bool isNoteEnterMode() const override;

    void showContextMenu(const ElementType& elementType, const QPointF& pos) override;
    void hideContextMenu() override;

    void showElementPopup(const ElementType& elementType) override;
    void hideElementPopup(const ElementType& elementType) override;
    void hideElementPopup(PopupModelType modelType = PopupModelType::TYPE_UNDEFINED) override;
    void toggleElementPopup(const ElementType& elementType) override;

    bool elementPopupIsOpen(const ElementType& elementType) const override;

    INotationInteractionPtr notationInteraction() const override;
    INotationPlaybackPtr notationPlayback() const override;

    QQuickItem* asItem() override;

    qreal startHorizontalScrollPosition() const;
    qreal horizontalScrollbarSize() const;
    qreal startVerticalScrollPosition() const;
    qreal verticalScrollbarSize() const;

    QVariant matrix() const;

    kfw::PointF viewportTopLeft() const override;
    kfw::RectF viewport() const;
    QRectF viewport_property() const;

    bool publishMode() const;
    void setPublishMode(bool arg);

    bool isMainView() const;
    void setIsMainView(bool isMainView);

signals:
    void showContextMenuRequested(int elementType, const QPointF& viewPos);
    void hideContextMenuRequested();

    void showElementPopupRequested(kn::notation::AbstractElementPopupModel::PopupModelType modelType);
    void hideElementPopupRequested();
    void isPopupOpenChanged(bool isPopupOpen);

    void horizontalScrollChanged();
    void verticalScrollChanged();

    void backgroundColorChanged(QColor color);
    void matrixChanged();
    void viewportChanged();
    void publishModeChanged();

    void activeFocusRequested();

    void isMainViewChanged(bool isMainView);

protected:
    INotationPtr notation() const;
    void setNotation(INotationPtr notation);

    NotationViewInputController* inputController() const;

    void setReadonly(bool readonly);
    void setMatrix(const kfw::draw::Transform& matrix);

    void moveCanvasToCenter();
    bool moveCanvasToPosition(const kfw::PointF& logicPos);

    kfw::RectF notationContentRect() const override;

    // Draw
    void paint(QPainter* painter) override;

    virtual void onNotationSetup();

    virtual void onLoadNotation(INotationPtr notation);
    virtual void onUnloadNotation(INotationPtr notation);

    virtual void initZoomAndPosition();

    virtual void onMatrixChanged(const kfw::draw::Transform& oldMatrix, const kfw::draw::Transform& newMatrix, bool overrideZoomType);

protected slots:
    virtual void onViewSizeChanged();

private:
    INotationNoteInputPtr notationNoteInput() const;
    INotationElementsPtr notationElements() const;
    INotationStylePtr notationStyle() const;
    INotationSelectionPtr notationSelection() const;

    void clear();
    void initBackground();
    void initNavigatorOrientation();

    bool canReceiveAction(const kfw::actions::ActionCode& actionCode) const override;
    void onCurrentNotationChanged();
    bool isInited() const;

    bool doMoveCanvas(qreal dx, qreal dy);

    void scheduleRedraw(const kfw::RectF& rect = kfw::RectF());
    kfw::RectF correctDrawRect(const kfw::RectF& rect) const;

    // Input
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void hoverLeaveEvent(QHoverEvent* event) override;
    bool event(QEvent* event) override;
    bool shortcutOverride(QKeyEvent* event);
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void inputMethodEvent(QInputMethodEvent* event) override;
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

    bool ensureViewportInsideScrollableArea();

    kfw::RectF scrollableAreaRect() const;

    qreal horizontalScrollableSize() const;
    qreal verticalScrollableSize() const;

    bool adjustCanvasPosition(const kfw::RectF& logicRect, bool adjustVertically = true);
    bool adjustCanvasPositionSmoothPan(const kfw::RectF& cursorRect);

    void onNoteInputStateChanged();

    void onShowItemRequested(const INotationInteraction::ShowItemRequest& request);

    void onPlayingChanged();
    void movePlaybackCursor(kfw::midi::tick_t tick);
    bool needAdjustCanvasVerticallyWhilePlayback(const kfw::RectF& cursorRect);

    void onPlaybackCursorRectChanged();

    void updateLoopMarkers();
    void updateShadowNoteVisibility();

    const Page* pageByPoint(const kfw::PointF& point) const;
    kfw::PointF alignToCurrentPageBorder(const kfw::RectF& showRect, const kfw::PointF& pos) const;

    void paintBackground(const kfw::RectF& rect, kfw::draw::Painter* painter);

    kfw::PointF canvasCenter() const;
    std::pair<qreal, qreal> constraintCanvas(qreal dx, qreal dy) const;

    INotationPtr m_notation;
    kfw::draw::Transform m_matrix;

    bool m_loadCalled = false;
    std::unique_ptr<NotationViewInputController> m_inputController;
    std::unique_ptr<PlaybackCursor> m_playbackCursor;
    std::unique_ptr<NoteInputCursor> m_noteInputCursor;
    std::unique_ptr<NotationRuler> m_ruler;
    std::unique_ptr<LoopMarker> m_loopInMarker;
    std::unique_ptr<LoopMarker> m_loopOutMarker;
    std::unique_ptr<ContinuousPanel> m_continuousPanel;

    qreal m_previousVerticalScrollPosition = 0;
    qreal m_previousHorizontalScrollPosition = 0;

    bool m_readonly = false;
    bool m_publishMode = false;
    int m_lastAcceptedKey = -1;
    bool m_isMainView = false;

    bool m_autoScrollEnabled = true;
    bool m_isAutomaticallyPanEnabled = false;
    bool m_isSmoothPanningEnabled = false;
    QTimer m_enableAutoScrollTimer;

    PopupModelType m_currentElementPopupType = PopupModelType::TYPE_UNDEFINED;
    bool m_isContextMenuOpen = false;

    kfw::RectF m_shadowNoteRect;

    QQuickItem* m_playbackCursorItem = nullptr;
};
}
