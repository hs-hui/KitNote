---
name: KitNote Website
description: The KitNote landing page, drawn from the app's own light theme: paper, ink, sage and the three drum-row colours.
colors:
  paper: "#F4F3EE"
  paper-2: "#E9E7E0"
  popup: "#FAF9F5"
  field: "#FFFFFF"
  canvas: "#E3E1D9"
  stroke: "#D8D5CB"
  button: "#DAD7CD"
  ink: "#2B2B28"
  ink-2: "#55534C"
  muted: "#6F6C63"
  ink-hover: "#3C3C38"
  sage: "#8DB3A4"
  sage-ink: "#3F6F5E"
  focus: "#75507B"
  hat: "#E3B95B"
  kick: "#D98B6A"
typography:
  display:
    fontFamily: "Pretendard, -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif"
    fontSize: "clamp(46px, 5.4vw, 78px)"
    fontWeight: 800
    lineHeight: 1.02
    letterSpacing: "-0.04em"
  headline:
    fontFamily: "Pretendard, -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif"
    fontSize: "clamp(30px, 3.2vw, 46px)"
    fontWeight: 800
    lineHeight: 1.1
    letterSpacing: "-0.03em"
  title:
    fontFamily: "Pretendard, -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif"
    fontSize: "clamp(24px, 2.2vw, 30px)"
    fontWeight: 800
    lineHeight: 1.15
    letterSpacing: "-0.025em"
  body:
    fontFamily: "Pretendard, -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif"
    fontSize: "16px"
    fontWeight: 400
    lineHeight: 1.55
    letterSpacing: "normal"
  label:
    fontFamily: "Pretendard, -apple-system, BlinkMacSystemFont, 'Segoe UI', 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif"
    fontSize: "14px"
    fontWeight: 500
    lineHeight: 1.55
    letterSpacing: "normal"
rounded:
  sm: "6px"
  md: "8px"
  lg: "10px"
  xl: "12px"
  pill: "99px"
spacing:
  xs: "4px"
  sm: "8px"
  md: "16px"
  lg: "24px"
components:
  button-primary:
    backgroundColor: "{colors.ink}"
    textColor: "{colors.paper}"
    rounded: "{rounded.md}"
    height: "40px"
    padding: "0 16px"
  button-primary-hover:
    backgroundColor: "{colors.ink-hover}"
  button-secondary:
    backgroundColor: "{colors.button}"
    textColor: "{colors.ink}"
    rounded: "{rounded.md}"
    height: "40px"
    padding: "0 16px"
  selection-pill:
    backgroundColor: "{colors.paper-2}"
    textColor: "{colors.ink}"
    rounded: "{rounded.pill}"
    height: "30px"
    padding: "0 12px"
  selection-pill-selected:
    backgroundColor: "{colors.sage}"
    textColor: "{colors.ink}"
  popup:
    backgroundColor: "{colors.popup}"
    rounded: "{rounded.xl}"
  tempo-field:
    backgroundColor: "{colors.field}"
    rounded: "{rounded.md}"
  step-cell:
    backgroundColor: "{colors.canvas}"
    height: "26px"
  terminal:
    backgroundColor: "{colors.ink}"
    textColor: "{colors.paper}"
---

# Design System: KitNote Website

This system describes `website/` (index.html, style.css, main.js). The website mirrors the desktop app's light theme (src/app/configs/light.cfg) and its step-grid colours. The values below were read from the shipped `:root` block and component rules in `website/style.css`, and the drum-row colours from `website/main.js`. Where the page and the app differ, the page is recorded as built.

## Overview

**Creative North Star: "The Pattern Library, Opened Full-Size"**

The page is the app's own pattern library at life size. The hero is the step-grid popup, a working instrument rather than an illustration. Below it, the pattern cards and the other features (audio to chart, engraving, AI) are tabs of the same popup. Nothing is set apart from the app: the same paper ground, the same popup panels with hairline strokes, the same ink buttons and the same sage selection pill.

The density is calm and dense at once. Grids carry the content: sixteen cells per row, nine labelled rows, mini grids on the cards. Text is short and sits beside the artefact it describes. Headlines are heavy and tightly tracked; everything else is quiet.

Divergence noted in the build: the popup insets differ between the step-grid popup (22px) and the library and download popups (26px), and a handful of literal colours (hover states, the waveform, a cell hover) sit outside `:root`. They are recorded here as built and are not canonical.

