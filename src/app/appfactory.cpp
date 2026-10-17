#include "appfactory.h"

#include "internal/guiapp.h"
#include "internal/consoleapp.h"

#include "global/runtime.h"

#include "kfw_framework_config.h"
#include "app_config.h"

#ifdef KFW_MODULE_ACCESSIBILITY
#include "framework/accessibility/accessibilitymodule.h"
#else
#include "framework/stubs/accessibility/accessibilitystubmodule.h"
#endif

#include "framework/actions/actionsmodule.h"

#ifdef KFW_MODULE_AUDIO
#include "framework/audio/main/audiomodule.h"
#else
#include "framework/stubs/audio/audiostubmodule.h"
#endif

#ifdef KFW_MODULE_AUDIOPLUGINS
#include "framework/audioplugins/audiopluginsmodule.h"
#endif

#include "framework/draw/drawmodule.h"

#ifdef KFW_MODULE_DIAGNOSTICS
#include "diagnostics/diagnosticsmodule.h"
#endif

#ifdef KFW_MODULE_LANGUAGES
#include "framework/languages/languagesmodule.h"
#else
#include "framework/stubs/languages/languagesstubmodule.h"
#endif

#include "framework/stubs/learn/learnmodule.h"

#ifdef KFW_MODULE_MEDIA
#include "framework/media/mediamodule.h"
#else
#include "framework/stubs/media/mediastubmodule.h"
#endif

#ifdef KFW_MODULE_MIDI
#include "framework/midi/midimodule.h"
#else
#include "framework/stubs/midi/midistubmodule.h"
#endif

#ifdef KFW_MODULE_MIDIREMOTE
#include "framework/midiremote/midiremotemodule.h"
#else
#include "framework/stubs/midiremote/midiremotestubmodule.h"
#endif

#ifdef KFW_MODULE_MPE
#include "framework/mpe/mpemodule.h"
#else
#include "framework/stubs/mpe/mpestubmodule.h"
#endif

#ifdef KFW_MODULE_MULTIWINDOWS
#include "framework/multiwindows/multiwindowsmodule.h"
#else
#include "framework/stubs/multiwindows/multiwindowsstubmodule.h"
#endif

#ifdef KFW_MODULE_NETWORK
#include "framework/network/networkmodule.h"
#else
#include "framework/stubs/network/networkstubmodule.h"
#endif

#ifdef KFW_MODULE_SHORTCUTS
#include "framework/shortcuts/shortcutsmodule.h"
#else
#include "framework/stubs/shortcuts/shortcutsstubmodule.h"
#endif

#ifdef KFW_MODULE_TOURS
#include "framework/tours/toursmodule.h"
#else
#include "framework/stubs/tours/toursstubmodule.h"
#endif

#ifdef KFW_MODULE_UI
#include "framework/ui/uimodule.h"
#include "framework/uicomponents/uicomponentsmodule.h"
#endif

#ifdef KFW_MODULE_DOCKWINDOW
#include "framework/dockwindow/dockmodule.h"
#endif

#include "framework/stubs/update/updatestubmodule.h"

#ifdef KFW_MODULE_VST
#include "framework/vst/vstmodule.h"
#else
#include "framework/stubs/vst/vststubmodule.h"
#endif

#ifdef KFW_MODULE_WORKSPACE
#include "framework/workspace/workspacemodule.h"
#else
#include "framework/stubs/workspace/workspacestubmodule.h"
#endif

// Modules
#include "appshell/appshellmodule.h"

#ifdef KITNOTE_BUILD_BRAILLE_MODULE
#include "braille/braillemodule.h"
#else
#include "stubs/braille/braillestubmodule.h"
#endif

#include "context/contextmodule.h"

#ifdef KITNOTE_BUILD_CONVERTER_MODULE
#include "converter/convertermodule.h"
#endif

#include "engraving/engravingmodule.h"

