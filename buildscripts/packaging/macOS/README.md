# KitNote for macOS: build, package, sign

Written without a Mac at hand. The first real run is the CI job `.github/workflows/build_macos.yml`
on a GitHub macOS runner; treat its first result as the test.

## What the build needs

- Qt 6.10.2 `clang_64` (universal) from the official Qt mirrors, through aqtinstall
  (`buildscripts/ci/macos/install_qt.sh`). Modules: `qt5compat qtnetworkauth qtshadertools qtwebsockets`,
  the same as Windows.
- Xcode 26 (Icon Composer icons and the Swift Quick Look extension). `ninja` and `ccache` from Homebrew.
- Nothing else. The third-party code KitNote uses (opus, opusenc, flac, lame, fdk-aac, freetype,
  HarfBuzz, zlib, stb, dr_libs, the ASIO SDK) is in the repository and is built with the app.
  No dependency is fetched from a MuseScore host or from a personal repository.
- The drum SoundFont is made by `tools/kitnote/make_drum_soundfont.py` before the build (stdlib Python).

## Build

    export QT_ROOT_DIR=$HOME/Qt/6.10.2/macos CMAKE_PREFIX_PATH=$HOME/Qt/6.10.2/macos
    bash ./buildscripts/ci/macos/build.sh -n 1

Architectures: `KITNOTE_MACOS_ARCHITECTURES` (default `arm64;x86_64`, universal). The minimum macOS
is `KITNOTE_MACOS_DEPLOYMENT_TARGET` in `buildscripts/cmake/SetupBuildEnvironment.cmake` (default 14.4,
taken from Qt's macOS page; check it against Qt 6.10 before a release).

## Package

    bash ./buildscripts/ci/macos/package.sh          # build.artifacts/KitNote-<version>-macos-<arch>.dmg
    bash ./buildscripts/ci/macos/smoke_test.sh applebuild/KitNote.app

The install puts `kitnote-mcp` into `Contents/MacOS` (`src/kitnotemcp/CMakeLists.txt`); `buildscripts/packaging/macOS/package.sh`
checks it is there (`--mcp <file>` copies another one in), runs `macdeployqt`, signs, makes the disk image. Inside the app: `Contents/Resources/ai` (skill, audio engine, drum model),
`Contents/Resources/sound/MS Basic.sf3` (the drum SoundFont), the templates, fonts and the
Quick Look extension in `Contents/PlugIns`.

## Signing

- Without secrets the app is ad-hoc signed. It runs on Apple silicon, but Gatekeeper stops the first
  start of a downloaded copy. The user allows it in System Settings > Privacy & Security > Open Anyway.
- With secrets the app is signed with a Developer ID Application certificate (hardened runtime, timestamp)
  and notarized, the app is stapled, and so is the disk image:

| Secret | Use |
| --- | --- |
| `KITNOTE_MACOS_CERT_P12` | Developer ID Application certificate and key, `.p12`, base64 |
| `KITNOTE_MACOS_CERT_PASSWORD` | password of the `.p12` |
| `KITNOTE_APPLE_ID` | Apple ID for `notarytool` |
| `KITNOTE_APPLE_TEAM_ID` | team ID |
| `KITNOTE_APPLE_APP_PASSWORD` | app-specific password of that Apple ID |

Pass them to the workflow from the repository secrets (`secrets: inherit` when a release workflow calls it).
Forks' pull requests get no secrets, so they are ad-hoc signed.

## Known gaps

- The audio engine is installed on a Mac by `share/ai/install_engine.sh` (sh, into
  `~/Library/Application Support/KitNote/ai`, the same layout as Windows). It has not been run on a Mac yet:
  check its uv SHA-256 values against the uv 0.13.0 release, and the Intel PyTorch pins (torch 2.2.2) against
  `beat-this` on the first run.
- The Quick Look extension is arm64-only (its Swift part is built for the host). It is loaded from the
  app's `Resources` through a symlink made at install time; check that preview works on a real Mac.
- `tools/kitnote/smoke_test.py` expects the Windows layout and is not used on macOS.
