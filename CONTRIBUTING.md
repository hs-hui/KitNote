# Contributing to KitNote / KitNote에 기여하기

KitNote is a drum-notation fork of MuseScore Studio 4.7.5. What it changes is listed in [KITNOTE.md](KITNOTE.md). Changes to MuseScore Studio itself belong upstream: see MuseScore's [Contributing page](https://github.com/musescore/MuseScore/wiki/Contributing).

KitNote는 MuseScore Studio 4.7.5를 고친 드럼 악보 편집기입니다. 바뀐 점은 [KITNOTE.md](KITNOTE.md)에 있습니다. MuseScore Studio 자체에 대한 수정은 MuseScore 쪽([Contributing](https://github.com/musescore/MuseScore/wiki/Contributing))에 보내 주세요.

## Rules / 규칙

- Keep changes to MuseScore's source small and mark each with a `KitNote:` comment; add a line for it to [KITNOTE.md](KITNOTE.md). `git diff v4.7.5` should show every change.
- Every modified file keeps its upstream copyright and SPDX lines unchanged and carries the line `Modified for KitNote, 2026` directly under the last copyright line (GPL-3.0, section 5(a)). New files get the same header as their neighbours.
- A new string in the interface needs a Korean translation in `share/locale/kitnote_ko.ts`.
- The user guide is written in `docs/kitnote/user-guide.en.md` and `user-guide.ko.md`; `python tools/kitnote/make_manual.py` turns them into `share/manual/kitnote/*.html`.
- Contributions are under GPL-3.0, like the rest of KitNote. Models and data that KitNote ships or downloads must have an open licence (MIT, BSD, Apache, CC BY, CC0, GPL); nothing non-commercial (NC). Record them in [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
- Do not commit personal paths, file names of your own recordings, or build output (`build.*/`, `dist/`).

- MuseScore 소스 수정은 작게 하고, 고친 곳마다 `KitNote:` 주석을 달고 [KITNOTE.md](KITNOTE.md)에 한 줄 적습니다.
- 화면에 새 문구를 넣으면 `share/locale/kitnote_ko.ts`에 한국어 번역을 넣습니다.
- 사용법은 `docs/kitnote/user-guide.*.md`에 쓰고 `python tools/kitnote/make_manual.py`로 HTML을 만듭니다.
- 기여한 코드는 GPL-3.0입니다. 포함하거나 내려받는 모델·데이터는 공개 라이선스(MIT, BSD, Apache, CC BY, CC0, GPL)여야 하며 비상업(NC) 라이선스는 쓰지 않습니다. [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)에 적어 주세요.
- 개인 경로, 본인 녹음 파일 이름, 빌드 결과물(`build.*/`, `dist/`)은 커밋하지 않습니다.

## Build (Windows) / 빌드

Requirements / 준비물: Visual Studio with the C++ desktop workload (MSVC), CMake, Ninja, Git for Windows (for `bash`), Python 3, Qt **6.10.2** `msvc2022_64` with `qt5compat qtnetworkauth qtshadertools qtwebsockets`. Nothing else is unpacked by hand: CMake downloads the dependencies listed in [Build-time downloads](#build-time-downloads--빌드-중-내려받는-파일) during configure. The `dependencies/` folder that older instructions mention is not used by the build any more.

```bash
python -m aqt install-qt windows desktop 6.10.2 win64_msvc2022_64 -m qt5compat qtnetworkauth qtshadertools qtwebsockets -O C:\Qt
```

Once, generate the drum-only SoundFont / 한 번만, 드럼 전용 사운드폰트 만들기:

```bash
python tools/kitnote/make_drum_soundfont.py "share/sound/MS Basic.sf3" "share/sound/KitNote Drums.sf3"
```

Build and install to `build.install\bin\KitNote.exe` / 빌드와 설치:

```bat
set QTDIR=C:\Qt\6.10.2\msvc2022_64
tools\kitnote\build_windows.bat
```

`build_windows.bat` finds Visual Studio, sets up MSVC and Qt, and runs `bash ./ninja_build.sh -t install` in release mode. Run `ninja_build.sh` through Git Bash, not `ninja_build.bat` (see [KITNOTE.md](KITNOTE.md#building-on-windows)). Close KitNote before building, or the install step cannot replace `KitNote.exe`.

`build_windows.bat`는 Visual Studio와 Qt 환경을 잡고 `ninja_build.sh -t install`을 릴리스 모드로 실행합니다. 빌드 전에 KitNote를 닫아 주세요.

Packaging / 패키징: `powershell -ExecutionPolicy Bypass -File tools\kitnote\package_windows.ps1` writes `dist/KitNote-<version>-win64.zip`.

## Build-time downloads / 빌드 중 내려받는 파일

A normal Windows build downloads nothing from MuseScore servers, from `github.com/musescore/*`, from `raw.githubusercontent.com/musescore/*` or from `muse_deps`. zlib, HarfBuzz and the ASIO SDK are vendored in `thirdparty/`, so a fresh clone configures and builds offline, apart from the Qt install. / 일반적인 Windows 빌드는 MuseScore 서버, `github.com/musescore/*`, `raw.githubusercontent.com/musescore/*`, `muse_deps`에서 아무것도 받지 않습니다. zlib, HarfBuzz, ASIO SDK는 `thirdparty/`에 들어 있어서, Qt 설치 외에는 네트워크 없이 구성하고 빌드합니다.

Vendored (in the repository) / 저장소에 들어 있는 것:

| Component / 구성 요소 | Where / 위치 | Version / 버전 | Licence / 라이선스 |
| --- | --- | --- | --- |
| zlib | `thirdparty/zlib/` (static library `zlibstat`, used on MSVC) | 1.3.2, from zlib.net (SHA-256 `bb329a0a...`) | zlib licence, `thirdparty/zlib/LICENSE` |
| HarfBuzz | `thirdparty/harfbuzz/` (`src/` unchanged, built as one unity file) | 12.3.0, from github.com/harfbuzz/harfbuzz (SHA-256 `8660ebd3...`) | "Old MIT", `thirdparty/harfbuzz/COPYING` |
| Steinberg ASIO SDK (only the files the driver needs) | `thirdparty/asiosdk/ASIOSDK/` | 2.3.4, from download.steinberg.net (SHA-256 `d5ebf0c2...`) | Steinberg ASIO SDK licence: GPLv3 variant chosen, `thirdparty/asiosdk/ASIOSDK/LICENSE.txt` |

Off by default, so not downloaded / 기본값으로 꺼져 있어 받지 않는 것:

| Item / 항목 | Switch / 스위치 | What happens when it is on / 켜면 일어나는 일 |
| --- | --- | --- |
| MNX importer and exporter (mnxdom, and the JSON libraries it fetches from GitHub, pinned by commit) / MNX 가져오기·내보내기 | `KITNOTE_BUILD_IMPEXP_MNX_MODULE` (OFF) | Configure fetches mnxdom and its dependencies from GitHub. With it off, File > Import/Export does not offer MNX. / 켜면 구성 시 GitHub에서 받습니다. 끄면 MNX가 나오지 않습니다. |
| VST3 SDK / VST3 SDK | `KFW_MODULE_VST` (OFF) | Configure stops with an error. Download the SDK from Steinberg and set `KFW_MODULE_VST_VST3_SDK_PATH` to its folder. / 구성이 오류로 멈추며, Steinberg에서 받아 `KFW_MODULE_VST_VST3_SDK_PATH`에 지정해야 합니다. |
| SoundFont refresh / 사운드폰트 갱신 | removed / 제거됨 | Not needed: KitNote ships `share/sound/KitNote Drums.sf3`. / 필요 없음: KitNote에 `KitNote Drums.sf3`가 들어 있습니다. |
| googletest (unit tests only) / 단위 테스트 | `KFW_ENABLE_UNIT_TESTS` (OFF) | Only the unit tests of the `kors_async` and `kors_rpcqueue` modules. The SSH URL needs a GitHub key. / 단위 테스트에서만 받습니다. |

Installed by hand, or after the user agrees / 직접 설치하거나 사용자가 동의한 뒤 설치하는 것:

- Qt 6.10.2 (`aqtinstall` or the Qt installer). / Qt 6.10.2 (`aqtinstall` 또는 Qt 설치 프로그램).
- Audio engine (Python, PyTorch, Demucs, Beat This!, uv): not at build time; only after the user agrees in KitNote. / 오디오 엔진: 빌드 중에는 받지 않고, 사용자가 KitNote에서 동의한 뒤에만 받습니다.

CI-only / CI에서만 쓰는 것:

- `buildscripts/ci/checkcodestyle/download_tools.cmake` used to fetch the codestyle tools from `musescore/muse_framework` and `musescore/muse_deps`. It no longer downloads them. It stops with an error unless the tools are vendored at `muse/tools/codestyle/` and `muse_deps/`, so the codestyle CI check fails until someone vendors them. / 예전에는 MuseScore 저장소에서 코드 스타일 도구를 받았습니다. 지금은 받지 않으며, 도구를 저장소에 넣기 전까지 코드 스타일 검사가 실패합니다.
- `buildscripts/ci/macos/` fetches Qt and dependencies from GitHub repositories of MuseScore developers (`cbjeukendrup/*`). Replace them with the owner's own copies before using the macOS CI. / macOS CI는 MuseScore 개발자의 GitHub 저장소(`cbjeukendrup/*`)에서 받습니다. 소유자의 저장소로 바꾼 뒤 쓰세요.

Owner decisions still open / 소유자가 정할 일: vendor the codestyle tools (or drop that check); replace the macOS CI downloads. / 코드 스타일 도구를 저장소에 넣을지(또는 검사를 뺄지), macOS CI의 내려받기를 바꿀지.

## Tests / 테스트

Against the installed build (`build.install\bin`) / 설치된 빌드로 실행:

```bash
python tools/kitnote/smoke_test.py [path\to\KitNote.exe]
```

checks the version, the `.kitnote` round trip, every import and export format, the templates and user guide, the drum commands, the refusal of scores with pitched instruments, and the playback of ghost notes and open hi-hats. Run it before every package. / 버전, `.kitnote` 저장·열기, 모든 가져오기·내보내기 형식, 드럼 명령, 음정 악기 악보 거부, 고스트 노트·오픈 하이햇 재생을 확인합니다. 패키징 전에 꼭 실행하세요.

```bash
python tools/kitnote/mcp_test.py <folder>
```

talks to `build.install\bin\kitnote-mcp.exe` as an AI tool would (JSON-RPC over stdio) and writes its files into `<folder>` (a temporary folder when left out); it ends with "mcp: all checks passed". / AI 도구처럼 MCP 서버와 통신해 확인하고, 결과 파일은 `<folder>`에 씁니다.

Longer checks / 더 긴 점검: `python tools/kitnote/stress_test.py` (about seven minutes: every pattern and rudiment on every template, chained with the other drum commands) and `powershell -ExecutionPolicy Bypass -File tools\kitnote\test_install.ps1` (the installer, against a scratch folder and registry key).

## Retraining the drum model / 드럼 모델 다시 학습하기

KitNote's drum-stroke model (`share/ai/kitnote_drum_model.py`, weights `share/ai/kitnote_drums.pt`) is trained locally with the scripts in `tools/kitnote/`. They need Python with PyTorch (a GPU helps much), NumPy, SciPy, soundfile, mido and (for 7z kits) py7zr. Only data whose licence allows an open-source model is used - CC BY, CC BY-SA, CC0, MIT, GPL and the like, never "non-commercial"; [share/ai/kitnote_drums.NOTICE.txt](share/ai/kitnote_drums.NOTICE.txt) lists every source. Keep the data folders outside the repository; they may be several (`--data <folder> <folder> ...`), and the cache of decoded audio is best on a fast disk (`--cache`).

KitNote의 드럼 타격 모델은 `tools/kitnote/`의 스크립트로 직접 학습합니다. 오픈소스 모델에 쓸 수 있는 라이선스(CC BY, CC BY-SA, CC0, MIT, GPL 등, 비상업 조건 없음)의 자료만 쓰며, 출처는 모두 NOTICE 파일에 있습니다. 데이터 폴더는 저장소 밖에 두세요.

1. **Groove MIDI Dataset**: download <https://storage.googleapis.com/magentadata/datasets/groove/groove-v1.0.0.zip> and unpack it so that `<data>/groove/info.csv` exists. / 내려받아 `<data>/groove/info.csv`가 생기게 풉니다.
2. **E-GMD subset** (the same performances on 43 kits, without the whole 96 GB archive; only the chosen files are fetched over HTTP ranges) / E-GMD 일부만 받기:
   ```bash
   python tools/kitnote/fetch_egmd_subset.py --out <data> [--minutes-per-kit 120] [--valid-minutes-per-kit 3]
   ```
   writes `<data>/e-gmd-v1.0.0/...` and `<data>/e-gmd-subset.csv`.
3. **More performances** (optional): grooves of 26 styles - rock to blast beats, 6/8, jazz, 5/4 - with fills, flams and every drum struck together with every other / 여러 스타일의 그루브 MIDI 만들기(선택):
   ```bash
   python tools/kitnote/random_grooves.py --out <data>/grooves --count 8000 --seed 2
   ```
4. **Sampled kits** (optional): DrumGizmo kits (<https://drumgizmo.org>), SFZ kits, one-shot folders, and - turned into SFZ by `convert_kits.py` - Hydrogen kits, Drumlabooh kits and the drum presets of SoundFonts; `kit_maps.py` says which sound plays which stroke where a kit's names do not. Play the performances on them / 샘플 킷으로 렌더링(선택):
   ```bash
   python tools/kitnote/render_kits.py --data <data> --kits <kit> [<kit> ...] [--midi groove | --midi grooves --share] [--out-name rendered_x]
   ```
   writes `<data>/<out-name>/<kit>/<n>.wav` and `.mid` (the performance as played: a little faster or slower, softer or harder, the kit's microphones mixed differently each time; strokes a kit has no sound for are left out of the labels too) and the table `<data>/<out-name>/<out-name>.csv`.
5. **Drums as separated from songs** (optional, slow): put rendered performances into songs - a bass on the bass drum, chords, a lead, pads, played with MuseScore's MS Basic SoundFont - and take the drums out again with Demucs, as KitNote does; run with KitNote's engine Python / 노래에 넣었다가 Demucs로 다시 분리한 드럼(선택, 느림):
   ```bash
   python tools/kitnote/separated_stems.py --data <data> --sources rendered_x [--workers N --worker k] [--until 17:00]
   ```
6. **Train** / 학습:
   ```bash
   python tools/kitnote/train_drum_model.py --data <data> [<data> ...] --out kitnote_drums.pt [--steps 120000 --width 64 --hidden 192] [--cache <fast folder>] [--refresh 5000]
   ```
   uses whatever of `groove/`, `e-gmd-subset.csv` and the `rendered*/` tables is in each folder (`--refresh`: tables written while it trains join in), and changes the audio while training (level, colour, room, drive, noise, other instruments, what a separation leaves) so that the model also hears drums separated from a song. Copy the result to `share/ai/kitnote_drums.pt`. If you add a data source, add it to `kitnote_drums.NOTICE.txt` and [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

   `<data>`에 있는 자료를 모두 써서 학습하고, 결과를 `share/ai/kitnote_drums.pt`로 복사합니다. 새 데이터를 쓰면 NOTICE와 THIRD_PARTY_NOTICES.md에 추가하세요.