#ifdef KITNOTE_BUILD_IMPEXP_BB_MODULE
#include "importexport/bb/bbmodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_BWW_MODULE
#include "importexport/bww/bwwmodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_CAPELLA_MODULE
#include "importexport/capella/capellamodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_MIDI_MODULE
#include "importexport/midi/midimodule.h"
#else
#include "stubs/importexport/midi/midimodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_MNX_MODULE
#include "importexport/mnx/mnxmodule.h"
#else
#include "stubs/importexport/mnx/mnxmodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_MUSEDATA_MODULE
#include "importexport/musedata/musedatamodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_MUSICXML_MODULE
#include "importexport/musicxml/musicxmlmodule.h"
#else
#include "stubs/importexport/musicxml/musicxmlmodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_OVE_MODULE
#include "importexport/ove/ovemodule.h"
#else
#include "stubs/importexport/ove/ovemodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_AUDIOEXPORT_MODULE
#include "importexport/audioexport/audioexportmodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_IMAGESEXPORT_MODULE
#include "importexport/imagesexport/imagesexportmodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_GUITARPRO_MODULE
#include "importexport/guitarpro/guitarpromodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_MEI_MODULE
#include "importexport/mei/meimodule.h"
#else
#include "stubs/importexport/mei/meimodule.h"
#endif

#ifdef KITNOTE_BUILD_IMPEXP_TABLEDIT_MODULE
#include "importexport/tabledit/tableditmodule.h"
#endif
#ifdef KITNOTE_BUILD_IMPEXP_LYRICS_MODULE
#include "importexport/lyricsexport/lyricsexportmodule.h"
#endif

#include "inspector/inspectormodule.h"

#ifdef KITNOTE_BUILD_INSTRUMENTSSCENE_MODULE
#include "instrumentsscene/instrumentsscenemodule.h"
#else
#include "stubs/instrumentsscene/instrumentsscenestubmodule.h"
#endif

#ifdef KITNOTE_BUILD_NOTATION_MODULE
#include "notation/notationmodule.h"
#else
#include "stubs/notation/notationstubmodule.h"
#endif

#ifdef KITNOTE_BUILD_NOTATIONSCENE_MODULE
#include "notationscene/notationscenemodule.h"
#else
#include "stubs/notationscene/notationscenestubmodule.h"
#endif

#ifdef KITNOTE_BUILD_PALETTE_MODULE
#include "palette/palettemodule.h"
#else
#include "stubs/palette/palettestubmodule.h"
#endif

#ifdef KITNOTE_BUILD_PLAYBACK_MODULE
#include "playback/playbackmodule.h"
#else
#include "stubs/playback/playbackstubmodule.h"
#endif

#ifdef KITNOTE_BUILD_PREFERENCES_MODULE
#include "preferences/preferencesmodule.h"
#endif

#ifdef KITNOTE_BUILD_PRINT_MODULE
#include "print/printmodule.h"
#endif

#ifdef KITNOTE_BUILD_PROJECT_MODULE
#include "project/projectmodule.h"
#else
#include "stubs/project/projectstubmodule.h"
#endif

#ifdef KITNOTE_CONFIGURATION_IS_APPWEB
#include "web/appjs/appjsmodule.h"
#endif

using namespace kfw;
using namespace kn::app;

std::shared_ptr<kfw::IApplication> AppFactory::newApp(const CmdOptions& options) const
{
    if (options.runMode == IApplication::RunMode::GuiApp) {
        return newGuiApp(options);
    } else {
        return newConsoleApp(options);
    }
}

