kfw_create_qml_module(kfw_workspace_qml ALIAS kfw::workspace_qml FOR kfw_workspace STUB)

qt_add_qml_module(kfw_workspace_qml
    URI KitNote.Workspace
    VERSION 1.0
    QML_FILES
        WorkspacesDialog.qml
        NewWorkspaceDialog.qml
    IMPORTS
        TARGET kfw_ui_qml
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(kfw_workspace_qml)
