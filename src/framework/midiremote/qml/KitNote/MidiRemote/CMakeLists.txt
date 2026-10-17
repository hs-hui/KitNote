kfw_create_qml_module(kfw_midiremote_qml ALIAS kfw::midiremote_qml FOR kfw_midiremote)

qt_add_qml_module(kfw_midiremote_qml
    URI KitNote.MidiRemote
    VERSION 1.0
    SOURCES
        editmidimappingmodel.cpp
        editmidimappingmodel.h
        mididevicemappingmodel.cpp
        mididevicemappingmodel.h
    QML_FILES
        EditMidiMappingDialog.qml
        internal/MidiMappingBottomPanel.qml
        internal/MidiMappingTopPanel.qml
        MidiDeviceMappingPage.qml
    IMPORTS
        TARGET kfw_ui_qml
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(kfw_midiremote_qml)