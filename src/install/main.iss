; Script generated by the Inno Script Studio Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "@CMAKE_PROJECT_NAME@"
#define MyAppVersion "@PROJECT_VERSION@"
#define MyAppPublisher "@DOODLE_PACKAGE_VENDOR@"
#define MyAppURL "https://github.com/uuiid/doodle"
#define MyAppExeName "DoodleExe.exe"

[Setup]
AppId={{EA8013D8-FB25-41C8-99D1-77410D1985CF}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={commonpf64}\{#MyAppName}
DefaultGroupName={#MyAppName}
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
AllowNoIcons=yes
OutputDir=${CMAKE_INSTALL_PREFIX}
OutputBaseFilename=${DOODLE_PACKAGE_NAME}
SetupIconFile=${PROJECT_SOURCE_DIR}/src/icon.ico
Compression=lzma
SolidCompression=yes
ChangesAssociations=yes

[Languages]
Name: "Chinese"; MessagesFile: "${innolagnuage_SOURCE_DIR}/ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
[Files]
Source: "${CMAKE_INSTALL_PREFIX}/bin/{#MyAppExeName}"; DestDir: "{app}/bin"; Flags: ignoreversion
Source: "${CMAKE_INSTALL_PREFIX}/bin/token.doodle_token"; DestDir: "{app}/bin"; Flags: ignoreversion
Source: "${CMAKE_INSTALL_PREFIX}/bin/doodle_maya*.exe"; DestDir: "{app}/bin"; Flags: ignoreversion
Source: "${CMAKE_INSTALL_PREFIX}/bin/*.dll"; DestDir: "{app}/bin"; Flags: ignoreversion recursesubdirs
Source: "${CMAKE_INSTALL_PREFIX}/houdini/*"; DestDir: "{app}/houdini"; Flags: ignoreversion recursesubdirs
Source: "${CMAKE_INSTALL_PREFIX}/maya/*"; DestDir: "{app}/maya"; Flags: ignoreversion recursesubdirs
Source: "${CMAKE_INSTALL_PREFIX}/SideFX_Labs/*"; DestDir: "{app}/SideFX_Labs"; Flags: ignoreversion recursesubdirs
Source: "${CMAKE_INSTALL_PREFIX}/ue427_Plug/*"; DestDir: "{app}/ue427_Plug"; Flags: ignoreversion recursesubdirs
Source: "${CMAKE_INSTALL_PREFIX}/ue52_Plug/*"; DestDir: "{app}/ue52_Plug"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\bin\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\bin\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Registry]
Root: "HKA"; Subkey: "SOFTWARE\Classes\doodle.main"; ValueType: string; ValueName: ""; ValueData: "doodle"; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: "HKA"; Subkey: "SOFTWARE\Classes\doodle.main\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\{#MyAppExeName}"; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: "HKA"; Subkey: "SOFTWARE\Classes\doodle.main\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\{#MyAppExeName}"" ""%1"""; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: "HKA"; Subkey: "SOFTWARE\Classes\.doodle_db"; ValueType: string;  ValueName: ""; ValueData: "doodle.main"; Flags: uninsdeletekeyifempty uninsdeletevalue
Root: "HKA"; Subkey: "SOFTWARE\Classes\.doodle_db\OpenWithProgids"; ValueType: string; ValueName: "doodle.main"; ValueData: ""; Flags: uninsdeletekeyifempty uninsdeletevalue