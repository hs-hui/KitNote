# Using KitNote

KitNote is an editor for drum and percussion scores only. Editing works as in MuseScore Studio 4, so anything you know from MuseScore carries over.

## Installing

`bin\KitNote.exe` in the unpacked folder runs as it is. To have KitNote in the Start menu and open `.kitnote` files with a double-click, right-click `install.ps1` in the unpacked folder and choose **Run with PowerShell**. No administrator rights are needed. Remove it from **Installed apps** in Windows Settings.

## Creating a score

The buttons in the **Start with** row at the bottom of the Home page (drum kit, four-piece, large, battery percussion) open a 4/4 score of 32 measures at once. To choose the time signature or length first, do this:

1. Press `Ctrl+N`, or **New score** on the Home page.
2. The **Choose instruments** tab already holds a drum kit. To use other percussion, pick it from the list on the left and add it. To start from a prepared setup, use the **Create from template** tab.
3. **Next** lets you set the time signature, tempo and number of measures; **Done** opens the score.

## The screen

A new score has four measures to a line. While it has no notes, `An empty score. Begin with:` at the top offers `Note input` and `Step grid` (at the first measure) and `A groove in every measure` (every measure selected and the pattern library open).


A score opens without panels: just the score. Instead of a row of toolbars at the top, a rail floats at the left edge of the score with the things you need all the time: back to Home, note input, the note-input toolbar, the drum pads (percussion panel), the palettes, rewind, play, loop and metronome, the mixer, undo and redo, the step grid, the pattern library, practice with a rising tempo, the song structure, the position and tempo, and the command search. A click on the position and tempo steps the playback speed through 100, 75 and 50% for practising.

Everything else is reached with `Ctrl+K` (or the magnifying glass at the foot of the rail): type part of a name and press `Enter`. The drum pads and the note-input toolbar open by themselves with note input and close with it (`Preferences → Percussion` changes when the pads open). `View → Restore the default layout` brings this layout back after you changed it.

`Edit → Preferences → Appearance → Toolbar position` moves the rail: `Top`, `Bottom`, `Left` or `Right`, and it moves at once. At the top and the bottom the rail lies flat.

## Entering notes

1. Click the measure (rest) where input should start. When a score opens, its first measure is already selected and the percussion panel at the bottom shows the pads.
2. Press `N` to enter note-input mode.
3. Choose a duration with the number keys.

   | Key | Duration |
   | --- | --- |
   | `1` | 64th note |
   | `2` | 32nd note |
   | `3` | 16th note |
   | `4` | 8th note |
   | `5` | Quarter note |
   | `6` | Half note |
   | `7` | Whole note |
   | `.` | Dotted |
   | `0` | Rest |

4. Click a pad, or press the letter shown on it. The note is entered and the cursor moves on. The drum kit's default keys are:

   | Key | Drum |
   | --- | --- |
   | `A` | Bass drum |
   | `B` | Snare |
   | `C` | Closed hi-hat |
   | `D` | Open hi-hat |
   | `E` | Crash cymbal |
   | `F` | Ride cymbal |
   | `G` | High tom |
   | `H` | Floor tom |

5. To add a drum on the same beat, hold `Shift` and press its letter.
6. Leave note-input mode with `Esc` or `N`.

Drums played with the hands go to voice 1 (stems up) and drums played with the feet to voice 2 (stems down) by themselves.

## The step grid

A measure can be written the way a drum machine is programmed. Select a measure (or a note in it) and press the grid button on the rail: the measure appears as a grid, a row for every drum and a step for every sixteenth note. A click puts a drum on a step or takes it off, and the score follows at once. `Ctrl+Z` undoes it.

