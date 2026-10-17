KitNote installs share/sound/MS Basic.sf3 (MIT, see MS Basic_License.md) as its SoundFont. It is in the repository, so the build does not download it.

KitNote Drums.sf3 is the percussion-only part of MS Basic.sf3, made by tools/kitnote/make_drum_soundfont.py. When it exists, the build installs it in place of the full file.

If MS Basic.sf3 is missing, the build installs FluidR3Mono_GM.sf3 under the MS Basic name instead. FluidR3Mono_GM.sf3 is an exact copy of the FluidR3Mono SoundFont as provided in MuseScore 2.1 (see FluidR3Mono_License.md).
