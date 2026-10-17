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
#include "canvaspreferencesmodel.h"

#include "types/translatablestring.h"

using namespace kn::preferences;
using namespace kn::notation;

CanvasPreferencesModel::CanvasPreferencesModel(QObject* parent)
    : QObject(parent), kfw::Contextable(kfw::iocCtxForQmlObject(this))
{
}

void CanvasPreferencesModel::load()
{
    setupConnections();
}

void CanvasPreferencesModel::setupConnections()
{
    notationConfiguration()->defaultZoomChanged().onNotify(this, [this]() {
        emit defaultZoomChanged();
    });
    notationConfiguration()->mouseZoomPrecisionChanged().onNotify(this, [this]() {
        emit mouseZoomPrecisionChanged();
    });
    notationConfiguration()->canvasOrientation().ch.onReceive(this, [this](kfw::Orientation) {
        emit scrollPagesOrientationChanged();
    });
    notationSceneConfiguration()->isLimitCanvasScrollAreaChanged().onNotify(this, [this]() {
        emit limitScrollAreaChanged();
    });
    notationConfiguration()->selectionProximityChanged().onReceive(this, [this](int selectionProximity) {
        emit selectionProximityChanged(selectionProximity);
    });
}

QVariantList CanvasPreferencesModel::zoomTypes() const
{
    QVariantList types = {
        QVariantMap { { "title", zoomTypeTitle(ZoomType::Percentage).qTranslated() }, { "value", static_cast<int>(ZoomType::Percentage) } },
        QVariantMap { { "title", zoomTypeTitle(ZoomType::PageWidth).qTranslated() }, { "value", static_cast<int>(ZoomType::PageWidth) } },
        QVariantMap { { "title", zoomTypeTitle(ZoomType::WholePage).qTranslated() }, { "value", static_cast<int>(ZoomType::WholePage) } },
        QVariantMap { { "title", zoomTypeTitle(ZoomType::TwoPages).qTranslated() }, { "value", static_cast<int>(ZoomType::TwoPages) } }
    };

    return types;
}

QVariantMap CanvasPreferencesModel::defaultZoom() const
{
    QVariantMap zoom;
    ZoomType zoomType = defaultZoomType();
    zoom["type"] = static_cast<int>(zoomType);
    zoom["isPercentage"] = zoomType == ZoomType::Percentage;
    zoom["level"] = notationConfiguration()->defaultZoom();

    return zoom;
}

int CanvasPreferencesModel::mouseZoomPrecision() const
{
    return notationConfiguration()->mouseZoomPrecision();
}

int CanvasPreferencesModel::scrollPagesOrientation() const
{
    return static_cast<int>(notationConfiguration()->canvasOrientation().val);
}

bool CanvasPreferencesModel::limitScrollArea() const
{
    return notationSceneConfiguration()->isLimitCanvasScrollArea();
}

int CanvasPreferencesModel::selectionProximity() const
{
    return notationConfiguration()->selectionProximity();
}

void CanvasPreferencesModel::setDefaultZoomType(int zoomType)
{
    ZoomType type = static_cast<ZoomType>(zoomType);
    if (defaultZoomType() == type) {
        return;
    }

    notationConfiguration()->setDefaultZoomType(type);
    emit defaultZoomChanged();
}

void CanvasPreferencesModel::setDefaultZoomLevel(int zoom)
{
    if (defaultZoomLevel() == zoom) {
        return;
    }

    notationConfiguration()->setDefaultZoom(zoom);
    emit defaultZoomChanged();
}

void CanvasPreferencesModel::setMouseZoomPrecision(int precision)
{
    if (mouseZoomPrecision() == precision) {
        return;
    }

    notationConfiguration()->setMouseZoomPrecision(precision);
    emit mouseZoomPrecisionChanged();
}

void CanvasPreferencesModel::setScrollPagesOrientation(int orientation)
{
    if (orientation == scrollPagesOrientation()) {
        return;
    }

    notationConfiguration()->setCanvasOrientation(static_cast<kfw::Orientation>(orientation));
}

void CanvasPreferencesModel::setLimitScrollArea(bool limit)
{
    if (limitScrollArea() == limit) {
        return;
    }

    notationSceneConfiguration()->setIsLimitCanvasScrollArea(limit);
    emit limitScrollAreaChanged();
}

void CanvasPreferencesModel::setSelectionProximity(int proximity)
{
    if (selectionProximity() == proximity) {
        return;
    }

    notationConfiguration()->setSelectionProximity(proximity);
    emit selectionProximityChanged(proximity);
}

ZoomType CanvasPreferencesModel::defaultZoomType() const
{
    return notationConfiguration()->defaultZoomType();
}

int CanvasPreferencesModel::defaultZoomLevel() const
{
    return notationConfiguration()->defaultZoom();
}