- Drums played with the hands go to voice 1, the bass drum and pedal hi-hat to voice 2.
- A right-click steps a stroke through accented, ghost note and plain. An accent shows as `>`, a ghost note as an outlined step.
- A drum is heard when it is put on a step. The arrows at the top right of the card move to the next and previous measure.
- While the measure plays, the step that sounds lights up.
- `Copy to next measure` writes the measure into the next one and moves there. `Clear` empties the measure.
- A measure edited in the grid loses its sticking, grace notes and dynamics: add those when the grid work is done.
- A measure that is a repeat sign (%) has to be written out before it can be edited.
- A measure in triplet eighths (a shuffle, say) appears as a grid with three steps to a beat. In an empty measure the `Triplets` button chooses that grid.
- A measure with notes shorter than its steps, such as 32nd notes, is not shown.

## Song structure

The list button on the rail (or `song structure` in the command search) shows the sections of the chart: name, first measure, length. A click on a section goes to its first measure. To start a section, select the measure and press one of the names under the list (Intro, Verse, Pre-Chorus, Chorus, Bridge, Solo, Interlude, Outro): the name is put over that measure, and the section runs up to the measure before the next one. A section name can be edited with a double-click, or selected and deleted.

Once there are sections the card also lists grooves. Select a measure in a section and press a groove: the whole section is filled with it, and with `End the section with a fill` on, the last two beats of its last measure become a fill. Divide the song into sections, pick a groove for each, and the skeleton of the chart is there.

The step grid, practice and song structure cards show one at a time and can be dragged by their background.

## Practising with a rising tempo

The clock button on the rail (or `practice` in the command search) opens the practice card. Select the measures to loop and press `Start`: they repeat, slowly at first (60% by default), a little faster with every pass (5%), up to the target (100%). Without a selection the whole score is looped. `Stop` ends playback and puts the tempo back.

At the foot of the same card, tap `Tap the tempo` on the beat a few times: the speed shows in BPM, and `Use as the tempo of the score` rewrites the tempo mark at the beginning of the score (or puts one there).

## Pattern library

The `%` button on the rail (or `Pattern library` in the command search) opens a card with the grooves, the fills and the rudiments. Every pattern shows one measure as a small step grid - a row for every drum, the sticking under a rudiment - so you see what will be written. Select measures and click a pattern to write it there. It is the same command as `Add → Drum patterns` below.

At the top of the `Fills` tab, `Random fill: a measure` and `Random fill: half a measure` make up a fill on the spot and write it into the selected measure - sixteenth notes from the snare down the toms, the half measure after two beats of the rock beat. Click again for another one.

The `My patterns` tab collects measures of your own. Select a measure and click `Keep the selected measure` (a name is optional): a tile appears, and from then on it can be written into any score like the other patterns. The `×` of a tile throws it away. Strokes, accents, ghost notes and open hi-hats are kept; marks such as flams and sticking are not.

## Inserting drum patterns

Common rhythms do not have to be entered note by note.

1. Select the measures to fill. For several measures, click the first one, then hold `Shift` and click the last one.
2. Choose a pattern from `Add → Drum patterns`. Right-clicking a measure or a note shows the same menu.

   | Pattern | What it writes |
   | --- | --- |
   | Rock beat (8th-note hi-hat) | Hi-hat in 8th notes, snare on 2 and 4, bass drum on 1 and 3 |
   | Rock beat with pushed kick | The same, with the bass drum on 1, the "and" of 2, and 3 |
   | Rock beat (16th-note hi-hat) | Hi-hat in 16th notes |
   | Rock beat (quarter-note hi-hat) | Hi-hat in quarter notes |
   | Four on the floor | Bass drum on every beat |
   | Half-time beat | Snare on 3 only |
   | Rock beat on the ride | Ride cymbal instead of the hi-hat |
   | Rock beat starting with a crash | A crash on the first beat of the first measure only |
   | Motown beat (snare on every beat) | Hi-hat in 8th notes, snare on every beat |
   | Shuffle beat | Hi-hat on the first and third note of each triplet, snare on 2 and 4 |
   | Slow blues beat (triplet hi-hat) | Hi-hat in triplet eighth notes, snare on 2 and 4 |
   | Jazz ride (swing) | The swing pattern on the ride cymbal, pedal hi-hat on 2 and 4 |
   | Disco beat (open hi-hat off the beat) | Bass drum on every beat, open hi-hat between the beats |
   | Bossa nova | Hi-hat in 8th notes with the bossa nova rhythm on cross-stick and bass drum |
   | Reggae one drop | Hi-hat in 8th notes, bass drum and cross-stick together on 3 only |
   | 6/8 beat | For 6/8 time: hi-hat in 8th notes, bass drum on 1, snare on 4 |
   | Fill: 16th notes on the snare | One measure of 16th notes on the snare |
   | Fill: 16th notes down the toms | Snare → high tom → mid tom → floor tom |
   | Fill: triplets down the toms | The same order in triplet eighth notes |
   | Fill: beat, then a fill on 3 and 4 | The rock beat on 1 and 2, sixteenth notes from the snare down the toms on 3 and 4 |

   In 6/8, 9/8 and 12/8 measures the patterns follow the dotted-quarter beat (bass drum on the odd beats, snare on the even ones).

