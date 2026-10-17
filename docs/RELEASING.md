# Releasing KitNote

A release is a Git tag `v<version>` on `main`. Pushing the tag starts `.github/workflows/release.yml`, which builds
Windows and macOS, and creates the GitHub Release with the packages, `SHA256SUMS.txt` and the release notes.

## Before the first release

- The GitHub repository must exist and be the `origin` of this checkout. Actions must be enabled for it.
- Optional, for a signed and notarized macOS app: the repository secrets `KITNOTE_MACOS_CERT_P12`, `KITNOTE_MACOS_CERT_PASSWORD`, `KITNOTE_APPLE_ID`, `KITNOTE_APPLE_TEAM_ID` and `KITNOTE_APPLE_APP_PASSWORD` (the header of `build_macos.yml` says what each one is). Without them the macOS app is ad-hoc signed, and the README's first-start steps for macOS are the ones that apply.
- Run the release once by hand (see "Manual runs" below) and check the artifacts before you tag anything.
- Nothing in the release workflows has run on GitHub yet. Treat the first run as a test.

## Cutting a release

1. Set the version in `version.cmake`: `KITNOTE_APP_VERSION_MAJOR`, `KITNOTE_APP_VERSION_MINOR` and
   `KITNOTE_APP_VERSION_PATCH`. Nothing else holds the version: `package_windows.ps1`, the build and the release
   workflow all read it from there.
2. Add a section to `CHANGELOG.md` with the heading `## <version>` (for example `## 1.0.1`). The release notes are
   exactly that section, up to the next `## ` heading. A release without such a section fails.
3. Commit on `main` and wait until **Build: Windows** and **Build: macOS** pass on that commit.
4. Tag and push:

   ```bash
   git tag -a v1.0.1 -m "KitNote 1.0.1"
   git push origin v1.0.1
   ```

   The tag must be `v` followed by the version in `version.cmake`. The release job stops with an error if it is not,
   before anything is built.

5. When the run finishes, the release is public and lists:
   - `KitNote-<version>-win64.zip`, the Windows package,
   - the macOS `.dmg`,
   - `SHA256SUMS.txt`,
   - the source code of the tag (GitHub adds the source archives to every release; they are the source that the
     GPL-3.0 licence requires for the binaries).

## What the workflows do

| Workflow | When | What |
| --- | --- | --- |
| `build_windows.yml` ("Build: Windows") | push and pull request to `main`, manual, and from `release.yml` | Windows x64 on `windows-2025`. Installs Qt 6.10.2 with aqtinstall (`win64_msvc2022_64`, modules `qt5compat qtnetworkauth qtshadertools qtwebsockets`), sets up MSVC with `ilammy/msvc-dev-cmd`, makes the drum SoundFont with `tools/kitnote/make_drum_soundfont.py`, builds and installs with `ninja_build.sh -t install` and the options of `tools/kitnote/build_windows.bat`, then runs `smoke_test.py`, `mcp_test.py`, `test_audio_chart.py` and `test_install.ps1`. Packs `dist/KitNote-<version>-win64.zip` with `package_windows.ps1` and uploads it as the artifact `KitNote-windows`. |
| `build_macos.yml` ("Build: macOS") | the same triggers | Builds the macOS app and the `.dmg`, and uploads the artifact `KitNote-macos`. Its header says what signing it does. |
| `release.yml` ("Release") | a pushed tag `v*`, or a manual run | Checks the tag against `version.cmake`, calls both build workflows, downloads both artifacts, writes the release notes from `CHANGELOG.md` and `SHA256SUMS.txt`, and creates the release with `softprops/action-gh-release`. The macOS job gets the signing secrets with `secrets: inherit`. |

The build workflows take no required inputs. `build_windows.yml` has an optional `full_tests` flag, which adds
`stress_test.py` (about seven minutes); `release.yml` turns it on. `build_macos.yml` takes `build_mode`; the release
passes `stable`.

All third-party actions are pinned to a commit SHA, with the release tag in a comment. Dependabot
(`.github/dependabot.yml`) proposes newer versions of them every week.

The Windows artifact of a build run is kept for 30 days. The packages of a release stay with the release.

## Manual runs

- **Actions > Build: Windows > Run workflow** builds the Windows package from the chosen branch. Set `full_tests`
  to run the stress test too. Use this to check a change before it is merged.
- **Actions > Release > Run workflow** takes a `tag` such as `v1.0.1`. It builds both platforms and creates a
  **draft** release, which only people with write access to the repository can see. Check the files, then publish
  the draft on GitHub. A tag that does not exist yet is created at the commit the run uses. A tag that exists at another commit stops the run before anything is built.

## Testing the artifacts

Check the checksums first. On Windows (PowerShell):

```powershell
Get-FileHash -Algorithm SHA256 .\KitNote-1.0.0-win64.zip
```

On macOS or Linux: `shasum -a 256 -c SHA256SUMS.txt`. The line for each file must say `OK`.

Windows: unpack the zip and run `KitNote-1.0.0-win64\bin\KitNote.exe`. The package also has `install.ps1`, which
installs it for the user (`-Uninstall` removes it). From a checkout of the same version, the command-line check
runs against the unpacked copy:

```powershell
python tools\kitnote\smoke_test.py "C:\path\to\KitNote-1.0.0-win64\bin\KitNote.exe"
```

`smoke_test.py` and `test_install.ps1` are Windows-only. `test_install.ps1` needs a build in `build.install`
of the checkout. The other checks run on the files in the zip.

macOS: open the `.dmg`, drag KitNote into Applications, and start it. CI runs `buildscripts/ci/macos/smoke_test.sh` on the app (bundle, signature, architectures, `--version`, one PDF export). The Windows command-line tests do not run on macOS, so check these by hand: open a MusicXML file, export PDF, MIDI and WAV, save as `.kitnote` and open it again.

Test a score with a pitched instrument as well: KitNote must refuse to open it and say which part it is.

## Signing and warnings

- **Windows.** The executable and the zip are not signed with a code-signing certificate. Windows SmartScreen shows
  "Windows protected your PC" the first time; users choose *More info*, then *Run anyway*. Signing would need a
  certificate and a secret in the repository; the Windows workflow does not do it yet.
- **macOS.** Signing and notarization are optional. Read the header of `build_macos.yml`: it says which Apple secrets
  the workflow needs, and what it does with them. An app that is not notarized is blocked by Gatekeeper on the first
  start; users then open *System Settings > Privacy & Security* and click *Open Anyway* (on macOS 14 and older, right-click the app and choose *Open* also works). When the app is notarized, update the first-start steps in the README.

## Keeping the two Windows builds the same

`tools/kitnote/build_windows.bat` (for building on a PC) and `.github/workflows/build_windows.yml` (for CI) set the
same `KITNOTE_*` options. When one changes, change the other.
