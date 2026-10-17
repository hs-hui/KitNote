#include "previewprovider.h"

#include <memory>
#include <string>
#include <vector>

#include <QGuiApplication>

#include "draw/drawmodule.h"
#include "engraving/engravingmodule.h"
#include "global/globalmodule.h"
#include "global/iapplication.h"
#include "global/io/buffer.h"
#include "importexport/imagesexport/imagesexportmodule.h"
#include "importexport/imagesexport/internal/pdfwriter.h"
#include "modularity/imodulesetup.h"
#include "modularity/ioc.h"
#include "notation/notationmodule.h"
#include "project/inotationproject.h"
#include "project/iprojectconfiguration.h"
#include "project/iprojectcreator.h"
#include "project/projectmodule.h"
#include "project/types/projecttypes.h"
#include "types/bytearray.h"

class App
{
public:
    App()
    {
        m_modules.emplace_back(std::make_unique<kfw::draw::DrawModule>());
        m_modules.emplace_back(std::make_unique<kn::engraving::EngravingModule>());
        m_modules.emplace_back(std::make_unique<kn::notation::NotationModule>());
        m_modules.emplace_back(std::make_unique<kn::project::ProjectModule>());
        m_modules.emplace_back(std::make_unique<kn::iex::imagesexport::ImagesExportModule>());

        int argc = 0;
        std::vector<char*> argv;

        m_qapp = std::make_unique<QGuiApplication>(argc, argv.data());

        m_globalModule.registerResources();
        m_globalModule.registerExports();
        for (const auto& m : m_modules) {
            m->registerResources();
        }

        for (const auto& m : m_modules) {
            m->registerExports();
        }

        m_globalModule.resolveImports();
        for (const auto& m : m_modules) {
            m->resolveImports();
        }

        constexpr auto runMode = kfw::IApplication::RunMode::ConsoleApp;
        m_globalModule.onPreInit(runMode);
        for (const auto& m : m_modules) {
            m->onPreInit(runMode);
        }

        m_globalModule.onInit(runMode);
        for (const auto& m : m_modules) {
            m->onInit(runMode);
        }

        m_globalModule.onAllInited(runMode);
        for (const auto& m : m_modules) {
            m->onAllInited(runMode);
        }

        disableMigration();
    }

    ~App()
    {
        for (const auto& m : m_modules) {
            m->onDeinit();
        }
        m_globalModule.onDeinit();

        for (const auto& m : m_modules) {
            m->onDestroy();
        }
        m_globalModule.onDestroy();
    }

private:
    void disableMigration()
    {
        kfw::GlobalInject<kn::project::IProjectConfiguration> projectConfiguration;

        // Based on ConsoleApp::applyCommandLineOptions
        kn::project::MigrationOptions migration;
        migration.appVersion = kn::engraving::Constants::MSC_VERSION;
        migration.isAskAgain = false;
        migration.isApplyLeland = false;
        migration.isApplyEdwin = false;
        migration.isRemapPercussion = false;

        for (kn::project::MigrationType type : kn::project::allMigrationTypes()) {
            projectConfiguration()->setMigrationOptions(type, migration, false);
        }
    }

    std::unique_ptr<QGuiApplication> m_qapp; // Needed for fonts loading
    kfw::GlobalModule m_globalModule;
    std::vector<std::unique_ptr<kfw::modularity::IModuleSetup> > m_modules;
};

void PreviewProviderCxx::initIfNeeded()
{
    static App app;
}

std::vector<uint8_t> PreviewProviderCxx::getPdfPreviewData(const std::string& filePath)
{
    kfw::GlobalInject<kn::project::IProjectCreator> projectCreator;

    kn::project::INotationProjectPtr project = projectCreator()->newProject(nullptr);

    kn::project::OpenParams openParams;
    openParams.disablePlayback = true;
    openParams.forceMode = true;
    openParams.forcePageMode = true;
    project->load(filePath, openParams);

    kfw::io::Buffer buffer;
    buffer.open(kfw::io::IODevice::WriteOnly);

    kn::iex::imagesexport::PdfWriter pdfWriter { nullptr };
    pdfWriter.write(project->masterNotation()->notation(), buffer);

    kfw::ByteArray byteArray = buffer.data();
    std::vector<uint8_t> data = byteArray.vdata();

    return data;
}