3. The measures stay selected, so pressing `R` repeats them into the following measures.

### Rudiments

`Add → Rudiments` has nine snare rudiments (single stroke roll, double stroke roll, single paradiddle, single stroke triplets, double paradiddle, paradiddle-diddle, flams, flam tap, flam accent). They are inserted like the drum patterns and come with their R and L sticking; the flams have their grace notes, the paradiddles and the flam accent their accents. The sticking runs on over the barlines.

Whatever was in the selected measures is replaced by the pattern; `Ctrl+Z` brings it back. Patterns are not available in note-input mode: press `Esc` first.

## Drum key

`Add → Drum key at the start of the score` puts a line in front of the first measure that shows each drum the score uses, with its name under the note. In a score without notes yet, it shows the drums that have a pad shortcut.

- The key's measures are not counted in the measure numbers and make no sound on playback.
- The grey `♩ = 600` and time signature over the key in the editor are hidden marks that let playback pass the key at once. They do not appear in print or in exported files.
- Run the command again after adding notes and the key is renewed for the drums the score uses now. To remove it, choose `Add → Remove the drum key`.
- When there are few drums, an empty frame after the key keeps it from stretching over the whole line.
- The names are lyrics, so a double-click lets you change them.

## Measure-repeat signs (%)

Measures that carry on with the same rhythm can be written as repeat signs.

- `Tools → Replace repeated measures with repeat signs`: every measure that has the same content as the one before it becomes `%`. With a range of measures selected only that range is done, otherwise the whole score. The first measure after a repeat barline or a double barline stays written out, and so does a measure with a dynamic, a tempo or text (the equal measures after it are replaced). Sticking counts as content: two measures are the same when notes and sticking both are.
- `Tools → Write out measure-repeat signs`: turns `%` back into notes, for when one of the repeated measures has to be changed.

On playback a `%` measure sounds exactly like the measure before it.

## The percussion panel

- **Write / Preview**: in Preview a pad only plays its sound and enters nothing.
- **Layout → Edit layout**: drag pads to other places, add or remove rows.
- **Layout → Pad names / Notation preview**: show the note as it appears on the staff instead of the drum's name.
- Right-click a pad for **Define keyboard shortcut** and **Delete pad**.
- **Customize kit**: change each drum's name, notehead, staff line, voice and shortcut. MuseScore drumset files (`.drm`) can be loaded and saved.

## Frequently used signs

Pick them from the **Palettes** on the left, or select a note and click the palette item. The **Drum techniques** palette at the top gathers accent, open and closed hi-hat signs, ghost note, flam, drag, rolls and sticking.

