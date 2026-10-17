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
#ifndef KN_APPSHELL_APPSHELLCONFIGURATION_H
#define KN_APPSHELL_APPSHELLCONFIGURATION_H

#include "iappshellconfiguration.h"

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "global/iapplication.h"
#include "ui/iuiconfiguration.h"

namespace kn::appshell {
class AppShellConfiguration : public IAppShellConfiguration, public kfw::Contextable, public kfw::async::Asyncable
{
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::ContextInject<kfw::IApplication> application = { this };

public:
    AppShellConfiguration(const kfw::modularity::ContextPtr& iocCtx)
        : kfw::Contextable(iocCtx) {}

    void init();

    std::string kitNoteVersion() const override;
    std::string kitNoteRevision() const override;

    bool isNotationNavigatorVisible() const override;
    void setIsNotationNavigatorVisible(bool visible) const override;
    kfw::async::Notification isNotationNavigatorVisibleChanged() const override;

private:
};
}

#endif // KN_APPSHELL_APPSHELLCONFIGURATION_H
