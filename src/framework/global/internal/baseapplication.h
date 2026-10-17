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
#ifndef KFW_GLOBAL_BASEAPPLICATION_H
#define KFW_GLOBAL_BASEAPPLICATION_H

#include "../iapplication.h"

#include "modularity/ioc.h"
#include "global/itickerprovider.h"

namespace kfw {
class BaseApplication : public IApplication, public kfw::Contextable
{
    GlobalInject<ITickerProvider> tickerProvider;
public:

    BaseApplication(const modularity::ContextPtr& ctx);

    static String appName();
    static String appTitle();
    static bool appUnstable();
    static Version appVersion();
    static Version appFullVersion();
    static String appBuild();
    static String appRevision();

    String name() const override { return appName(); }
    String title() const override { return appTitle(); }
    bool unstable() const override { return appUnstable(); }
    Version version() const override { return appVersion(); }
    Version fullVersion() const override { return appFullVersion(); }
    String build() const override { return appBuild(); }
    String revision() const override { return appRevision(); }

    void setRunMode(const RunMode& mode);
    RunMode runMode() const override;
    bool noGui() const override;

    void restart() override;
    void finish() override;

    const modularity::ContextPtr iocContext() const override;
    modularity::ModulesIoC* ioc() const override;

    void processEvents() override;

#ifndef NO_QT_SUPPORT
    QWindow* focusWindow() const override;
    bool notify(QObject* object, QEvent* event) override;

    Qt::KeyboardModifiers keyboardModifiers() const override;
#endif

protected:

    void removeIoC();

private:
    RunMode m_runMode = RunMode::GuiApp;
    FinishMode m_finishMode = FinishMode::Default;
    modularity::ContextPtr m_iocContext;
};
}

#endif // KFW_GLOBAL_BASEAPPLICATION_H