| Sign | Where |
| --- | --- |
| Accent | The `>` button in the toolbar, `Shift+V`, or the Articulations palette |
| Ghost note (parentheses) | The `( )` button in the toolbar, or `Shift+9`. It plays softer than the other notes. |
| Open hi-hat (`o`) / closed (`+`) | The Drum techniques palette. A closed hi-hat note with `o` on it plays the open hi-hat sound. |
| Flam | The grace-note button in the toolbar, or `/` |
| Drag, ruff | The Grace notes palette |
| Roll (tremolo) | The Tremolos palette |
| Dynamics | The Dynamics palette |
| Sticking (R/L) | `R` and `L` in the Drum techniques palette, or type it with `Ctrl+Shift+K` (`Space` moves to the next note) |
| Automatic sticking | Select the notes, then `Tools → Sticking: R L R L` (or `L R L R` to start with the left hand). Notes for the feet are passed over. |
| Counting | Select the notes, then `Tools → Counting: 1 e & a`. The beat gets its number, the half beat `&`, sixteenth notes `e` and `a`, triplets `&` and `a`. In 6/8, 9/8 and 12/8 every eighth note gets a number (1 to 6 in 6/8) and the sixteenth notes between them `&`. A pickup measure is counted towards its barline. The count is lyrics, so a double-click lets you change it. |
| Section names (Intro, Verse, Chorus…) and instructions such as Fill and Break | The Chart marks palette. Double-click one in the score to change its text. |
| Repeats, voltas, D.S., Coda | The Repeats & jumps palette |
| Tuplets | The tuplet button in the toolbar (`Ctrl+3` for a triplet) |

## Playback

`Space` starts and stops playback. Rewind, loop and metronome are on the rail; the rest, such as count-in, is in the command search (`Ctrl+K`). A click on the position and tempo of the rail steps the playback speed through 100, 75 and 50%.

For a swing feel, choose `Swung eighth notes` or `Swung sixteenth notes` in `Tools → Swing`. The notation stays as it is; only playback and exported audio change, and `Straight (no swing)` turns it off again.

## Saving and exporting

- `Ctrl+S` saves a `.kitnote` file. The save dialog can also write a **MuseScore file (\*.mscz)**.
- A `.kitnote` file has the same content as an `.mscz` file. Rename it to `.mscz` and MuseScore Studio opens it.
- When exporting audio, **Include the metronome click** makes a practice track with the click in it.
- `File → Export` writes PDF, PNG, SVG, MP3, WAV, OGG, FLAC, AAC, MIDI and MusicXML.

### Changing the selection

`Tools → Change the selection` gathers what an arranger does all the time. Select measures or notes, then choose:

- `Hi-hat to ride` / `Ride to hi-hat`: moves the cymbal part of a groove (to the ride for a chorus, say).
- `Swap the sticking (R and L)`: exchanges R and L in the sticking.
- `Accents on the beats` / `Accents off the beats`: accents the hand strokes that fall on a beat, or halfway between two (for accent studies).
- `Remove the accents`: takes the accents of the selection away.

### Copying as a drum tab (text)

`Tools → Copy as drum tab (text)` puts the selected measures (the whole score when nothing is selected) on the clipboard as a text drum tab, to paste into a message or a text file (it lines up in a font with characters of equal width).

```
HH|x-x-x-x-x-x-x-x-|
SD|----o-------o---|
BD|o-------o-------|
```

`x` is a cymbal, `o` a drum (on the hi-hat line: the open hi-hat), a capital an accent, `g` a ghost note. To save it as a file, choose `Drum tab (text)` in `File → Export`. A measure with triplets gets 12 or 24 columns.

### Pasting a drum tab (text)

The other way round: copy a text drum tab from the web or from your notes, then choose `Tools → Paste drum tab (text)`. It is written into the score from the selected measure on (from the first measure when nothing is selected); measures are added when the score is shorter than the tab.