std::shared_ptr<kfw::IApplication> AppFactory::newGuiApp(const CmdOptions& options) const
{
    modularity::ContextPtr ctx = std::make_shared<modularity::Context>();
    ++m_lastID;
    //ctx->id = m_lastID;
    ctx->id = -1; //! NOTE At the moment global ioc

    std::shared_ptr<GuiApp> app = std::make_shared<GuiApp>(options, ctx);

#ifdef KFW_MODULE_DIAGNOSTICS
    //! NOTE `diagnostics` is added first by convention.
    //! For other modules, the order is (and should be) unimportant.
    app->addModule(new kfw::diagnostics::DiagnosticsModule());
#endif

    // framework
    app->addModule(new kfw::accessibility::AccessibilityModule());
    app->addModule(new kfw::actions::ActionsModule());
    app->addModule(new kfw::audio::AudioModule());
#ifdef KFW_MODULE_AUDIOPLUGINS
    app->addModule(new kfw::audioplugins::AudioPluginsModule());
#endif
    app->addModule(new kfw::draw::DrawModule());
    app->addModule(new kfw::midi::MidiModule());
    app->addModule(new kfw::midiremote::MidiRemoteModule());
    app->addModule(new kfw::mpe::MpeModule());

    app->addModule(new kfw::network::NetworkModule());
    app->addModule(new kfw::shortcuts::ShortcutsModule());
#ifdef KFW_MODULE_UI
    app->addModule(new kfw::ui::UiModule());
    app->addModule(new kfw::uicomponents::UiComponentsModule());
#endif

#ifdef KFW_MODULE_DOCKWINDOW
    app->addModule(new kfw::dock::DockModule());
#endif
    app->addModule(new kfw::tours::ToursModule());
    app->addModule(new kfw::vst::VSTModule());
    app->addModule(new kfw::media::MediaModule());

// modules
#ifdef KITNOTE_BUILD_APPSHELL_MODULE
    app->addModule(new kn::appshell::AppShellModule());
#endif

    app->addModule(new kn::braille::BrailleModule());

    app->addModule(new kn::context::ContextModule());

#ifdef KITNOTE_BUILD_CONVERTER_MODULE
    app->addModule(new kn::converter::ConverterModule());
#endif

    app->addModule(new kn::engraving::EngravingModule());

#ifdef KITNOTE_BUILD_IMPEXP_BB_MODULE
    app->addModule(new kn::iex::bb::BBModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_BWW_MODULE
    app->addModule(new kn::iex::bww::BwwModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_CAPELLA_MODULE
    app->addModule(new kn::iex::capella::CapellaModule());
#endif
    app->addModule(new kn::iex::midi::MidiModule());
    app->addModule(new kn::iex::mnxio::MnxModule());
#ifdef KITNOTE_BUILD_IMPEXP_MUSEDATA_MODULE
    app->addModule(new kn::iex::musedata::MuseDataModule());
#endif
    app->addModule(new kn::iex::musicxml::MusicXmlModule());
    app->addModule(new kn::iex::ove::OveModule());
#ifdef KITNOTE_BUILD_IMPEXP_AUDIOEXPORT_MODULE
    app->addModule(new kn::iex::audioexport::AudioExportModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_IMAGESEXPORT_MODULE
    app->addModule(new kn::iex::imagesexport::ImagesExportModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_GUITARPRO_MODULE
    app->addModule(new kn::iex::guitarpro::GuitarProModule());
#endif
    app->addModule(new kn::iex::mei::MeiModule());
#ifdef KITNOTE_BUILD_IMPEXP_TABLEDIT_MODULE
    app->addModule(new kn::iex::tabledit::TablEditModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_LYRICS_MODULE
    app->addModule(new kn::iex::lrcexport::LyricsExportModule());
#endif

    app->addModule(new kn::inspector::InspectorModule());
    app->addModule(new kn::instrumentsscene::InstrumentsSceneModule());
    app->addModule(new kfw::languages::LanguagesModule());
    app->addModule(new kfw::learn::LearnModule());
    app->addModule(new kfw::mi::MultiInstancesModule());
    app->addModule(new kn::notation::NotationModule());
    app->addModule(new kn::notation::NotationSceneModule());
    app->addModule(new kn::palette::PaletteModule());
    app->addModule(new kn::playback::PlaybackModule());
#ifdef KITNOTE_BUILD_PREFERENCES_MODULE
    app->addModule(new kn::preferences::PreferencesModule());
#endif

#ifdef KITNOTE_BUILD_PRINT_MODULE
    app->addModule(new kn::print::PrintModule());
#endif
    app->addModule(new kn::project::ProjectModule());
    app->addModule(new kfw::update::UpdateModule());
    app->addModule(new kfw::workspace::WorkspaceModule());

#ifdef KITNOTE_CONFIGURATION_IS_APPWEB
    app->addModule(new kn::appjs::AppJsModule());
#endif

    return app;
}

static void addConsoleModules(std::shared_ptr<ConsoleApp> app)
{
#ifdef KFW_MODULE_DIAGNOSTICS
    //! NOTE `diagnostics` is added first by convention.
    //! For other modules, the order is (an should be) unimportant.
    app->addModule(new kfw::diagnostics::DiagnosticsModule());
#endif

    //! TODO Some modules can be removed

    // framework
    app->addModule(new kfw::actions::ActionsModule());
    app->addModule(new kfw::audio::AudioModule());
#ifdef KFW_MODULE_AUDIOPLUGINS
    app->addModule(new kfw::audioplugins::AudioPluginsModule());
#endif
    app->addModule(new kfw::draw::DrawModule());
    app->addModule(new kfw::midi::MidiModule());
    app->addModule(new kfw::mpe::MpeModule());

#ifdef KFW_MODULE_UI
    app->addModule(new kfw::ui::UiModule());
#endif

#ifdef KFW_MODULE_VST
    app->addModule(new kfw::vst::VSTModule());
#endif

    app->addModule(new kfw::media::MediaModule());

    app->addModule(new kn::context::ContextModule());

#ifdef KITNOTE_BUILD_CONVERTER_MODULE
    app->addModule(new kn::converter::ConverterModule());
#endif

    app->addModule(new kn::engraving::EngravingModule());

#ifdef KITNOTE_BUILD_IMPEXP_BB_MODULE
    app->addModule(new kn::iex::bb::BBModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_BWW_MODULE
    app->addModule(new kn::iex::bww::BwwModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_CAPELLA_MODULE
    app->addModule(new kn::iex::capella::CapellaModule());
#endif
    app->addModule(new kn::iex::midi::MidiModule());
    app->addModule(new kn::iex::mnxio::MnxModule());
#ifdef KITNOTE_BUILD_IMPEXP_MUSEDATA_MODULE
    app->addModule(new kn::iex::musedata::MuseDataModule());
#endif
    app->addModule(new kn::iex::musicxml::MusicXmlModule());
    app->addModule(new kn::iex::ove::OveModule());
#ifdef KITNOTE_BUILD_IMPEXP_AUDIOEXPORT_MODULE
    app->addModule(new kn::iex::audioexport::AudioExportModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_IMAGESEXPORT_MODULE
    app->addModule(new kn::iex::imagesexport::ImagesExportModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_GUITARPRO_MODULE
    app->addModule(new kn::iex::guitarpro::GuitarProModule());
#endif
    app->addModule(new kn::iex::mei::MeiModule());
#ifdef KITNOTE_BUILD_IMPEXP_TABLEDIT_MODULE
    app->addModule(new kn::iex::tabledit::TablEditModule());
#endif
#ifdef KITNOTE_BUILD_IMPEXP_LYRICS_MODULE
    app->addModule(new kn::iex::lrcexport::LyricsExportModule());
#endif

    app->addModule(new kfw::mi::MultiInstancesModule());
    app->addModule(new kn::notation::NotationModule());
    app->addModule(new kn::playback::PlaybackModule());
    app->addModule(new kn::project::ProjectModule());
    app->addModule(new kn::notation::NotationSceneModule());
}

static void addAudioPluginRegistrationModules(std::shared_ptr<ConsoleApp> app)
{
    app->addModule(new kfw::audio::AudioModule());

#ifdef KFW_MODULE_AUDIOPLUGINS
    app->addModule(new kfw::audioplugins::AudioPluginsModule());
#endif

#ifdef KFW_MODULE_VST
    app->addModule(new kfw::vst::VSTModule());
#endif
}

std::shared_ptr<kfw::IApplication> AppFactory::newConsoleApp(const CmdOptions& options) const
{
#ifdef KITNOTE_ENABLE_CONSOLEAPP

    modularity::ContextPtr ctx = std::make_shared<modularity::Context>();
    ++m_lastID;
    // ctx->id = m_lastID;
    ctx->id = -1; //! NOTE At the moment global ioc

    std::shared_ptr<ConsoleApp> app = std::make_shared<ConsoleApp>(options, ctx);

    if (options.runMode == kfw::IApplication::RunMode::ConsoleApp) {
        addConsoleModules(app);
    } else if (options.runMode == kfw::IApplication::RunMode::AudioPluginRegistration) {
        addAudioPluginRegistrationModules(app);
    }

    return app;

#else // KITNOTE_ENABLE_CONSOLEAPP

    UNUSED(options);
    return nullptr;

#endif
}
