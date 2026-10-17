---
name: kitnote
description: Write, edit, read and export drum scores (drum kit sheet music) with KitNote, and turn recordings into drum scores. Use when the user wants drum notation, a drum chart, a groove, a fill, a drum part of a song, a drum transcription of an audio file, or a .kitnote file made or changed. Load it before the first mcp__kitnote tool call: it has the tab rules, the exact arguments and what the tools cannot do.
---

# KitNote: drum scores with AI

KitNote is a drum notation program (a drum-only MuseScore). Its MCP server `kitnote` (tools below) does the
work: you describe the music as a **drum tab**, KitNote engraves it as real notation and plays it back.
Everything it writes can be downloaded by the user: the `.kitnote` score, and PDF, PNG, SVG, MIDI,
MusicXML, MP3, WAV, OGG, FLAC or a text drum tab from `export_score`.

## Tools

Use these argument names exactly (a wrong name such as `text`, `drum_tab` or `command` fails):

| Tool | Arguments | What it does |
| --- | --- | --- |
| `create_score` | `path`, `title`, `composer`, `tempo`, `time_signature`, `measures`, `kit` (standard, 4-piece, large, marching), `overwrite` | New score from a template |
| `write_drum_tab` | `path`, `tab` (the tab text), `start_measure` | Writes the tab from `start_measure` on (adds measures at the end if needed) |
| `fill_pattern` | `path`, `pattern`, `first_measure`, `last_measure` | Ready-made groove / fill / rudiment (`list_patterns` names them) |
| `clear_measures` | `path`, `first_measure`, `last_measure` | Empties measures |
| `add_section` | `path`, `measure`, `name` | Section name (Intro, Verse, Chorus ...) at a measure |
| `set_properties` | `path`, `title`, `composer`, `tempo`, `time_signature`, `measures`, `swing` ("off", "8", "16"), `recording`, `recording_offset`, `recording_timing` | Changes them for the whole score (`tempo`: the one at measure 1). `recording`: a wav/flac/mp3/ogg KitNote plays along with the score in its Mixer, to check it by ear - where measure 1 begins in it is found from the notes (`""` removes it); `recording_offset`: that place in seconds; `recording_timing: false`: drop a transcription's hidden tempo changes |
| `edit_score` | `path`, `commands` (a list of strings) | KitNote commands, see below |
| `view_score` | `path`, `page` | A page of the score as a picture: look at it before saying the score is done |
| `read_score` | `path` | Summary JSON: title, tempo, time signature, measures, sections, the score as drum tab |
| `export_score` | `path`, `output_path` | PDF / PNG / SVG / MIDI / MusicXML / MP3 / WAV / OGG / FLAC / TXT by the extension of `output_path` |
| `transcribe_audio` | `audio_path`, `output_path`, ... | Recording (wav, mp3, flac, ogg) → drum score: separates the drums (Demucs), finds and sorts the strokes, writes them at the song's tempo; `stems_folder` also keeps drums/bass/vocals/other as wav files; the score plays the recording along with it (read_score: `recording`) |
| `audio_engine` | `action`, `confirm` | Whether the audio engine for `transcribe_audio` is installed; installs or removes it (`confirm: true`, only after the user agreed) |
| `open_in_kitnote` | `path` | Opens the score in the KitNote window for the user |

`edit_score` commands are strings; a range of measures comes first: `["@5-8 change:hihat-to-ride", "@1-4 sticking-r", "counting"]`
(no range = the whole score). `sticking-r` writes alternating R L starting with the right hand, `sticking-l` starts
with the left - use one, never both (the second replaces the first). Others: `counting` (1 e & a), `repeats`
(a measure equal to the one before becomes a % sign; a phrase of 2 or 4 measures played again right after itself
becomes the 2- or 4-measure repeat sign, never across a section, repeat barline, ending, time signature or tempo
change; `write-out` turns all of them back into notes), `key` (a drum key / legend before measure 1),
`change:accent-beats`, `change:ride-to-hihat`, `pattern:<name>`, `section:<name>`, `clear`.
- A tempo change partway through: `@9 tempo:140` (one measure; the new tempo holds from there on, measure 1 keeps
  its own). `read_score` lists them as `tempo_marks`.
- Repeat barlines (도돌이표): `@5-8 repeat` puts |: at 5 and :| at 8 (`repeat:3` plays them three times); endings
  after it: `@8 volta:1`, `@9-10 volta:2`; `@5-10 repeat-remove` takes both away. `read_score` lists `repeats`
  and `voltas`; write the repeated measures once.
