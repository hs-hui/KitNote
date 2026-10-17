#pragma once

#include "global/iapplication.h"

#include "cmdoptions.h"

namespace kn::app {
class AppFactory
{
public:
    AppFactory() = default;

    std::shared_ptr<kfw::IApplication> newApp(const CmdOptions& options) const;

private:
    std::shared_ptr<kfw::IApplication> newGuiApp(const CmdOptions& options) const;
    std::shared_ptr<kfw::IApplication> newConsoleApp(const CmdOptions& options) const;

    mutable int m_lastID = 0;
};
}
