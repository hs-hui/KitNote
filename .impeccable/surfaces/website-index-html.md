---
version: 1
slug: "website-index-html"
primary_target: "website/index.html"
related_targets: []
---

# KitNote website — surface brief

Scope: `website/index.html` (+ `style.css`, `main.js`, `assets/`), the public landing page, deployed on Vercel.
Visitor mode: Persuade.

Audience and job: drummers worldwide (English default, Korean full second language) deciding in under a minute whether to download KitNote. Action: download the right build (Windows setup.exe / zip, macOS dmg). Proof: the real step grid working in the page (plays sound, copies a tab KitNote pastes), the pattern library, real app screenshots at legible size, the AI terminal demo. Must keep: the playable step grid and the AI terminal demo. Must avoid (owner): common SaaS landing look, a site that feels separate from the app, too much text, small or blurry screenshots.

Chosen structure: "the pattern library is the page" (surface roll 27f5fcb6, dealt index 3 of 7, picked by the owner).

## Direction contract

THESIS: The site is KitNote's own pattern library opened full-size: the hero is the step-grid popup working at life size, the patterns below are library cards with mini grids you can press to hear, and the other features (audio to chart, engraving, AI) are tabs of the same library popup. It refuses the split hero over a row of icon feature cards.

OWN-WORLD: The app's light theme verbatim: paper #F4F3EE ground, popup #FAF9F5 panels with #D8D5CB hairline and 12-14px radius, ink #2B2B28 text and primary buttons, button grey #DAD7CD, sage #8DB3A4 active tab pill, link #3F6F5E, step cells #E3E1D9 off / amber #E3B95B cymbals / sage snare & toms / coral #D98B6A kick, Pretendard 800 headlines. No gradients, glass, glow or neon.

STORY: A drummer sees "Drum notation. Nothing else.", presses play and hears the groove they tapped, flips library tabs (Grooves, Fills, Rudiments, Audio → chart, AI) to see each feature as a card with a real app view, and downloads the build for their OS.

FIRST VIEWPORT: 1440x900. Top bar (mark + KitNote, sage-pill section tabs, ink Download). Left column: H1 two lines, one-sentence lede, ink "Download for <OS>" + grey second platform, version line. Right: the step-grid popup at the app's size (~600px), the app's nine labelled rows x 16 cells, play button, BPM, preset chips, live tab line; it plays. Below the fold begins "Pattern library" with tabs.

FORM: the pattern-library popup as page structure; position 3 of the ordered list; seed key 27f5fcb6.

FINISH: unreviewed and undocumented is unfinished; this build ends with the finish review, the verdict, DESIGN.md, and every shipping raster carrying its provenance