**Key Characteristics:**
- Warm paper ground (#F4F3EE) with ink (#2B2B28) type; no pure white page.
- Popup panels as the only raised surfaces: hairline stroke, 12px radius, soft shadow.
- Sage (#8DB3A4) marks selection; ink marks action.
- Pretendard 800 headlines, Pretendard 400 body, Korean line-breaking set to keep-all.
- The drum-row colours and the focus violet are the only saturated colours on the page.

## Colors

The palette is the app's light theme, nothing added. It is warm and low-chroma; the only saturated colours are the drum-row colours in the step grid and the focus violet, which appears only as the keyboard focus ring.

### Primary
- **Ink** (#2B2B28): Primary text, the filled primary button (Download, play, the terminal's paper-on-ink surface), and the accent outline on step cells. The strongest colour on the page.
- **Ink Hover** (#3C3C38): Hover state of the ink button and the play button.

### Secondary
- **Sage** (#8DB3A4): The selected state of section tabs, selection pills and pattern cards, the selection highlight (at 45% alpha), and the snare and tom row of the step grid.
- **Sage Ink** (#3F6F5E): Links, the version figure, the progress bar of the audio waveform, and the emphasis word in the hero headline.

### Tertiary
- **Hat Amber** (#E3B95B): Cymbals, hi-hats and the ride row of the step grid; the "demo" tag in the terminal; the "done" state of a called tool.
- **Kick Coral** (#D98B6A): The bass drum row of the step grid.
- **Focus Violet** (#75507B): Keyboard focus ring only (2px outline, 2px offset). It is the single cool-violet note on the page.

### Neutral
- **Paper** (#F4F3EE): The page ground and the top bar.
- **Paper Two** (#E9E7E0): Hover fill for tabs and icon buttons, the fill of metadata chips and the version chip, and the tabline strip.
- **Popup Cream** (#FAF9F5): Popup panel background.
- **Field White** (#FFFFFF): Text fields, the tempo control, the drop zone.
- **Score Canvas** (#E3E1D9): The engraved-page backdrop and the empty step cell.
- **Stroke** (#D8D5CB): Hairline borders, rules between rows, and the top bar's bottom edge.
- **Button Grey** (#DAD7CD): Secondary (grey) buttons and the AI tool pills.
- **Ink Two** (#55534C): Lede, row labels and secondary body text.
- **Muted** (#6F6C63): Meta lines, hints, notes and captions.

### Named Rules
**The App Is The Page Rule.** Website colours come from the app's light theme and nothing else. A new colour on the site must first exist in the app.

**The Sage Means Selected Rule.** Sage as a fill means "this is the current choice": the active tab, a pressed chip, a selected card, the row for your own platform. Sage is also the snare row's colour in the step grid, so it is shared; no other solid fill uses it. Sage-ink as text means a link or a version figure.

## Typography

**Display Font:** Pretendard (with -apple-system, BlinkMacSystemFont, Segoe UI, Apple SD Gothic Neo, Malgun Gothic, sans-serif). Loaded as three subset WOFF2 files: weight 400, 500–600, and 700–900.
**Body Font:** Pretendard, the same family.
**Label/Mono Font:** ui-monospace, Cascadia Mono, Consolas, Menlo (used only for the drum tab, file names and checksums).

**Character:** One family carries the whole page, Latin and Hangul alike. Weight does the hierarchy work: 800 for headlines, 600 for popup titles and buttons, 500 for tabs and chips, 400 for reading text.

### Hierarchy
- **Display** (800, clamp(46px, 5.4vw, 78px), line-height 1.02, tracking -0.04em): The two-line hero headline "Drum notation. / Nothing else." The second line is set in sage-ink, not in a different face.
- **Headline** (800, clamp(30px, 3.2vw, 46px), line-height 1.1, tracking -0.03em): Section headings: "Pattern library", "Download KitNote". The pattern library heading is smaller, clamp(26px, 2.6vw, 36px).
- **Title** (800, clamp(24px, 2.2vw, 30px), line-height 1.15, tracking -0.025em): Feature panel headings ("From a recording to a drum chart.", "Or just ask your AI.").
- **Popup Title** (600, 15px, tracking 0): The step-grid and popup headers. It is the only small heading and sits inside a panel, not on the page.
- **Lede** (400, clamp(17px, 1.35vw, 19px), line-height 1.55, ink-2, max 31em): The one-sentence introduction under the hero headline.
- **Body** (400, 16px, line-height 1.55; 16.5px in feature lists and panel copy, max 34em): Reading text. Line length is held to about 65 characters by the panel width.
- **Label** (500, 14px, normal tracking): Tabs, chips, pills, and the small grey captions beneath them (13–14px, muted).

Korean text sets `word-break: keep-all` on the body, so Hangul words are not split mid-word.

### Named Rules
**The Heavy Headline Rule.** Headlines are 800 weight with negative tracking. Body text, labels and popup titles stay at 600 or below; the wordmark and the terminal prompt are the only 700s. Weight marks importance; size alone is not enough.

## Layout

The page is a single centred column, `max-width: 1312px`, with horizontal padding of `clamp(16px, 4.4vw, 64px)`. The top bar is 64px tall and sticks to the top at z-index 30.

The hero is two columns: the copy at `minmax(0, 33rem)` and the step-grid popup at `minmax(0, 1fr)`, with a column gap of `clamp(32px, 4vw, 64px)`, vertically centred. At a 1440px viewport the popup is about 600px wide. Sections begin with a top padding of `clamp(64px, 8vw, 112px)`.

Inside the step-grid popup the grid is one label column (7.6em) followed by sixteen equal cells, with a 4px column gap and a 5px row gap. Every fourth cell has a 6px extra left margin, so beats read as groups of four. Cells are 26px tall.

Popups inset their content by 22px (step grid) or 26px (pattern library, download). This difference is a build inconsistency and is recorded as built.

The pattern cards are three columns, with a 12px gap and 14–16px inner padding.

**Breakpoints.** At 1100px and below: the section tabs are hidden, the hero and feature panels collapse to one column (the feature copy moves above its visual on the engraving panel), and the cards go to two columns. At 640px and below: the GitHub icon link is hidden, the grid trims to a 4.6em label column with 2px gaps, and cells shrink to 22px tall with 3px corners, the cards go to one column, and the download rows stack.

## Elevation & Depth

Depth is mostly tonal. The page and the cards sit flat on paper-two and stroke. Only popups and the engraved page receive a soft, ambient shadow. There are no hard offset shadows, no blur glows and no gradients on any surface.

### Shadow Vocabulary
- **Popup Lift** (`box-shadow: 0 1px 2px rgba(43, 43, 40, .06), 0 18px 40px -22px rgba(43, 43, 40, .38)`): Every popup panel. A tight contact shadow plus a long, low ambient one.
- **Engraved Page** (`box-shadow: 0 2px 6px rgba(43, 43, 40, .08), 0 20px 40px -26px rgba(43, 43, 40, .45)`): The score-page image on the canvas.
- **Playhead Ring** (`box-shadow: 0 0 0 2px rgba(43, 43, 40, .28)`): The step cell that the transport is on. It is a state ring, not a shadow, and it has no blur.
- **Accent Inset** (`box-shadow: inset 0 0 0 2px var(--ink)`): A cell marked as an accent. Inset, no offset.

### Named Rules
**The Lift Is For Popups Rule.** A surface gets a shadow only when it is a popup or an engraved page. Cards, tabs and fields are separated by fill or a 1px stroke, never by shadow.

## Shapes

The form language is soft and mostly round. Radii follow the role of the element rather than one global value.

- **Pills (99px):** Section tabs, selection pills, the version chip, feature facts, the sage selection state.
- **Popups (12px):** The app's popup radius (--radius). Panels and the download list.
- **Cards, drop zones and large buttons (10px):** Pattern cards, the audio drop zone, the engraved-page frame, and the large hero buttons.
- **Controls (8px):** Default buttons, the tempo field, the tabline strip, the AI tool pills.
- **Small controls (6px):** Icon buttons, the small text buttons, and the keyboard focus ring.
- **Grid cells (5px):** Step cells; mini-grid cells use 2px.
- **Circles:** The play button (44px, 38px small) and the window dots.

Borders are 1px strokes in stroke (#D8D5CB), and only on popups, fields, the top bar, and row dividers. Images carry no border; the engraved page is a white sheet on canvas.

## Components

Each component is named by its character first, then its states.

### Buttons
Buttons are plain and direct; they have a fill, a short label and an optional 18px icon.
- **Shape:** 8px radius; 40px height; 0 16px padding; 600 weight at 15px. Small is 34px high; extra-small is 30px high with a 6px radius; large is 52px high with a 10px radius.
- **Primary (ink):** Ink (#2B2B28) fill, paper text. Reserved for the download action (hero, top bar, and each download row). Hover moves to ink-hover (#3C3C38). Pressing moves the button down 1px.
- **Secondary (grey):** Button Grey (#DAD7CD) fill, ink text. Hover darkens the grey to #CFCCC1. Used for the second platform option and the small tool actions.
- **Icon button:** 32px square (28px small), transparent, 6px radius, a Paper Two fill on hover. Disabled at 30% opacity.
- **Play:** A 44px ink circle with a paper glyph. Hover moves to ink-hover; pressing scales it to 96%.

### Selection Pills
- **Section tabs and the library tabs:** Transparent at rest; Paper Two on hover; a sage fill with ink text when selected (aria-selected). 14px, 500 weight, 7px by 15px padding.
- **Preset chips:** Paper Two fill, ink text, 30px high. Hover moves to Button Grey. Pressed (aria-pressed) becomes sage with ink text.
- **Metadata chips:** Paper Two fill, 14px text, no interaction.

### Popups and Panels
- **Shape and fill:** Popup Cream (#FAF9F5) fill, 1px stroke, 12px radius, Popup Lift shadow.
- **Feature panels:** Not separate cards. A panel is a tab's content inside the library popup. It has a title, a short body, and a visual; it is laid out in two columns above 1100px.
- **Pattern cards:** Paper Two fill, transparent 1px border, 10px radius, 14–16px padding. Hover moves to #E3E1D9. The selected card takes Field White and a sage border.

### Inputs and Fields
- **Tempo field:** Field White fill, 1px stroke, 8px radius, 2px padding. It holds two icon buttons and the BPM readout (600 weight, tabular figures).
- **Drop zone:** Field White fill, 1px stroke, 10px radius, 14–16px padding; holds a play button, the file name (600) and a muted caption.
- **Focus (all interactive elements):** A 2px Focus Violet outline, 2px offset, 6px radius. Nothing else changes.

### Step Grid (signature)
The step grid is the page's signature component. It is a 16 by 9 grid of 26px cells on the paper popup.
- **Off cell:** Score Canvas (#E3E1D9) fill. Hover moves to #D3D0C6.
- **On cell:** The row colour. The row colour is set per row through a custom property: Hat Amber (#E3B95B) for cymbals and hi-hats, Sage (#8DB3A4) for snare and toms, Kick Coral (#D98B6A) for the bass drum. Hover darkens the on-cell by 5%.
- **Accent:** An inset 2px ink ring on the cell.
- **Ghost note:** A transparent cell with an inset 2px ring in the row colour.
- **Playhead:** Playhead Ring shadow on the cell being played.

### Tab Line and Terminal
- **Tab line:** Paper Two strip, 8px radius, a monospace drum tab in ink-two at 12.5px, with the "copy tab" button at its right end. Bold stroke letters carry a 45% sage highlight.
- **Terminal (AI demo):** An ink surface (#2B2B28) with paper text, 14px radius, no shadow. The header and the call list use #A9A79E for secondary text. Its tool-call dots are sage while running and Hat Amber when done.

### Navigation
- **Top bar:** Paper ground, 64px high, sticky, with a 1px stroke bottom edge. Brand is 700 at 19px with the mark on the left; section tabs in the centre; the language toggle, GitHub link and ink Download button on the right.
- **Mobile:** Section tabs are hidden at 1100px and below; the GitHub link is hidden at 640px and below.

### Footer
Stroke top rule, 32px top padding and 48px bottom padding. Links are ink-two at 14px and become ink on hover with an underline. The legal line is muted at 13px.

## Do's and Don'ts

### Do:
- **Do** use Paper (#F4F3EE) as the page ground and Popup Cream (#FAF9F5) for popups, each popup with a 1px Stroke (#D8D5CB) border and a 12px radius.
- **Do** use the ink (#2B2B28) button for the download action only, and Button Grey (#DAD7CD) for the alternative platform action.
- **Do** colour the step grid with the three drum-row colours only: Hat Amber for cymbals and hi-hats, Sage for snare and toms, Kick Coral for the bass drum.
- **Do** show keyboard focus as a 2px Focus Violet (#75507B) outline with a 2px offset.
- **Do** keep Korean body text at `word-break: keep-all`.
- **Do** respect `prefers-reduced-motion`: stop the caret blink and the smooth scroll.
- **Do** keep the headline weight at 800 with tracking between -0.02em and -0.04em.

### Don't:
- **Don't** use gradients, glass effects, glows or neon. The app's light theme has none, and the page has none; the only shadows are the soft popup lift and the engraved-page lift.
- **Don't** add a colour that is not in the app's light theme. A new brand colour starts in the app.
- **Don't** use sage as a fill for anything other than the current choice.
- **Don't** put a shadow on a card, tab, field or button; they are separated by fill or stroke.
