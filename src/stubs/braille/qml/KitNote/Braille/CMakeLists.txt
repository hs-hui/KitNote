kfw_create_qml_module(braille_qml FOR braille STUB)

qt_add_qml_module(braille_qml
    URI KitNote.Braille
    VERSION 1.0
    QML_FILES
        BrailleView.qml
    IMPORTS
        TARGET kfw_ui_qml
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(braille_qml)
