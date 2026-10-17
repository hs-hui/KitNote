#ifndef KN_APP_GUIAPP_H
#define KN_APP_GUIAPP_H

#include <vector>
#include <memory>

#include "global/internal/baseapplication.h"
#include "../cmdoptions.h"

#include "modularity/imodulesetup.h"
#include "global/globalmodule.h"

#include "modularity/ioc.h"
#include "multiwindows/imultiwindowsprovider.h"
#include "appshell/iappshellconfiguration.h"
#include "appshell/internal/istartupscenario.h"
#include "importexport/guitarpro/iguitarproconfiguration.h"

class QQuickWindow;
namespace kn::appshell {
class SplashScreen;
}

namespace kn::app {
class GuiApp : public kfw::BaseApplication, public std::enable_shared_from_this<GuiApp>
{
    kfw::GlobalInject<kfw::mi::IMultiWindowsProvider> multiwindowsProvider;
    kfw::GlobalInject<appshell::IAppShellConfiguration> appshellConfiguration;
    kfw::GlobalInject<iex::guitarpro::IGuitarProConfiguration> guitarProConfiguration;
    kfw::ContextInject<appshell::IStartupScenario> startupScenario = { this };

public:
    GuiApp(const CmdOptions& options, const kfw::modularity::ContextPtr& ctx);

    void addModule(kfw::modularity::IModuleSetup* module);

    void setup() override;
    void finish() override;

    kfw::modularity::ContextPtr setupNewContext() override;
    int contextCount() const override;
    std::vector<kfw::modularity::ContextPtr> contexts() const override;

private:
    void applyCommandLineOptions(const CmdOptions& options);

    std::vector<kfw::modularity::IContextSetup*>& contextSetups(const kfw::modularity::ContextPtr& ctx);

    CmdOptions m_options;

    appshell::SplashScreen* m_splashScreen = nullptr;

    //! NOTE Separately to initialize logger and profiler as early as possible
    kfw::GlobalModule m_globalModule;
    std::vector<kfw::modularity::IModuleSetup*> m_modules;

    struct Context {
        kfw::modularity::ContextPtr ctx;
        std::vector<kfw::modularity::IContextSetup*> setups;
    };

    std::vector<Context> m_contexts;
    QQuickWindow* m_window = nullptr;
};
}

#endif // KN_APP_GUIAPP_H
