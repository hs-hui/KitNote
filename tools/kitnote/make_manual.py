#!/usr/bin/env python3
"""Turns the KitNote user guides into the HTML pages that Help > KitNote user guide opens.

    python tools/kitnote/make_manual.py

Reads docs/kitnote/user-guide.<lang>.md and writes share/manual/kitnote/user-guide_<lang>.html.
The guides use a small part of Markdown (headings, paragraphs, lists, tables nested in lists,
`code`, **bold**), and this script handles just that, so no package is needed.
"""
import html
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "docs" / "kitnote"
DST = ROOT / "share" / "manual" / "kitnote"
LANGS = {"en": "en", "ko": "ko"}

CSS = """
:root { color-scheme: light dark; --fg: #1d2330; --bg: #ffffff; --muted: #5d6677; --line: #d8dde6; --key: #eef1f6; --accent: #2e6fe0; }
@media (prefers-color-scheme: dark) {
  :root { --fg: #e6e9ef; --bg: #1b1f27; --muted: #9aa3b2; --line: #343b48; --key: #2a303b; --accent: #7aa7ff; }
}
* { box-sizing: border-box; }
body { margin: 0; background: var(--bg); color: var(--fg); font: 16px/1.65 Pretendard, "Segoe UI", "Malgun Gothic", system-ui, sans-serif; }
main { max-width: 820px; margin: 0 auto; padding: 40px 20px 80px; }
h1 { font-size: 30px; margin: 0 0 12px; }
h2 { font-size: 21px; margin: 44px 0 12px; padding-bottom: 6px; border-bottom: 1px solid var(--line); }
h3 { font-size: 17px; margin: 28px 0 8px; }
p, li { margin: 8px 0; }
ol, ul { padding-left: 24px; }
pre { background: var(--key); border: 1px solid var(--line); border-radius: 8px; padding: 12px 16px; font: 0.9em/1.35 Consolas, "Cascadia Mono", monospace; overflow-x: auto; }
code { background: var(--key); border: 1px solid var(--line); border-radius: 5px; padding: 1px 6px; font: 0.88em Consolas, "Cascadia Mono", monospace; white-space: nowrap; }
table { border-collapse: collapse; margin: 12px 0; width: 100%; }
th, td { border: 1px solid var(--line); padding: 7px 12px; text-align: left; vertical-align: top; }
th { background: var(--key); font-weight: 600; }
nav { margin: 20px 0 0; padding: 12px 18px; border: 1px solid var(--line); border-radius: 8px; }
nav a { color: var(--accent); text-decoration: none; margin-right: 16px; white-space: nowrap; }
nav a:hover { text-decoration: underline; }
footer { margin-top: 56px; color: var(--muted); font-size: 13px; }
@media (max-width: 600px) {
  main { padding: 24px 14px 60px; overflow-wrap: anywhere; }
  code { white-space: normal; }
  th, td { padding: 6px 8px; }
}
"""


def inline(text: str) -> str:
    text = html.escape(text.replace("\\*", "\x00"), quote=False)
    text = re.sub(r"`([^`]+)`", r"<code>\1</code>", text)
    text = re.sub(r"\*\*([^*]+)\*\*", r"<strong>\1</strong>", text)
    return text.replace("\x00", "*")


def slug(index: int) -> str:
    return f"s{index}"


def convert(lines: list[str]) -> tuple[str, str, list[str]]:
    """Returns (title, body html, section titles)."""
    out: list[str] = []
    sections: list[str] = []
    title = ""
    stack: list[str] = []          # open list tags
    in_item = False
    i = 0

    def close_lists() -> None:
        nonlocal in_item
        if in_item:
            out.append("</li>")
            in_item = False
        while stack:
            out.append(f"</{stack.pop()}>")

    while i < len(lines):
        line = lines[i].rstrip("\n")
        stripped = line.strip()

        if not stripped:
            i += 1
            continue

        if stripped.startswith("```"):
            # a fenced block: kept as it is, in a font with characters of equal width (a drum tab)
            close_lists()
            block = []
            i += 1
            while i < len(lines) and not lines[i].strip().startswith("```"):
                block.append(html.escape(lines[i].rstrip()))
                i += 1
            i += 1
            out.append("<pre>" + chr(10).join(block) + "</pre>")
            continue

        if stripped.startswith("|"):
            rows = []
            while i < len(lines) and lines[i].strip().startswith("|"):
                rows.append([c.strip() for c in lines[i].strip().strip("|").split("|")])
                i += 1
            head, body = rows[0], rows[2:]
            out.append("<table><thead><tr>" + "".join(f"<th>{inline(c)}</th>" for c in head) + "</tr></thead><tbody>")
            out.extend("<tr>" + "".join(f"<td>{inline(c)}</td>" for c in row) + "</tr>" for row in body)
            out.append("</tbody></table>")
            continue

        heading = re.match(r"(#{1,3}) (.+)", stripped)
        if heading and not line.startswith(" "):
            close_lists()
            if len(heading.group(1)) == 1:
                title = heading.group(2)
                out.append(f"<h1>{inline(title)}</h1>\x01")      # the contents go here
            elif len(heading.group(1)) == 3:
                out.append(f"<h3>{inline(heading.group(2))}</h3>")       # not listed in the contents
            else:
                sections.append(heading.group(2))
                out.append(f'<h2 id="{slug(len(sections))}">{inline(heading.group(2))}</h2>')
            i += 1
            continue

        item = re.match(r"(\d+\.|-) (.+)", stripped)
        if item and not line.startswith(" "):
            tag = "ol" if item.group(1) != "-" else "ul"
            if in_item:
                out.append("</li>")
            if not stack or stack[-1] != tag:
                while stack:
                    out.append(f"</{stack.pop()}>")
                stack.append(tag)
                out.append(f"<{tag}>")
            out.append(f"<li>{inline(item.group(2))}")
            in_item = True
            i += 1
            continue

        if not line.startswith(" "):
            close_lists()
        out.append(f"<p>{inline(stripped)}</p>")
        i += 1

    close_lists()
    return title, "\n".join(out), sections


def main() -> None:
    DST.mkdir(parents=True, exist_ok=True)
    for lang, html_lang in LANGS.items():
        source = SRC / f"user-guide.{lang}.md"
        title, body, sections = convert(source.read_text(encoding="utf-8").splitlines())
        nav = "<nav>" + "".join(f'<a href="#{slug(n)}">{inline(s)}</a>' for n, s in enumerate(sections, 1)) + "</nav>"
        page = (f'<!DOCTYPE html>\n<html lang="{html_lang}">\n<head>\n<meta charset="utf-8">\n'
                f'<meta name="viewport" content="width=device-width, initial-scale=1">\n<title>{html.escape(title)}</title>\n'
                f"<style>{CSS}</style>\n</head>\n<body>\n<main>\n{body.replace(chr(1), nav)}\n"
                f"<footer>KitNote is based on MuseScore Studio and is distributed under the GNU General Public License version 3.</footer>\n"
                f"</main>\n</body>\n</html>\n")
        target = DST / f"user-guide_{lang}.html"
        target.write_text(page, encoding="utf-8", newline="\n")
        print(f"{target.relative_to(ROOT)}  ({len(sections)} sections)")


if __name__ == "__main__":
    main()
