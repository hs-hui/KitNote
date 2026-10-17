kfw_create_qml_module(kfw_shortcuts_qml ALIAS kfw::shortcuts_qml FOR kfw_shortcuts STUB)

qt_add_qml_module(kfw_shortcuts_qml
    URI KitNote.Shortcuts
    VERSION 1.0
    QML_FILES
        Shortcuts.qml
    IMPORTS
        TARGET kfw_ui_qml
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(kfw_shortcuts_qml)
