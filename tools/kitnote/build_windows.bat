@echo off
REM Builds and installs KitNote into build.install\bin.
REM Usage: set QTDIR=C:\Qt\6.10.2\msvc2022_64 && tools\kitnote\build_windows.bat
REM See KITNOTE.md for the prerequisites.

if "%QTDIR%"=="" (
    echo Set QTDIR to the Qt 6.10.2 msvc2022_64 folder first.
    exit /b 1
)

set "VSWHERE=C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_INSTALL_DIR=%%i"
if "%VS_INSTALL_DIR%"=="" (
    echo Visual Studio with the C++ tools was not found.
    exit /b 1
)
call "%VS_INSTALL_DIR%\VC\Auxiliary\Build\vcvars64.bat"

cd /d "%~dp0..\.."
set "CMAKE_PREFIX_PATH=%QTDIR%"
set "PATH=%QTDIR%\bin;%VS_INSTALL_DIR%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"
set KITNOTE_RUN_WINDEPLOYQT=ON
set KITNOTE_APP_BUILD_MODE=release
REM Real build number (date) and revision instead of the placeholders in ninja_build.sh.
for /f %%i in ('git rev-parse --short HEAD') do set "KITNOTE_REVISION=%%i"
for /f %%i in ('powershell -NoProfile -Command "Get-Date -Format yyMMdd"') do set "KITNOTE_BUILD_NUMBER=%%i"

bash ./ninja_build.sh -t install
exit /b %ERRORLEVEL%
