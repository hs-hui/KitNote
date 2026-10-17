/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2022 MuseScore Limited and others
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

#ifndef KN_APPSHELL_SPLASHSCREEN_H
#define KN_APPSHELL_SPLASHSCREEN_H

#include <QWidget>

class QSvgRenderer;

namespace kn::appshell {
class SplashScreen : public QWidget
{
public:
    enum SplashScreenType {
        Default,
        ForNewInstance
    };

    explicit SplashScreen(SplashScreenType type, bool forNewScore = false, const QString& openingFileName = QString());

private:
    bool event(QEvent* event) override;
    void repaint();
    void draw(QPainter* painter);
    void setSize(const QSize& size);

    QWidget* m_view = nullptr;
};
}

#endif // KN_APPSHELL_SPLASHSCREEN_H
