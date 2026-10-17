REM @echo off
ECHO "KitNote build"

SET ARTIFACTS_DIR=build.artifacts
SET INSTALL_DIR=../build.install
SET BUILD_NUMBER=""
SET TARGET_PROCESSOR_BITS=64

:GETOPTS
IF /I "%1" == "-n" SET BUILD_NUMBER=%2 & SHIFT
IF /I "%1" == "-b" SET TARGET_PROCESSOR_BITS=%2 & SHIFT
SHIFT
IF NOT "%1" == "" GOTO GETOPTS

IF %BUILD_NUMBER% == "" ( ECHO "error: not set BUILD_NUMBER" & EXIT /b 1)
IF NOT %TARGET_PROCESSOR_BITS% == 64 (
    IF NOT %TARGET_PROCESSOR_BITS% == 32 (
        ECHO "error: not set TARGET_PROCESSOR_BITS, must be 32 or 64, current TARGET_PROCESSOR_BITS: %TARGET_PROCESSOR_BITS%"
        EXIT /b 1
    )
)

SET /p BUILD_MODE=<%ARTIFACTS_DIR%\env\build_mode.env
SET "KITNOTE_APP_BUILD_MODE=dev"
IF %BUILD_MODE% == devel   ( SET "KITNOTE_APP_BUILD_MODE=dev" ) ELSE (
IF %BUILD_MODE% == nightly ( SET "KITNOTE_APP_BUILD_MODE=dev" ) ELSE (
IF %BUILD_MODE% == testing ( SET "KITNOTE_APP_BUILD_MODE=testing" ) ELSE (
IF %BUILD_MODE% == stable  ( SET "KITNOTE_APP_BUILD_MODE=release" ) ELSE (
    ECHO "error: unknown BUILD_MODE: %BUILD_MODE%"
    EXIT /b 1
))))

ECHO "KITNOTE_APP_BUILD_MODE: %KITNOTE_APP_BUILD_MODE%"
ECHO "BUILD_NUMBER: %BUILD_NUMBER%"
ECHO "TARGET_PROCESSOR_BITS: %TARGET_PROCESSOR_BITS%"

REM KitNote: zlib, HarfBuzz and the ASIO SDK are vendored under thirdparty/; nothing is fetched here.

SET "KITNOTE_BUILD_CONFIGURATION=app"

bash ./buildscripts/ci/tools/make_revision_env.sh 
SET /p KITNOTE_REVISION=<%ARTIFACTS_DIR%\env\build_revision.env

SET KITNOTE_BUILD_CONFIGURATION=%KITNOTE_BUILD_CONFIGURATION%
SET KITNOTE_APP_BUILD_MODE=%KITNOTE_APP_BUILD_MODE%
SET KITNOTE_BUILD_NUMBER=%BUILD_NUMBER%
SET KITNOTE_REVISION=%KITNOTE_REVISION%
SET KITNOTE_INSTALL_DIR=%INSTALL_DIR%
SET KITNOTE_BUILD_VST_MODULE="ON"
SET KITNOTE_BUILD_WEBSOCKET="ON"
SET KITNOTE_RUN_WINDEPLOYQT="ON"

CALL ninja_build.bat -t installrelwithdebinfo || exit \b 1

bash ./buildscripts/ci/tools/make_release_channel_env.sh -c %KITNOTE_APP_BUILD_MODE%
bash ./buildscripts/ci/tools/make_version_env.sh %BUILD_NUMBER%
bash ./buildscripts/ci/tools/make_branch_env.sh
