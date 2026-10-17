# Adding a new language

KitNote keeps its translations in this folder. `kitnote_<lang>.ts` holds the interface strings and `instruments_<lang>.ts` the instrument names. They are Qt Linguist files.

1. Copy `kitnote_en.ts` to `kitnote_LANG.ts` (and `instruments_en.ts` to `instruments_LANG.ts`), then translate it with Qt Linguist or in a text editor. `tools/translations/run_lupdate.sh` updates the source file when strings change; `tools/translations/run_lrelease.sh` makes the `.qm` files.

2. Add the language name and/or code to these files:

    - `share/locale/languages.json`
    - `buildscripts/packaging/macOS/Info.plist.in`

3. Commit the changes and submit a PR.
