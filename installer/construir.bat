@echo off
REM ===========================================================================
REM  Empaqueta el instalador de Windows del Compilador LyC (Grupo 5).
REM  Requisitos previos (ver BUILD.md):
REM    1) compilador.exe ya compilado en la raiz del proyecto (con MSYS2).
REM    2) Python embebido descomprimido en installer\python\.
REM    3) Inno Setup 6 instalado.
REM  Uso:  doble clic, o desde cmd:  installer\construir.bat
REM ===========================================================================
setlocal

set "INSTDIR=%~dp0"
set "ROOT=%INSTDIR%.."

echo === Verificando requisitos ===

if not exist "%ROOT%\compilador.exe" (
    echo [ERROR] Falta compilador.exe en la raiz del proyecto.
    echo         Compilalo primero con MSYS2:  make   (ver BUILD.md^)
    goto :error
)

if not exist "%INSTDIR%python\python.exe" (
    echo [ERROR] Falta Python embebido en installer\python\python.exe
    echo         Descarga el "Windows embeddable package x86-64" de python.org
    echo         y descomprimelo en installer\python\  (ver BUILD.md^)
    goto :error
)

REM WinAPE: se descarga a tools\winape para que el instalador lo empaquete
REM (boton "Ejecutar en WinAPE" de la interfaz).
if not exist "%ROOT%\tools\winape\WinApe.exe" (
    echo === Descargando WinAPE ===
    powershell -ExecutionPolicy Bypass -File "%ROOT%\tools\descargar_winape.ps1"
    if not exist "%ROOT%\tools\winape\WinApe.exe" (
        echo [AVISO] No se pudo descargar WinAPE: el instalador se generara
        echo         sin el emulador y el boton Ejecutar pedira instalarlo.
    )
)

REM Buscar el compilador de Inno Setup (ISCC.exe).
set "ISCC="
if exist "%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe" set "ISCC=%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe"
if exist "%ProgramFiles%\Inno Setup 6\ISCC.exe" set "ISCC=%ProgramFiles%\Inno Setup 6\ISCC.exe"
if "%ISCC%"=="" (
    where iscc >nul 2>nul && set "ISCC=iscc"
)
if "%ISCC%"=="" (
    echo [ERROR] No se encontro ISCC.exe (Inno Setup 6^).
    echo         Instalalo desde https://jrsoftware.org/isdl.php
    goto :error
)

echo === Generando el instalador con Inno Setup ===
"%ISCC%" "%INSTDIR%windows.iss"
if errorlevel 1 goto :error

echo.
echo === LISTO ===
echo Instalador generado en:  installer\Output\CompiladorLyC-Setup.exe
goto :fin

:error
echo.
echo Se detuvo el proceso por un error. Revisa los mensajes de arriba.
exit /b 1

:fin
endlocal
pause
