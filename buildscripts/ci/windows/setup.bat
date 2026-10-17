@echo off
ECHO "Setup Windows build environment"

SET TARGET_PROCESSOR_BITS=64

:GETOPTS
IF /I "%1" == "-b" SET TARGET_PROCESSOR_BITS=%2 & SHIFT
SHIFT
IF NOT "%1" == "" GOTO GETOPTS

IF NOT %TARGET_PROCESSOR_BITS% == 64 (
    IF NOT %TARGET_PROCESSOR_BITS% == 32 (
        ECHO "Error: TARGET_PROCESSOR_BITS not set, must be 32 or 64, current TARGET_PROCESSOR_BITS: %TARGET_PROCESSOR_BITS%"
        EXIT /b 1
    )
)

IF %TARGET_PROCESSOR_BITS% == 32 (
    ECHO "Error: Build 32-bit not available"
    EXIT /b 1
)

:: Install tools
where /q git
IF ERRORLEVEL 1 ( choco install -y git.install )

where /q 7z
IF ERRORLEVEL 1 ( choco install -y 7zip.install )

:: Set temp dir
SET TEMP_DIR="c:\TEMP\musescore"
MKDIR %TEMP_DIR%

:: KitNote: nothing is downloaded from MuseScore servers here. zlib, HarfBuzz and the
:: ASIO SDK are vendored under thirdparty/, so CMake needs no network access for them.
:: The JACK driver is off in KitNote, so its files are not needed either.



:: Clean
ECHO "=== Clean ==="
RMDIR /Q /S "C:\TEMP"

ECHO "Setup script done"
