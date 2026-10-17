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

#ifndef KN_CONSOLEAPP_APP_H
#define KN_CONSOLEAPP_APP_H

#include <vector>
#include <memory>
#include <map>

#include "global/internal/baseapplication.h"
#include "../cmdoptions.h"

#include "global/globalmodule.h"

#include "modularity/imodulesetup.h"
#include "modularity/ioc.h"
#include "global/iapplication.h"
#include "converter/iconvertercontroller.h"
#include "engraving/devtools/drawdata/idiagnosticdrawprovider.h"
#include "audioplugins/iregisteraudiopluginsscenario.h"

#include "ui/iuiconfiguration.h"
#include "notation/inotationconfiguration.h"
#include "project/iprojectconfiguration.h"
#include "playback/isoundprofilesrepository.h"
#include "importexport/imagesexport/iimagesexportconfiguration.h"
#include "importexport/midi/imidiconfiguration.h"
#include "importexport/audioexport/iaudioexportconfiguration.h"
#include "importexport/guitarpro/iguitarproconfiguration.h"
#include "importexport/musicxml/imusicxmlconfiguration.h"

namespace kn::app {
class ConsoleApp : public kfw::BaseApplication, public std::enable_shared_from_this<ConsoleApp>
{
    kfw::GlobalInject<kfw::ui::IUiConfiguration> uiConfiguration;
    kfw::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    kfw::GlobalInject<project::IProjectConfiguration> projectConfiguration;
    kfw::GlobalInject<iex::imagesexport::IImagesExportConfiguration> imagesExportConfiguration;
    kfw::GlobalInject<iex::midi::IMidiImportExportConfiguration> midiImportExportConfiguration;
    kfw::GlobalInject<iex::audioexport::IAudioExportConfiguration> audioExportConfiguration;
    kfw::GlobalInject<iex::guitarpro::IGuitarProConfiguration> guitarProConfiguration;
    kfw::GlobalInject<iex::musicxml::IMusicXmlConfiguration> musicXmlConfiguration;

    kfw::ContextInject<converter::IConverterController> converter = { this };
    kfw::ContextInject<engraving::IDiagnosticDrawProvider> diagnosticDrawProvider = { this };
    kfw::ContextInject<kfw::audioplugins::IRegisterAudioPluginsScenario> registerAudioPluginsScenario = { this };
    kfw::ContextInject<playback::ISoundProfilesRepository> soundProfilesRepository = { this };

public:
    ConsoleApp(const CmdOptions& options, const kfw::modularity::ContextPtr& ctx);

    void addModule(kfw::modularity::IModuleSetup* module);

    void setup() override;
    void finish() override;

    kfw::modularity::ContextPtr setupNewContext() override;
    int contextCount() const override;
    std::vector<kfw::modularity::ContextPtr> contexts() const override;

private:
    void applyCommandLineOptions(const CmdOptions& options, kfw::IApplication::RunMode runMode);
    int processConverter(const CmdOptions::ConverterTask& task);
    int processDiagnostic(const CmdOptions::Diagnostic& task);
    int processAudioPluginRegistration(const CmdOptions::AudioPluginRegistration& task);

    std::vector<kfw::modularity::IContextSetup*>& contextSetups(const kfw::modularity::ContextPtr& ctx);

    CmdOptions m_options;

    //! NOTE Separately to initialize logger and profiler as early as possible
    kfw::GlobalModule m_globalModule;

    std::vector<kfw::modularity::IModuleSetup*> m_modules;
    kfw::modularity::ContextPtr m_context;

    struct Context {
        kfw::modularity::ContextPtr ctx;
        std::vector<kfw::modularity::IContextSetup*> setups;
    };

    std::vector<Context> m_contexts;
};
}

#endif // KN_CONSOLEAPP_APP_H
