<p align="center"><img src="docs/kitnote/banner.png" alt="KitNote — 드럼 악보 전용 편집기" width="100%"></p>

<p align="center"><a href="README.md">English</a> · <b>한국어</b> · <a href="https://kitnote.hui.kr">웹사이트</a></p>

# KitNote

KitNote는 드러머를 위한 **드럼 악보 전용 편집기**입니다.
[MuseScore Studio](https://github.com/musescore/MuseScore) 4.7.5 소스 코드를 바탕으로 만들었습니다. 조판·재생·편집 방식은
MuseScore 그대로 두고, 그 밖의 것을 모두 드럼 키트에 맞췄습니다:
- 드럼 전용 템플릿·팔레트·패드
- 스텝 그리드와 패턴 라이브러리
- 드럼에 맞춘 재생
- 녹음 파일에서 드럼 악보 만들기
- Claude Code·Codex 같은 AI 도구가 드럼 악보를 쓰고 채보할 수 있게 해 주는 로컬 MCP 서버

![패턴 라이브러리를 연 KitNote](docs/kitnote/screenshot-patterns.png)

## 기능

**악보 쓰기**
- **스텝 그리드**: 드럼머신처럼 16분음표 칸을 찍어 마디를 씁니다.
- **패턴 라이브러리**: 그루브·필인·루디먼트를 미니 그리드로 미리 보고 클릭해서 넣습니다. 직접 쓴 마디를 패턴으로 저장할 수 있고, 누를 때마다 다른 필인을 넣는 랜덤 필인도 있습니다.
- **드럼 패턴** (`추가 → 드럼 패턴`): 록·모타운·셔플·재즈 라이드·디스코·보사노바·레게·6/8 등 그루브와 필인 20가지를 선택한 마디에 바로 채웁니다.
- **루디먼트** (`추가 → 루디먼트`): 스티킹·플램·악센트가 붙은 스네어 루디먼트 9가지를 넣습니다.
- **드럼 탭 주고받기**: 악보를 `HH|x-x-x-x-|` 같은 텍스트 탭으로 복사하고, 인터넷의 텍스트 드럼 탭을 붙여 넣으면 악보가 됩니다.
- **선택 영역 도구**: 하이햇↔라이드 바꾸기, 스티킹 좌우 바꾸기, 정박·엇박 악센트, 음표 아래 스티킹(R L R L)과 카운트(1 e & a) 표기

**보고 연주하기**
- **출판 악보 같은 기본 조판**
  - 한 줄에 4마디, 네모 칸 섹션 글자
  - 1·2·4마디 반복 기호(%)와 마디 수 표시
  - 손과 발 성부의 쉼표를 하나로 합쳐 표시
- **드럼 키(범례)**: 악보에 쓰인 드럼과 이름을 첫 마디 앞에 안내합니다.
- **드럼에 맞춘 재생**
  - 고스트 노트는 작게, 플램의 꾸밈음은 약하게 소리 냅니다.
  - 하이햇에 `o`를 붙이면 오픈 하이햇 소리가 납니다.
  - 스윙 재생을 한 번에 켜고 끕니다.
- **연습**: 구간을 반복하며 템포를 조금씩 올리기, 탭 템포, 곡 구조(Intro·Verse·Chorus …) 목록으로 섹션 이동
- **원곡과 함께 재생**
  - 녹음으로 만든 악보는 그 녹음을 기억해서, 믹서에 '녹음' 트랙이 생깁니다.
  - 악보에는 템포가 하나만 깔끔하게 표시되지만, 재생은 원곡의 실제 템포 흐름을 마디마다 따라갑니다.

**녹음에서 드럼 악보 만들기** (`파일` 메뉴, 또는 녹음 파일을 창에 끌어다 놓기)
- 노래에서 드럼 소리를 분리하고, 박자와 마디를 찾고, **KitNote 자체 드럼 모델**로 타격을 인식합니다.
- 이 모델은 공개 라이선스 자료로만 학습했습니다: 실제 드러머의 연주, 그리고 여러 스타일의 그루브를 공개 라이선스 드럼 킷 51종으로 연주한 자료입니다.
- 하이햇 열림/닫힘, 크래시/라이드, 어느 탐인지, 고스트 노트, 박자표(4/4·3/4·6/8·12/8), 섹션을 자동으로 판단하고, 반복되는 마디는 % 기호로 씁니다.
- 결과는 완성된 채보가 아니라 고쳐 나갈 출발점입니다.
- 오디오 엔진(약 1GB)은 동의한 뒤에만 내려받습니다.

**AI 도구 연동** (`편집 → 환경 설정 → AI`)
- 버튼 한 번으로 Claude Code·Codex·Claude Desktop·Cursor·Gemini CLI에 KitNote의 로컬 MCP 서버(`kitnote-mcp.exe`)와 스킬이 설치됩니다.
- 그다음 평소 말로 요청하면 됩니다. 예:
  - "KitNote로 120 BPM 팝록 드럼 악보 만들어 줘. 인트로 4, 벌스 8, 코러스 8(라이드), 섹션 끝마다 필인. PDF랑 MIDI도."
  - "song.kitnote 9~16마디를 하이햇에서 라이드로 바꾸고 마지막 마디에 탐 필인 넣어 줘."
  - "C:/Music/song.mp3 드럼 악보로 만들어 줘."
- 악보는 `.kitnote`로 저장되고, PDF·PNG·SVG·MIDI·MusicXML·MP3/WAV/OGG/FLAC·텍스트 탭으로 내보낼 수 있습니다.

![스텝 그리드](docs/kitnote/screenshot-stepgrid.png)

## 설치

[Releases](../../releases) 페이지에서 내려받습니다. 각 릴리스에는 파일마다 체크섬을 적은 `SHA256SUMS.txt`가 함께 있습니다.

### Windows

- **설치 프로그램 (추천):** `KitNote-<버전>-win64-setup.exe`를 내려받아 실행합니다.
  - 관리자 권한 없이 내 계정에 설치하며, 모든 사용자용으로도 설치할 수 있습니다.
  - 시작 메뉴와 바탕화면 바로가기가 생기고, `.kitnote` 파일이 KitNote로 열립니다.
  - '설치된 앱'에서 지울 수 있습니다.
- **압축 파일 (설치 없이 사용):** `KitNote-<버전>-win64.zip`을 내려받아 압축을 풀고 `bin\KitNote.exe`를 실행합니다. 압축을 푼 폴더에서 `powershell -ExecutionPolicy Bypass -File install.ps1`을 실행하면 설치할 수도 있습니다.

프로그램에 아직 코드 서명이 없어서, 처음 실행하면 Windows SmartScreen 경고가 뜹니다. *추가 정보*를 누른 다음 *실행*을 누르세요.

### macOS

1. `.dmg` 파일을 내려받아 엽니다. macOS 14.4 이상에서 실행되며 Apple 실리콘과 인텔 Mac을 모두 지원합니다.
2. **KitNote**를 **응용 프로그램**(Applications) 폴더로 끌어 넣습니다.
3. 아직 공증(notarization)을 받지 않아서 처음 실행이 막힐 수 있습니다. *시스템 설정 > 개인정보 보호 및 보안*에서 KitNote 관련 메시지 아래의 *그래도 열기*를 누른 뒤 확인하세요. macOS 14 이하에서는 응용 프로그램에서 KitNote를 control-클릭하고 *열기*를 골라도 됩니다.

인터페이스는 한국어와 영어를 지원합니다. 타악기가 아닌 악기가 들어 있는 악보는 열지 않습니다.

## 문서

- 사용법: [한국어](docs/kitnote/user-guide.ko.md) · [English](docs/kitnote/user-guide.en.md). 프로그램의 `도움말 → KitNote 사용법`에서도 열립니다.
- MuseScore Studio에서 무엇을 바꿨는지, 빌드·패키징 방법: [KITNOTE.md](KITNOTE.md)
- 빌드, 테스트, 빌드 중 내려받는 파일, 드럼 모델 다시 학습하기: [CONTRIBUTING.md](CONTRIBUTING.md)
- 서드파티 구성 요소와 드럼 모델 학습 데이터: [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)

## 소스에서 빌드하기

Windows에서 Visual Studio 2022 이상, CMake, Ninja, Qt 6.10(msvc2022_64)이 필요합니다. 간단히 하면:

```bash
bash ./ninja_build.sh -t install
python tools/kitnote/smoke_test.py
```

필요한 Qt 모듈을 포함한 자세한 방법은 [CONTRIBUTING.md](CONTRIBUTING.md)에 있습니다.

## 감사의 말

KitNote는 [MuseScore Studio](https://musescore.org)와 그 기여자들의 작업 위에 서 있습니다.
- 녹음 채보: 드럼 분리에 [Demucs](https://github.com/facebookresearch/demucs), 박자 인식에 [Beat This!](https://github.com/CPJKU/beat_this)를 씁니다.
- 드럼 모델 학습 데이터: Groove MIDI Dataset과 E-GMD(Google Magenta), 그리고 공개 라이선스 샘플 킷들입니다. 자세한 내용은 [share/ai/kitnote_drums.NOTICE.txt](share/ai/kitnote_drums.NOTICE.txt)에 있습니다.

## 라이선스와 상표

KitNote는 GNU General Public License v3.0을 따르는 자유 소프트웨어입니다. [LICENSE.txt](LICENSE.txt)를 보세요.

MuseScore는 MuseScore Limited의 상표입니다. KitNote는 MuseScore Studio 4.7.5를 고쳐 만든 독립적인 수정판이며,
MuseScore Limited나 Muse Group과 제휴하거나 그들의 승인을 받지 않았습니다. "MuseScore Studio", "MuseScore 파일(*.mscz)"처럼
원본 프로그램과 파일 형식을 가리키는 표현은 설명을 위해서만 씁니다.
