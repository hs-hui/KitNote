/*
 * SPDX-License-Identifier: GPL-3.0-only
 *
 * KitNote
 * Drum notation, built on MuseScore Studio
 *
 * Copyright (C) 2025 MuseScore Limited and others
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

#include <QObject>
#include <QDialog>
#include <QWindow>

namespace kfw::ui {
class WidgetDialogAdapter : public QObject
{
public:
    WidgetDialogAdapter(QDialog* parent);

    WidgetDialogAdapter& onShow(const std::function<void()>& func);
    WidgetDialogAdapter& onHide(const std::function<void()>& func);

    bool eventFilter(QObject* watched, QEvent* event) override;

private:

    QDialog* m_dialog = nullptr;
    std::function<void()> m_onShownCallBack;
    std::function<void()> m_onHideCallBack;
};
}
