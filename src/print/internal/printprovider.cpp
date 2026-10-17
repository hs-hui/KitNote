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
#include "printprovider.h"

#include <QPrinter>
#include <QPrintDialog>
#include <QWindow>

#include "log.h"

using namespace kfw;
using namespace kfw::draw;
using namespace kn::notation;

namespace kn::print {
PrintProvider::PrintProvider(const kors::modularity::ContextPtr& iocCtx)
    : Contextable(iocCtx)
{
}

Ret PrintProvider::printNotation(INotationPtr notation)
{
    IF_ASSERT_FAILED(notation) {
        return make_ret(Ret::Code::InternalError);
    }

    auto painting = notation->painting();

    SizeF pageSizeInch = painting->pageSizeInch();
    QPrinter printerDev(QPrinter::HighResolution);
    QPageSize ps(QPageSize::id(pageSizeInch.toQSizeF(), QPageSize::Inch));
    printerDev.setPageSize(ps);
    printerDev.setPageOrientation(pageSizeInch.width() > pageSizeInch.height() ? QPageLayout::Landscape : QPageLayout::Portrait);

    //printerDev.setCreator("MuseScore Studio Version: " VERSION);
    printerDev.setFullPage(true);
    if (!printerDev.setPageMargins(QMarginsF())) {
        LOGW() << "unable to clear printer margins";
    }

    printerDev.setDocName(notation->projectWorkTitleAndPartName());
    printerDev.setOutputFormat(QPrinter::NativeFormat);
    printerDev.setFromTo(1, painting->pageCount());

    QPrintDialog pd(&printerDev);
    pd.setMinMax(1, painting->pageCount());

    // HACK: ensure we have a valid windowHandle to which we can set a transient parent to
    pd.winId();
    // the print dialog needs a valid parent window to show the modern print dialog on Windows 11
    pd.windowHandle()->setTransientParent(mainWindow()->qWindow());
    if (!pd.exec()) {
        return kfw::make_ret(Ret::Code::Cancel);
    }

    Painter painter(&printerDev, "print");

    INotationPainting::Options opt;
    opt.fromPage = printerDev.fromPage() - 1;
    opt.toPage = printerDev.toPage() - 1;
    // See https://doc.qt.io/qt-5/qprinter.html#supportsMultipleCopies
    opt.copyCount = printerDev.supportsMultipleCopies() ? 1 : printerDev.copyCount();
    opt.deviceDpi = printerDev.logicalDpiX();
    opt.onNewPage = [&printerDev]() { printerDev.newPage(); };

    painting->paintPrint(&painter, opt);

    painter.endDraw();

    return kfw::make_ok();
}
}
