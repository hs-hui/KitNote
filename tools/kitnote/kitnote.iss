; The Windows installer of KitNote (Inno Setup 6): KitNote-<version>-win64-setup.exe.
; tools\kitnote\package_windows.ps1 builds it from the packed program folder:
;
;   ISCC.exe /DAppVersion=1.0.0 /DSourceDir=<dist\KitNote-1.0.0-win64> /DOutputDir=<dist> tools\kitnote\kitnote.iss
;
; It installs for the current user by default (no administrator rights, %LOCALAPPDATA%\Programs\KitNote, the
; same place as install.ps1) and offers an installation for all users. It adds Start menu and desktop shortcuts,
; opens .kitnote files with KitNote and adds KitNote to "Installed apps". Scores and settings are never touched.

#ifndef AppVersion
  #error Pass /DAppVersion=<version>
#endif
#ifndef SourceDir
  #error Pass /DSourceDir=<the packed KitNote folder>
#endif
#ifndef OutputDir
  #define OutputDir "."
#endif

#define RepoRoot AddBackslash(SourcePath) + "..\.."

[Setup]
AppId={{6C1E3B9A-6F4D-4E0B-9A57-3B2D8E1F4C21}
AppName=KitNote
AppVersion={#AppVersion}
AppVerName=KitNote {#AppVersion}
AppPublisher=KitNote contributors
AppPublisherURL=https://github.com/hs-hui/KitNote
AppSupportURL=https://github.com/hs-hui/KitNote/issues
AppUpdatesURL=https://github.com/hs-hui/KitNote/releases
VersionInfoVersion={#AppVersion}
VersionInfoDescription=KitNote setup
DefaultDirName={autopf}\KitNote
DefaultGroupName=KitNote
DisableProgramGroupPage=yes
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
MinVersion=10.0
ChangesAssociations=yes
CloseApplications=yes
RestartApplications=no
SetupIconFile={#RepoRoot}\share\icons\AppIcon\kitnote_AppIcon.ico
UninstallDisplayIcon={app}\bin\KitNote.exe
UninstallDisplayName=KitNote {#AppVersion}
WizardStyle=modern
Compression=lzma2/max
SolidCompression=yes
OutputDir={#OutputDir}
OutputBaseFilename=KitNote-{#AppVersion}-win64-setup

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "korean"; MessagesFile: "compiler:Languages\Korean.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "fileassoc"; Description: "{cm:AssocFileExtension,KitNote,.kitnote}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
; (install.ps1 and register_filetype.ps1 belong to the zip, which is installed without this program)
Source: "{#SourceDir}\*"; DestDir: "{app}"; Excludes: "install.ps1,register_filetype.ps1"; Flags: ignoreversion recursesubdirs createallsubdirs

[InstallDelete]
; what an installation made from the zip with install.ps1 left in the same folder
Type: files; Name: "{app}\install.ps1"
Type: files; Name: "{app}\register_filetype.ps1"

[Icons]
Name: "{autoprograms}\KitNote"; Filename: "{app}\bin\KitNote.exe"; WorkingDir: "{app}\bin"; Comment: "KitNote drum notation editor"
Name: "{autodesktop}\KitNote"; Filename: "{app}\bin\KitNote.exe"; WorkingDir: "{app}\bin"; Comment: "KitNote drum notation editor"; Tasks: desktopicon

[Registry]
; An installation made from the zip with install.ps1 is replaced by this one: its "Installed apps" entry goes (its
; uninstall would delete this program), and, for an installation for all users, its per-user association, which
; would otherwise go on opening .kitnote files with the old copy.
Root: HKCU; Subkey: "Software\Microsoft\Windows\CurrentVersion\Uninstall\KitNote"; ValueType: none; Flags: deletekey dontcreatekey
Root: HKCU; Subkey: "Software\Classes\KitNote.Score"; ValueType: none; Flags: deletekey dontcreatekey; Check: IsAdminInstallMode
Root: HKA; Subkey: "Software\Classes\.kitnote"; ValueType: string; ValueName: ""; ValueData: "KitNote.Score"; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.kitnote\OpenWithProgids"; ValueType: string; ValueName: "KitNote.Score"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\KitNote.Score"; ValueType: string; ValueName: ""; ValueData: "KitNote score"; Flags: uninsdeletekey; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\KitNote.Score\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\KitNote.exe"",1"; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\KitNote.Score\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\KitNote.exe"" ""%1"""; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\Applications\KitNote.exe\SupportedTypes"; ValueType: string; ValueName: ".kitnote"; ValueData: ""; Flags: uninsdeletekey

[Run]
Filename: "{app}\bin\KitNote.exe"; Description: "{cm:LaunchProgram,KitNote}"; Flags: nowait postinstall skipifsilent
