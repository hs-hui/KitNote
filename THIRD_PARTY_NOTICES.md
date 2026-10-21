# Third-party notices

KitNote is a modification of [MuseScore Studio](https://github.com/musescore/MuseScore) 4.7.5 and is distributed under the GNU General Public License version 3 ([LICENSE.txt](LICENSE.txt)). This file lists what KitNote adds on top of MuseScore Studio: what it bundles, what its audio engine downloads, and the data its drum model was trained on.

MuseScore is a trademark of MuseScore Limited. KitNote is an independent modification and is not affiliated with or endorsed by MuseScore Limited or Muse Group.

## Inherited from MuseScore Studio

MuseScore Studio has no central notice file; each component keeps its licence next to its source. KitNote keeps all of them unchanged. The main places:

| Component | Licence file |
| --- | --- |
| MuseScore Studio itself | [LICENSE.txt](LICENSE.txt) (GPL-3.0) |
| Music fonts (Bravura, Leland, Edwin, Campania) | [fonts/bravura/LICENSE.txt](fonts/bravura/LICENSE.txt), [fonts/leland/LICENSE.txt](fonts/leland/LICENSE.txt), [fonts/edwin/LICENSE.txt](fonts/edwin/LICENSE.txt), [fonts/campania/LICENSE](fonts/campania/LICENSE) |
| SoundFonts (MS Basic, FluidR3Mono) | [share/sound/MS Basic_License.md](share/sound/MS%20Basic_License.md), [share/sound/FluidR3Mono_License.md](share/sound/FluidR3Mono_License.md) |
| Audio libraries (FluidSynth, LAME, FLAC, Opus, libopusenc, fdk-aac, dr_libs, stb_vorbis ...) | `src/framework/audio/thirdparty/*/` |
| dr_libs (dr_wav, dr_flac, dr_mp3 by David Reid) and stb_vorbis (Sean Barrett) - KitNote decodes with them the recording a score plays along with (`src/framework/audio/common/audiofilereader.cpp`) | dr_libs: public domain or MIT-0; stb_vorbis: MIT or public domain - each at the user's choice, as stated at the end of each file in `src/framework/audio/thirdparty/dr_libs/` and `src/framework/audio/thirdparty/stb/` |
| Other bundled libraries | `thirdparty/`, `src/framework/*/thirdparty/`, `src/*/thirdparty/` |
| Qt 6 (shipped as DLLs in the Windows package) | LGPL-3.0 / GPL-3.0 depending on the module: <https://doc.qt.io/qt-6/licensing.html> |

## Bundled with KitNote

| Component | Licence | Where | Notes |
| --- | --- | --- | --- |
| [Pretendard](https://github.com/orioncactus/pretendard) 1.3.9 - interface font, four weights | SIL Open Font License 1.1 | [src/framework/ui/data/pretendard/LICENSE.txt](src/framework/ui/data/pretendard/LICENSE.txt) | Copyright (c) 2021 Kil Hyung-jin, Reserved Font Name "Pretendard". Compiled into the executable as Qt resources. |
| `share/sound/KitNote Drums.sf3` - the percussion presets of MS Basic, copied unchanged by `tools/kitnote/make_drum_soundfont.py` | MIT (as MS Basic, FluidR3Mono and FluidR3) | [share/sound/MS Basic_License.md](share/sound/MS%20Basic_License.md) | That licence asks for its acknowledgements to be kept in derivative works; the build installs `MS Basic_License.md` next to the SoundFont. |
| `share/ai/kitnote_drums.pt` - KitNote's own drum-stroke model (weights) | Model and training code: GPL-3.0 (part of KitNote). Training data: CC BY 4.0, CC BY-SA 3.0/4.0, CC0 1.0, public domain, MIT, GPL, WTFPL, GeneralUser GS License v2.0 (see below) | zlib 1.3.2 - vendored in `thirdparty/zlib/` and built as the static library `zlibstat` on Windows. Source: <https://zlib.net/zlib-1.3.2.tar.gz>, SHA-256 `bb329a0a2cd0274d05519d61c667c062e06990d72e125ee2dfa8de64f0119d16`. About 1.2 MB with the build file. | zlib licence | [thirdparty/zlib/LICENSE](thirdparty/zlib/LICENSE) | Used by the zip container code (`src/framework/global/serialization/internal/zipcontainer.cpp`). Unmodified sources. |
| HarfBuzz 12.3.0 - text shaping, vendored in `thirdparty/harfbuzz/` (`src/` unchanged; tests, docs and subprojects not included) and built with FreeType support. Source: <https://github.com/harfbuzz/harfbuzz/releases/tag/12.3.0>, SHA-256 `8660ebd3c27d9407fc8433b5d172bafba5f0317cb0bb4339f28e5370c93d42b7`. About 6.8 MB. | "Old MIT" (MIT-style), [thirdparty/harfbuzz/COPYING](thirdparty/harfbuzz/COPYING) | `thirdparty/harfbuzz/` | Build file: `thirdparty/harfbuzz/CMakeLists.txt`. Replaces the earlier muse_deps package, which had the same sources. |
| Steinberg ASIO SDK 2.3.4 - only the files the Windows ASIO driver uses (`ASIOSDK/common/`, `ASIOSDK/host/`, `ASIOSDK/host/pc/`), about 130 KB. Source: <https://download.steinberg.net/sdk_downloads/ASIO-SDK_2.3.4_2025-10-15.zip> (linked from <https://www.steinberg.net/developers/asiosdk-open/>), the official Steinberg download, SHA-256 `d5ebf0c20dd2c5f43771fd0c1418f4b361bf52434ee670097cfa6b3a335e2eca`. | Dual licence: the Steinberg ASIO licence, or GPL version 3. KitNote uses the GPLv3 option. [thirdparty/asiosdk/ASIOSDK/LICENSE.txt](thirdparty/asiosdk/ASIOSDK/LICENSE.txt) | `thirdparty/asiosdk/ASIOSDK/` | "ASIO" is a Steinberg trademark. The SDK's samples, logo files and PDFs are not included. |
| [share/ai/kitnote_drums.NOTICE.txt](share/ai/kitnote_drums.NOTICE.txt) | The weights file is added to the repository separately. |

### Data the drum model was trained on

[share/ai/kitnote_drums.NOTICE.txt](share/ai/kitnote_drums.NOTICE.txt) is the authoritative attribution. In short:

- **Groove MIDI Dataset** v1.0.0 (Gillick, Roberts, Engel, Eck, Bamman; Google Magenta) - CC BY 4.0. <https://magenta.tensorflow.org/datasets/groove>
- **Expanded Groove MIDI Dataset (E-GMD)** v1.0.0 (Callender, Hawthorne, Engel; Google Magenta) - CC BY 4.0, a subset chosen by `tools/kitnote/fetch_egmd_subset.py`. <https://magenta.tensorflow.org/datasets/e-gmd>
- Performances KitNote made itself (`tools/kitnote/random_grooves.py`, GPL-3.0).
- Sampled kits the performances were played on by `tools/kitnote/render_kits.py`:
  - **DRSKit 2.1**, **MuldjordKit 3**, **CrocellKit 1.1**, **The Aasimonster 2.1**, **ShittyKit 1.2** (DrumGizmo) - CC BY 4.0. <https://drumgizmo.org>
  - **StemGMD single hits**, ten acoustic kits (Mezza, Giampiccolo, Bernardini, Sarti; Politecnico di Milano) - CC BY 4.0. <https://zenodo.org/records/7882857>
  - **Virtuosity Drums** 0.925 (Versilian Studios); **Big Rusty Drums**, **Swirly Drums**, **Gogodze Phu Vol II** (Karoryfer Samples); **FreePats Synthesizer Percussion**; a Freesound jazz kit (bigjoedrummer, shpira) - CC0 1.0. **Avangard** (Peter Semiletov) - public domain.
  - **Salamander Drumkit** v1 (Alexander Holm) - CC BY-SA 3.0; **AVL Drumkits** (Glen MacArthur) and **BJA Pacific** (Bransin James Anderson, Luc Tanguay) - CC BY-SA 3.0; **Sam's Sonor** (Sam Greene) - CC BY-SA 4.0. Their share-alike condition applies, as their authors state, to the samples and to sample libraries made from them, not to other works.
  - **El Nacho drum kit** (Paolo D'Emilio) - CC BY 4.0.
  - Hydrogen kits by ArtemioLabs (TR808 Emulation, Electric Empire, Hard Electro 1), Sineshine (VariBreaks) and Emillo (Millo MultiLayered 3) - GPL; **SF3007-2011-Set-03** (Rainer Steffen Hain) - WTFPL.
  - The drum kits of **MS Basic** (MuseScore's SoundFont, MIT) and **GeneralUser GS** v2.0.3 (S. Christian Collins, GeneralUser GS License v2.0).
- Drums separated from songs made for training (`tools/kitnote/separated_stems.py`): the band played with MS Basic (MIT), the drums taken out with Demucs (MIT).

The audio changes applied during training (rooms, colour, drive, noise, tones, what a separation leaves) are KitNote's own.

## On the website (`website/`)

The website at <https://kitnote.hui.kr> is deployed from `website/` and is not part of the program. It ships:

- `website/assets/fonts/Pretendard-*.subset.woff2` - the official subset files of [Pretendard](https://github.com/orioncactus/pretendard) 1.3.9, unmodified. SIL Open Font License 1.1, [website/assets/fonts/LICENSE.txt](website/assets/fonts/LICENSE.txt).
- `website/assets/drums/kit.mp3` - one stroke of each drum, rendered by KitNote with its drum SoundFont (the percussion of MS Basic). MIT, [website/assets/drums/LICENSE-MS-Basic.md](website/assets/drums/LICENSE-MS-Basic.md).
- `website/assets/demo-groove.mp3` and `website/assets/score-page.webp` - a demo groove played and engraved by KitNote.

## Fonts shipped with KitNote

Every font file that the build installs (listed in `src/app/CMakeLists.txt` and the `.qrc` files under `src/engraving/data/fonts/`), with its licence. The interface font Pretendard is in the table above.

| Font family and files | Licence | Copyright and source | Licence text in the repository |
| --- | --- | --- | --- |
| Bravura, BravuraText (`fonts/bravura/`) | SIL Open Font License 1.1 | © 2015 Steinberg Media Technologies GmbH | `fonts/bravura/OFL.txt` |
| Campania (`fonts/campania/`) | SIL Open Font License 1.1 | © 2019 Marc Sabatella | `fonts/campania/LICENSE` |
| Edwin, four styles (`fonts/edwin/`) | SIL Open Font License 1.1 | © 2021 MuseScore Limited | `fonts/edwin/LICENSE.txt` |
| Leland, LelandText (`fonts/leland/`) | SIL Open Font License 1.1 | © 2025 MuseScore Limited | `fonts/leland/LICENSE.txt` |
| Gootville, GootvilleText (`fonts/gootville/`) | SIL Open Font License 1.1, as stated in `readme.txt` | Grzegorz Pruchniakowski, from SMuFL templates based on Bravura | [fonts/gootville/OFL.txt](fonts/gootville/OFL.txt) (OFL 1.1 text added here; the copyright line to be confirmed with the upstream author). |
| MuseJazz, MuseJazzText (`fonts/musejazz/`) | SIL Open Font License 1.1 | © 2016 Torsten Hämmerle, Abraham Lee | `fonts/musejazz/OFL.txt` |
| Petaluma, PetalumaText (`fonts/petaluma/`) | SIL Open Font License 1.1 | © 2018 Steinberg Media Technologies GmbH | `fonts/petaluma/OFL.txt` |
| Finale Maestro, Finale Maestro Text (`fonts/finalemaestro/`) | SIL Open Font License 1.1 | © 2021 MakeMusic, Inc. | `fonts/finalemaestro/OFL.txt` |
| Finale Broadway, Finale Broadway Text (`fonts/finalebroadway/`) | SIL Open Font License 1.1 | © 2021 MakeMusic, Inc. | `fonts/finalebroadway/OFL.txt` |
| FreeSans, FreeSerif in regular, bold, italic and bold italic (`fonts/FreeSans.ttf`, `fonts/FreeSerif*.ttf`) | GNU GPL version 3 or later, as stated in the font headers | GNU FreeFont contributors | The GPL text is `LICENSE.txt`; the font headers state the licence. |
| MScore (`fonts/mscore/MScore.otf`, the Emmentaler-derived music font) | GNU GPL version 3 or later with a font exception, as the font source states; the font clause of [LICENSE.txt](LICENSE.txt) covers all MuseScore fonts | MuseScore contributors (named in the font source); parts derived from Emmentaler (GNU LilyPond) and Parnassus | Font clause in [LICENSE.txt](LICENSE.txt); font source `fonts/mscore/mscore.sfd`. |
| MScoreText (`fonts/mscore/MScoreText.otf`) | SIL Open Font License 1.1 | Copyright (c) 2014 Werner Schweer and Others, Reserved Font Name "MScore Text" | [fonts/mscore/OFL.txt](fonts/mscore/OFL.txt) |
| MScoreBC (`fonts/mscore-BC.ttf`), MScoreTabulature (`fonts/mscoreTab.ttf`) | SIL Open Font License 1.1 | © 2012-15 Maurizio M. Gavioli | [fonts/OFL.txt](fonts/OFL.txt) |
| MusescoreIcon (`src/framework/ui/data/MusescoreIcon.ttf`), the interface icon font | The font clause of [LICENSE.txt](LICENSE.txt): all fonts of MuseScore Studio are made available under the GNU Freefont License (GPL) with a font exception. The font's own name table has no licence text. | MuseScore contributors (font name "Musescore Icon Set", version 1.0) | Font clause in [LICENSE.txt](LICENSE.txt). To be confirmed by the owner. |

## Downloaded by the audio engine (not bundled)

"Drum score from a recording" needs an audio engine of about 1 GB. KitNote does not ship it: `share/ai/install_engine.ps1` downloads it into `%LOCALAPPDATA%\KitNote\ai`, and only after the user has agreed in a dialog that names what is downloaded and from where. Each component stays under its own licence:

| Component | Version | Licence | Downloaded from |
| --- | --- | --- | --- |
| [uv](https://github.com/astral-sh/uv) (Python installer) | latest release | MIT OR Apache-2.0 | github.com/astral-sh/uv releases |
| Python (CPython standalone build, installed by uv) | 3.11 | PSF License Agreement. The standalone build also contains other libraries (OpenSSL, SQLite, Tcl/Tk, libffi ...) under their own licences; check the `LICENSE.txt` in the installed Python folder | python-build-standalone, by uv |
| [PyTorch](https://github.com/pytorch/pytorch) `torch`, `torchaudio` (CPU builds) | 2.5.1 | BSD-3-Clause (torch), BSD-2-Clause (torchaudio) | download.pytorch.org |
| [Demucs](https://github.com/facebookresearch/demucs) | 4.0.1 | MIT (Meta Platforms) | PyPI |
| Demucs model weights `htdemucs` (and `htdemucs_ft` when "more accurate separation" is chosen) | - | MIT, as stated for the Demucs code and pretrained models by the Demucs repository; check the repository's README if in doubt | dl.fbaipublicfiles.com (Meta) |
| [Beat This!](https://github.com/CPJKU/beat_this) `beat-this` | 1.1.0 | MIT (Institute of Computational Perception, JKU Linz) | PyPI |
| Beat This! checkpoint `final0` | - | MIT, per the Beat This! repository; check the repository if in doubt | cloud.cp.jku.at (JKU Linz) |
| Python packages pulled in by the above: numpy, scipy, soundfile, einops, rotary-embedding-torch, julius, openunmix, dora-search, PyYAML, tqdm, and their dependencies | as installed | mostly BSD / MIT / Apache-2.0. Exceptions to note: `soundfile` bundles libsndfile (LGPL-2.1), `soxr` is LGPL-2.1-or-later, `lameenc` is LGPL-3.0-or-later, `tqdm` is MPL-2.0 AND MIT | PyPI |

None of these is linked into KitNote: KitNote starts the engine's Python as a separate program. `ffmpeg`, which the engine uses for `.m4a`/`.aac` files when it is installed, is neither bundled nor downloaded by KitNote.

## Not used

KitNote does not ship or download ADTOF or any other model or dataset under a non-commercial (NC) licence.