- The drum of a line is read from the letters it begins with: `HH`/`H` (hi-hat), `SD`/`S`/`SN` (snare), `BD`/`B`/`K` (bass drum), `CC`/`C`/`CR` (crash), `RD`/`R` (ride), `T1`/`HT`, `T2`/`MT`, `FT`/`LT`/`F` (toms), `HF` (pedal hi-hat).
- A capital is an accent and `g` a ghost note. On the hi-hat line an `o` is the open hi-hat, on the snare line an `x` the cross-stick.
- Other lines, such as counting or lyrics, are passed over.
- Lines with twelve columns to the measure (in 4/4) are read as triplets.
- A measure of another length than the measure of the score, or with notes between its steps, cannot be written; you are told how many were left out.

## Drum scores with AI

KitNote comes with an **MCP server** and a **skill** for AI tools such as Claude Code and Codex: the AI can create drum scores, write them as drum tabs, change, read and export them to PDF, MIDI, MusicXML or MP3.

1. Open `Edit → Preferences → AI`.
2. Click `Set up` next to your tool (Claude Code, Codex, Claude Desktop, Cursor, Gemini CLI). KitNote is added to the tool's settings (the settings file is copied to `.kitnote-backup` first), and the skill to the tools that have skills.
3. Restart the tool and ask, for example: "A drum score for a 32-measure rock song at 120 BPM in 4/4: 4 measures of intro, 8 of verse, the chorus on the ride, a fill at the end of every section - and a PDF."
4. The files (`.kitnote`, PDF ...) are written where you said; open them in KitNote to play and edit.

Scores that exist can be changed too: "In Documents/KitNote/song.kitnote put measures 9-16 on the ride and a tom fill in the last measure." And recordings: "Make a drum score from C:/Music/song.mp3." If the audio engine is not there yet, the AI first asks in the chat - saying what it downloads, about 1 GB - and installs it only when you agree.

In Claude Code, KitNote's ready requests are there as commands: `/mcp__kitnote__drum_chart`, `/mcp__kitnote__groove_exercise`, `/mcp__kitnote__transcribe_song`, `/mcp__kitnote__change_score`.

Other MCP tools take `kitnote-mcp.exe` (in the folder of KitNote.exe) as a stdio server. `Remove` takes KitNote out of a tool again.

## Drum score from a recording

`File → Drum score from a recording…` (or the `From a recording` card on Home) separates the drums of a song from the other instruments (Demucs), finds their strokes and sorts them - bass drum, snare, toms, closed and open hi-hat, crash - finds the beat and writes a score - on sixteenth notes, or on triplets in measures played that way (a shuffle). It is saved next to the recording as `.kitnote` and opened. A recording (wav, mp3, flac, ogg, m4a…) can also be dropped on the KitNote window, or on the recording box of the dialog.

- The first time, it needs the audio engine (about 1 GB: Python, PyTorch, the separation model). It is installed into `%LOCALAPPDATA%\KitNote\ai` only after you agree in a dialog that says what is downloaded from where (or, when you give a recording to an AI tool such as Claude Code, after you agree in the chat), and `Preferences → AI` removes it.
- `Separate more accurately` uses the better separation model (htdemucs_ft): about four times slower, and about 320 MB more to download the first time.
- `Mark the sections where the groove changes` (on by default) puts A, B, C ... where the groove changes and stays changed, the same letter when a groove comes back - rename them (Verse, Chorus) after listening. `Write measures that repeat the one before as % signs` makes the chart shorter to read.
- Choose the time signature of the song. The default, `Automatic`, finds 4/4, 3/4, 6/8 or 12/8 from the measures the beat model hears and from how the strokes divide the beat: in 6/8 and 12/8 every eighth note is played (a hi-hat or ride on all three of each beat), while a shuffle leaves out the middle one and stays 4/4, with triplets. Or choose 4/4, 3/4, 2/4, 6/8 or 12/8; in 6/8 and 12/8 a beat is a dotted quarter, and the tempo is written so (`♩. = 60`) - the tempo field of the dialog then shows `♩.` too.
- For a recording of drums alone, tick `The recording is of drums alone` to skip the separation. A known tempo (of the beat) makes the result better. The hum of the mains (50 or 60 Hz, a cable not grounded) is taken out before the strokes are looked for, so that it is not written as a floor tom.
- Engine 2 uses two models trained on real music: Beat This! (JKU Linz, MIT licence) finds the beats and measures, KitNote's own drum model (trained by KitNote on openly licensed data: real drummers' performances and grooves of many styles played on 51 sampled kits) the drum strokes; KitNote tells open or closed hi-hat, crash or ride, which tom and ghost notes from the separated drums. An engine installed before shows `Update the audio engine…` in `Preferences → AI` (about 150 MB).
- An automatic transcription is a starting point. Against a published chart of a real song, engine 2 had about 99% of the bass drum, 95% of the snare and 96% of the time kept on hi-hat and cymbals right, and 99% of the hi-hats open or closed as in the chart; tempo, measures and bar lines were the same. Hi-hat and cymbal (crash, ride) are decided per measure, so playing that changes from beat to beat is written more simply. Toms are often missed, above all in a song with other instruments. Correct it with the step grid.
- Weak snare strokes (ghost notes) are left out unless you tick `Also write ghost notes`: charts usually leave them out, and many are the noise of other drums.
- The score plays the song along with it, so that you can check it by ear (next section).

