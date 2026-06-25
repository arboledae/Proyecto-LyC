; ============================================================================
;  Instalador de Windows para el Compilador Dummy (Grupo 5)
;  Genera CompiladorLyC-Setup.exe con Inno Setup 6 (https://jrsoftware.org/).
;
;  Empaqueta:
;    - compilador.exe / analizador_lexico.exe  (compilados con MSYS2/MinGW)
;    - Python embebido                          (installer\python\)
;    - La interfaz grafica web                  (gui\)
;    - Los ejemplos                             (ejemplos\)
;
;  Al instalar crea un acceso directo "Compilador LyC". Al abrirlo arranca el
;  servidor local y se abre solo el navegador predeterminado con el programa.
;
;  Compilar este script:  ISCC.exe windows.iss   (o usar construir.bat)
; ============================================================================

#define AppName "Compilador LyC"
#define AppVersion "1.0.0"
#define AppPublisher "Grupo 5 - UNMSM"
; Carpeta raiz del proyecto (este .iss vive en installer\)
#define RootDir ".."

[Setup]
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
DefaultDirName={autopf}\{#AppName}
DefaultGroupName={#AppName}
; Instalacion por usuario: no pide permisos de administrador.
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputDir={#RootDir}\installer\Output
OutputBaseFilename=CompiladorLyC-Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
; Python embebido y los .exe de MinGW son de 64 bits.
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
DisableProgramGroupPage=yes
UninstallDisplayName={#AppName}

[Languages]
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "Crear un acceso directo en el escritorio"; GroupDescription: "Accesos directos:"

[Files]
Source: "{#RootDir}\compilador.exe";         DestDir: "{app}"; Flags: ignoreversion
Source: "{#RootDir}\analizador_lexico.exe";  DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist
Source: "{#RootDir}\gui\server.py";          DestDir: "{app}\gui"; Flags: ignoreversion
Source: "{#RootDir}\gui\static\*";           DestDir: "{app}\gui\static"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#RootDir}\ejemplos\*";             DestDir: "{app}\ejemplos"; Flags: ignoreversion recursesubdirs createallsubdirs
; Python embebido: descargar y descomprimir en installer\python\ antes de compilar.
Source: "python\*";                          DestDir: "{app}\python"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
; El acceso directo arranca el servidor con python.exe (ventana de consola que
; sirve para detenerlo cerrandola). server.py abre el navegador solo.
Name: "{group}\{#AppName}"; Filename: "{app}\python\python.exe"; Parameters: """{app}\gui\server.py"""; WorkingDir: "{app}"
Name: "{group}\Desinstalar {#AppName}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\python\python.exe"; Parameters: """{app}\gui\server.py"""; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\python\python.exe"; Parameters: """{app}\gui\server.py"""; WorkingDir: "{app}"; Description: "Ejecutar {#AppName} ahora"; Flags: postinstall skipifsilent nowait
