# Changelog

All notable changes to KitNote. The release workflow copies the section for the version being released
(the `## <version>` heading) into the GitHub Release notes, so every release needs one. See
[docs/RELEASING.md](docs/RELEASING.md).

## 1.0.0

The first release of KitNote, a drum notation editor built on MuseScore Studio 4.7.5. The engraving, playback
and editing are MuseScore's; everything around them is made for the drum kit.

Platforms: Windows (x64; an installer and a portable zip) and macOS 14.4 or later (Apple silicon and Intel).

### Writing
- Step grid: write a measure like on a drum machine, sixteenth by sixteenth.
- Pattern library: grooves, fills and rudiments shown as mini grids, inserted with one click; save your own
  measures as patterns, or ask for a random fill.
- Drum patterns (Add > Drum patterns): 20 grooves and fills, among them rock, Motown, shuffle, jazz ride, disco,
  bossa nova, reggae and 6/8, written into the selected measures.
- Rudiments (Add > Rudiments): nine snare rudiments with sticking, flams and accents.
- Drum tab in and out: copy a score as text tab (`HH|x-x-x-x-|`), or paste a text drum tab and get notation.
- Selection tools: hi-hat to ride, swap sticking, accents on or off the beat, sticking (R L R L) and counting
  (1 e & a) under the notes.

### Reading and playing
- Published-chart engraving by default: four measures to a line, boxed section letters, repeat signs for one,
  two and four measures with counts, merged rests for hands and feet.
- Drum key: a legend of the drums the score uses, before the first measure.
- Drum-aware playback: soft ghost notes, light flam grace notes, an open sign over the hi-hat plays the open
  hi-hat, one-click swing.
- Practice: loop a passage with a rising tempo, tap tempo, and song structure (Intro, Verse, Chorus ...) to
  jump between sections.
- Play along with a recording: a score made from a recording keeps it in the Mixer, and playback follows the
  recording's timing measure by measure while the chart shows one clean tempo.

### Drum score from a recording
- The drums are separated from the song, and the beat, the measures and the strokes are found by KitNote's own
  drum model, trained only on openly licensed data.
- Open or closed hi-hat, crash or ride, toms, ghost notes, time signature (4/4, 3/4, 6/8, 12/8) and sections are
  decided for you; repeated measures become repeat signs.
- The result is a starting point to correct. The audio engine (about 1 GB) is downloaded only after you agree.

### AI tools
- Preferences > AI sets up KitNote's local MCP server (`kitnote-mcp.exe`) and its skill for Claude Code, Codex,
  Claude Desktop, Cursor and Gemini CLI, and takes them out again.
- AI tools can write drum charts, change a score, make a drum score from an audio file and export the result.

### Files and export
- Scores are saved as `.kitnote`, which is a MuseScore archive: rename a copy to `.mscz` to open it in MuseScore
  Studio. `.mscz` and `.mscx` files open and save as well; MusicXML and MIDI files can be imported and exported.
- Export: PDF, PNG, SVG, MIDI, MusicXML, MP3, WAV, OGG, FLAC, AAC and text tab.

### Limits
- Scores with pitched instruments are not opened; the message names the first such part.
- The SoundFont is a drum-only cut of MS Basic: pitched presets are not included.
- Interface in English and Korean.
- The automatic update check of MuseScore Studio is off.

### 한국어 요약
- 플랫폼: Windows(x64; 설치 프로그램과 압축 파일), macOS 14.4 이상(Apple 실리콘과 인텔).
- 악보 쓰기: 스텝 그리드, 패턴 라이브러리(20가지 그루브와 필인, 루디먼트 9가지), 드럼 탭 붙여넣기, 선택 도구.
- 읽기와 재생: 드럼 악보 조판(한 줄 네 마디, 구간 문자, 반복 기호), 드럼 키, 고스트 노트와 꾸밈음 재생,
  연습 모드(템포 올리기, 구간 반복, 곡 구조 이동).
- 녹음에서 드럼 악보 만들기: 드럼 분리 후 KitNote의 드럼 모델이 스트로크를 인식합니다. 오디오 엔진은 동의한 뒤에만 내려받습니다.
- AI 도구: Claude Code, Codex 등에 로컬 MCP 서버와 스킬을 한 번에 설정합니다.
- 저장과 내보내기: `.kitnote` 파일, PDF·PNG·SVG·MIDI·MusicXML·MP3·WAV 등.
- 제한: 타악기가 아닌 악기가 들어 있는 악보는 열지 않습니다.
