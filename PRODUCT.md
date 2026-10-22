# Product

<!-- impeccable:product-schema 1 -->

## Platform

web (this record covers the KitNote website in `website/`; the KitNote app itself is a Qt desktop program for Windows and macOS)

## Stack

Static HTML, CSS and JavaScript in `website/`, no build step, deployed on Vercel (the owner's choice).

## Users

Drummers anywhere in the world who read and write drum charts: players learning songs, teachers writing parts for students, band drummers charting a set. They arrive from GitHub, a shared link or a search, and decide within a minute whether KitNote is worth downloading. English is the default language; Korean is a full second language (the owner and the first users are Korean).

## Product Purpose

KitNote is a drum notation editor made only for the drum kit. It is a fork of MuseScore Studio 4.7.5 that keeps its engraving, playback and editing and rebuilds everything around them for drums. The website exists to make a drummer understand that in seconds, show the real app, and get the right download (Windows installer, Windows zip, macOS dmg) onto their computer.

## Positioning

General notation programs treat drums as one instrument among many. KitNote only knows the drum kit: drum-only templates, palettes and pads, a step grid, a pattern library of grooves, fills and rudiments, published-chart engraving by default, drum-aware playback, drum scores made from recordings by its own drum model, and a local MCP server for AI tools. It is free and open source (GPL-3.0).

The first message is "a notation editor made only for drums"; recording transcription, AI tools and open source support it.

## Operating Context

- Downloads come from GitHub Releases of `hs-hui/KitNote`: `KitNote-<version>-win64-setup.exe`, `KitNote-<version>-win64.zip`, `KitNote-<version>-macos-universal.dmg`, `SHA256SUMS.txt`.
- Windows builds are not code-signed (SmartScreen warning on first start); the macOS app is not notarized (Privacy & Security → Open Anyway).
- macOS 14.4 or later, Apple silicon and Intel. Windows 10/11 64-bit.
- Scores are `.kitnote` files; export to PDF, PNG, SVG, MIDI, MusicXML, MP3/WAV/OGG/FLAC and text drum tab.
- The AI tools (Claude Code, Codex, Claude Desktop, Cursor, Gemini CLI) connect through `kitnote-mcp`; there is no AI chat inside the app.

## Capabilities and Constraints

Confirmed features (README.md is the source): step grid, pattern library, drum patterns and rudiments, drum tab in and out, selection tools (hi-hat ↔ ride, sticking, counting), published-chart engraving (four measures a line, boxed sections, % repeats with counts, merged rests), drum key, drum-aware playback (ghost notes, flams, open hi-hat, swing), practice (rising-tempo loop, tap tempo, sections), play along with the recording, drum score from a recording (separation, beat, KitNote's own drum model), MCP server and skill for AI tools. Interface in English and Korean. Scores with pitched instruments are not opened.

Transcription is a starting point to correct, not a finished transcription; the audio engine (about 1 GB) downloads only after the user agrees.

## Brand Commitments

- Name: KitNote. Logo: the mark of four beamed notes with x heads on a staff (`share/icons/KitNoteMark.svg`) and the light tile app icon (`share/icons/KitNoteLogo.svg`).
- The website follows the KitNote app's own design (owner's instruction): the app's light paper-and-ink interface, its step-grid colours (hi-hat amber, snare sage, kick coral), Pretendard, and the app's calm, tool-like character. The dark neon promo-video look is not the website's look.
- MuseScore is a trademark of MuseScore Limited; KitNote must say it is an independent modification, not affiliated with or endorsed by MuseScore Limited or Muse Group. Upstream copyright notices stay.

## Evidence on Hand

- App screenshots: `docs/kitnote/screenshot*.png|jpg` (demo scores "KitNote Groove"), recordings in the promo working folder (demo score "Neon Run").
- Promo video: `dist/kitnote-promo-v2.mp4` (43 s, dark style).
- Release v1.0.0 assets on GitHub.
- No testimonials, user counts, press, ratings or download numbers exist; none may be invented.
- The owner's own songs and purchased charts must never appear in public material, and no paths with the owner's user name.

## Product Principles

1. Drums first: every word and picture is about the drum kit, never generic notation.
2. Show the real app; claims must match README.md and the shipped release.
3. Honest about limits: unsigned builds, transcription as a starting point, opt-in downloads.
4. Free and open: no account, GPL-3.0, source one click away.

## Accessibility & Inclusion

English and Korean. No product-specific standard beyond sound web practice was set.
