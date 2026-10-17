kfw_create_qml_module(kfw_shortcuts_qml ALIAS kfw::shortcuts_qml FOR kfw_shortcuts)

qt_add_qml_module(kfw_shortcuts_qml
    URI KitNote.Shortcuts
    VERSION 1.0
    SOURCES
        editshortcutmodel.cpp
        editshortcutmodel.h
        shortcutsinstancemodel.cpp
        shortcutsinstancemodel.h
        shortcutsmodel.cpp
        shortcutsmodel.h
    QML_FILES
        EditShortcutDialogContent.qml
        internal/ShortcutsBottomPanel.qml
        internal/ShortcutsList.qml
        internal/ShortcutsTopPanel.qml
        Shortcuts.qml
        ShortcutsPage.qml
        StandardEditShortcutDialog.qml
    IMPORTS
        TARGET kfw_ui_qml
        TARGET kfw_uicomponents_qml
)

fixup_qml_module_dependencies(kfw_shortcuts_qml)

if (OS_IS_MAC)
    find_library(CARBON_LIBRARY Carbon)

    target_sources(kfw_shortcuts_qml PRIVATE
        platform/macos/macosshortcutsinstancemodel.mm
        platform/macos/macosshortcutsinstancemodel.h
    )

    set_source_files_properties(
        platform/macos/macosshortcutsinstancemodel.mm
        PROPERTIES
        SKIP_UNITY_BUILD_INCLUSION ON
        SKIP_PRECOMPILE_HEADERS ON
    )

    target_include_directories(kfw_shortcuts_qml PRIVATE
        ${CARBON_LIBRARY}
    )

    # Necessary for the auto-generated sources
    target_include_directories(kfw_shortcuts_qml PRIVATE platform/macos)
endif(OS_IS_MAC)