## Playing along with the recording

A score made from a recording plays that recording with it, in time: the Mixer (`View → Mixer`) has a `Recording` channel next to the drums. Its fader sets the balance; `S` (solo) plays the recording alone, `M` (mute) leaves the drums alone. Play, stop, a click in the score, loops and the count-in move the recording with the score.

- `File → Play along with a recording…` attaches a recording (wav, flac, mp3, ogg) to any score, changes it or takes it away. KitNote finds where measure 1 begins in it by matching the notes with the strokes it hears; check by ear, and move `Measure 1 begins at … in the recording` a few hundredths of a second if need be (`Play` in the same dialog). The score keeps the file (next to the score: by its name) and this time in its properties.
- The recording is heard at 100% speed only: it is not stretched. At 75% or 50% its channel says `Only at 100%` and is silent.
- Exported audio leaves the recording out; tick `Include the recording` in the export dialog to mix it in. At a speed other than 100% the recording is left out of the export as well, and the dialog says so.
- A recording that was not found (moved, or on a drive that is not plugged in) is looked for again when you play, open the Mixer or `File → Play along with a recording…`: put back, it plays without the score being opened again.
- A song is never played at one exact tempo, so a score made from a recording follows the recording's own timing: hidden tempo changes (`View → Show → Show invisible` shows them in grey) keep every measure within about a hundredth of a second of where it is in the recording, and measure 1 begins where the beat model found the first downbeat. The chart still shows one tempo mark, rounded (`♩ = 94`), and a mark of its own only where the song clearly changes its tempo for at least four measures (a slower bridge, a ritardando at the end).
- Changing the first tempo mark (its text, Properties, tap tempo) scales the hidden changes and the later marks with it, in one step of the undo history: the song's feel at the new speed - the recording, never stretched, then no longer lines up (undo, or set the tempo back). Taking the recording away asks whether the score keeps its timing (it still plays like the song) or plays at its tempo marks alone.
- A recording attached by hand keeps its own time: where the song drifts away from the tempo of the score, the two drift apart.

## Files KitNote opens

`.kitnote`, `.mscz`, `.mscx`, MusicXML and MIDI files. **A file that contains any instrument other than unpitched percussion is not opened.**

## Other shortcuts

| Action | Key |
| --- | --- |
| Undo / redo | `Ctrl+Z` / `Ctrl+Y` |
| Repeat the selection | `R` |
| Add one measure at the end | `Ctrl+B` |
| Insert one measure before the selection | `Ins` |
| Delete the selected measures | `Ctrl+Del` |
| Show or hide the percussion panel | `O` |
| Zoom in / out | `Ctrl++` / `Ctrl+-` |
| Command search | `Ctrl+K` |

Shortcuts can be changed in `Edit → Preferences → Shortcuts`.