- Measure-repeat signs are not tab syntax: `read_score` writes them out in `drum_tab` as the measures they stand
  for, and lists them as `repeat_signs` (`first_measure`, `measure_count` 1, 2 or 4). Writing a tab over part of a
  2- or 4-measure sign writes the rest of it out as notes.
- `title:`, `composer:`, `measures:` and swing are for the whole score: with a range they are an error.

What the tools cannot do - say so plainly and offer `open_in_kitnote` to add it by hand in KitNote:
D.S., D.C., Segno, Coda and Fine (`repeat` and `volta` cover |: :| and endings), a gradual tempo change
(accel., rit.), dynamics and other text in the music.

Paths may be absolute or relative to the working folder (in a chat app such as Claude Desktop, relative paths
go to the KitNote folder in Documents: Documents\KitNote on Windows, Documents/KitNote on a Mac - the server's instructions say which). Work in the user's folder unless told
otherwise, and tell the user the full path of every file you wrote.

## The drum tab

One line per drum, `|` between measures, one character per step:

```
CC|x---------------|----------------|
HH|--x-x-x-x-x-x-x-|x-x-x-x-x-x-x-o-|
SD|----o-------o---|----o--g----o---|
BD|o-------o-o-----|o-----o-o-------|
```

- Labels: `CC` crash, `RD` ride, `HH` hi-hat (played with a stick), `T1` high tom, `T2` mid tom, `SD` snare,
  `FT` floor tom, `BD` bass drum, `HF` hi-hat pedal (the foot: the jazz "chick" on 2 and 4 is `HF`, not `HH`).
- Signs: `x` / `o` a stroke (on `HH`, `o` = open hi-hat; on `SD`, `x` = cross-stick), capital `X` / `O` an
  accent, `g` a ghost note, `-` nothing.
- Every line of a row covers the same measures: write a silent measure as `----------------`, never
  leave it out. A row whose lines cover different numbers of measures is refused with an error naming
  the line, and nothing is written.
- Accents are rare: write the crash and the backbeat in lowercase unless the user wants accents. An
  accent belongs to the step: the hand strokes of a step are one chord, so `O` on the snare also makes
  the hi-hat or ride on that step `X` in `read_score` - expected, not a fault to report. `read_score`
  also writes a plain stroke on a tom or the bass drum as `o`, whatever you wrote.
- `tempo` is always quarter notes a minute; in 6/8 and 12/8 KitNote writes it as dotted quarters
  ("dotted quarter = 60" is `tempo: 90`); `read_score` then also gives `tempo_dotted_quarter` - tell the user that one.
- After a change, the summary notes empty measures left at the end: keep the length the user asked for
  (`set_properties` `measures`), and say the real number of measures.
- Steps per measure: 16 = sixteenth notes in 4/4 (4 per beat), 8 = eighth notes, 12 = triplet eighths in 4/4
  or sixteenths in 3/4, 12 in 6/8 = sixteenths. Every line of a measure has the same number of steps.
- Hands go to the upper voice and the feet (`BD`, `HF`) to the lower one by themselves.
- Only the labels above are drum lines; another label is an error. A measure whose lines do not fit it
  (a line of 14 columns in 4/4) is refused with an error - count the columns.
- Set the time signature before writing: a new one re-bars the music already in the score.
- Rows of four measures are usual; separate rows with an empty line - and never put an empty line inside
  a row. A row begins at the top of the kit, and has the drums of its measures, not only cymbals: a row
  split by an empty line, or one of CC/RD/HH lines alone among rows with drums, is refused with an error
  (a tab of the hi-hat alone is fine). Lines of other text are ignored.
- Measure numbers start at 1. Use absolute paths for files.

### A worked example

Eight measures at 100 BPM: a verse groove on the hi-hat with a fill in measure 4, then a chorus on the
ride that opens with a crash and ends with a tom fill. `create_score` with `measures: 8`, `add_section`
at 1 ("Verse") and 5 ("Chorus"), then one `write_drum_tab` from measure 1:

```
HH|x-x-x-x-x-x-x-x-|x-x-x-x-x-x-x-x-|x-x-x-x-x-x-x-x-|x-x-x-x---------|
SD|----o-------o---|----o-------o---|----o--g----o---|----o---oooooooo|
BD|o-------o-o-----|o-------o-o-----|o-------o-o-----|o-------o-------|

CC|x---------------|----------------|----------------|----------------|
RD|--x-x-x-x-x-x-x-|x-x-x-x-x-x-x-x-|x-x-x-x-x-x-x-x-|x-x-x-x---------|
SD|----o-------o---|----o-------o---|----o-------o---|----o---oo------|
T1|----------------|----------------|----------------|----------oo----|
FT|----------------|----------------|----------------|------------oooo|
BD|o-------o-o-----|o-------o-o-----|o-------o-o-----|o-------o-------|
```

## Ready requests

The server also offers prompts (in Claude Code: `/mcp__kitnote__drum_chart`, `groove_exercise`, `transcribe_song`, `change_score`).

## How to work

1. `create_score` with the title, tempo, time signature and enough measures (a song chart: count the form).
2. Write the music with `write_drum_tab` - a section or a few rows at a time, `start_measure` where it goes.
   For plain grooves `fill_pattern` is quicker (`@` ranges with `edit_score` for many at once).
3. Mark the form with `add_section`; add the `edit_score` touches asked for (sticking, counting, key, repeat
   barlines, tempo changes ...).
4. Check with `read_score` (the tab comes back as written: compare it with what you meant, measure by
   measure - fills, crashes, section starts), and look at the result with `view_score`.
5. Give the user files: `export_score` to PDF (to print), MIDI or MusicXML (other programs), MP3 (to listen);
   `open_in_kitnote` when they want to see and play it.
6. Tell the user what is in the score as `read_score` returned it, not as you planned it: the real number
   of measures, where the fills and crashes really are, and anything asked for that you could not do.

Music sense for drum parts: a groove repeats with small changes; a fill (toms going down, snare runs) ends
a section in its last measure or two, and a crash (`CC` on step 1) marks the start of the next section;
a drummer has two hands - at most two of HH, RD, CC, SD, T1, T2, FT on one step; where a fill plays the
hi-hat or ride line is empty; choruses often open the hi-hat or move to the ride; keep the bass drum and
snare clear (snare on 2 and 4 in most pop and rock).
When the request is vague ("신나는 드럼 악보"), choose for yourself (e.g. rock or pop, 110-130 BPM,
8-16 measures), make it, and say what you chose. Leave `composer` empty unless the user names one.

## Transcribing a recording

`transcribe_audio` needs KitNote's audio engine. Check with `audio_engine` (action `status`). Engine 2 (a model for
beats and measures, Beat This!, and KitNote's own drum model) transcribes far better than engine 1; if
`update_available` is true, offer the update (about 100 MB, the same `install` call with `confirm: true` after
the user agrees). If it is not
installed, ask the user in the chat before installing: say that it downloads uv, Python 3.11, PyTorch (CPU)
and the Demucs separation model by Meta - about 1 GB, some minutes - into KitNote's own folder only, and can
be removed again. Only when they agree, call `audio_engine` with `action: "install"` and `confirm: true`,
then transcribe. (KitNote → Preferences → AI installs and removes it as well.) The result is a starting point: read it back with `read_score`,
fix what is clearly wrong (a missing backbeat, a fill), and say that the transcription is automatic.
Leave `time_signature` out unless the user knows it: it is found from the groove (4/4, 3/4, 6/8 or 12/8; a
shuffle stays 4/4 with triplets). The result's `time_signature` tells what was found. The tempo is counted as
a chart counts it, the snare's backbeat on 2 and 4: a slow groove with sixteenth-note hi-hats stays slow
(♩=70, not 140), fast punk in eighths stays fast; a song in half time all through is written at the slower
tempo. If the user knows the tempo, pass `tempo`. Toms are named by their pitch, from the highest: three toms
are `T1` `T2` `FT`, two (a four-piece kit) `T1` and `FT`, a fourth one is a second floor tom. Weak snare strokes are
left out as a chart leaves them out; `ghost_notes: true` writes them as ghost notes (only if the user asks).
The score follows the recording's own timing: `read_score` says `tempo_follows_recording: true` and
`hidden_tempo_changes` (hidden tempo marks that keep every measure with the recording; the chart shows one rounded
tempo, and `tempo_marks` only where the song clearly changes tempo). Leave them alone: setting `tempo` scales
them all (the recording then no longer lines up); `set_properties` `recording_timing: false` removes them.
