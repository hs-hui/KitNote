#!/usr/bin/env python3
"""Generates KitNote's icon, logo and loading screen.

KitNote may not ship MuseScore's logo, so these replace the upstream artwork.
The logo is share/icons/KitNoteLogo.svg (on its tile) and KitNoteMark.svg (the
mark alone); the script writes the SVG files the application uses and
rasterises the icon into the PNG sizes, the .ico and the .icns the build expects. Needs PySide6 (for SVG rendering) and Pillow (for .ico).

    python tools/kitnote/make_branding.py
"""
import sys
from pathlib import Path

from PIL import Image
from PySide6.QtCore import QByteArray, Qt
from PySide6.QtGui import QFontDatabase, QGuiApplication, QImage, QPainter
from PySide6.QtSvg import QSvgRenderer

ROOT = Path(__file__).resolve().parents[2]
# The colours of the interface: ink and paper (src/app/configs/light.cfg).
NAVY = "#2B2B28"    # the ink
PAPER = "#F4F3EE"

# KitNote's logo, as drawn: on its light tile (the application icon), and the mark alone, without the tile.
LOGO = (ROOT / "share/icons/KitNoteLogo.svg").read_text(encoding="utf-8")
MARK_SVG = (ROOT / "share/icons/KitNoteMark.svg").read_text(encoding="utf-8")


def inner(svg: str) -> str:
    """What an SVG draws, without its <svg> element: to be placed in another drawing."""
    return svg[svg.index(">", svg.index("<svg")) + 1:svg.rindex("</svg>")]


def placed(svg: str, x: float, y: float, scale: float) -> str:
    return f'<g transform="translate({x} {y}) scale({scale})">{inner(svg)}</g>'


# The icon of a score file: a sheet of paper carrying the mark (drawn on 128 x 128).
DOCUMENT = f"""<svg width="392" height="392" viewBox="0 0 392 392" fill="none" xmlns="http://www.w3.org/2000/svg">
  <path d="M84 24h160l64 64v256a24 24 0 0 1-24 24H84a24 24 0 0 1-24-24V48a24 24 0 0 1 24-24z" fill="{PAPER}"/>
  <path d="M244 24l64 64h-48a16 16 0 0 1-16-16z" fill="#D8D5CB"/>
  {placed(MARK_SVG, 62, 98, 1.9)}
</svg>
"""


def loading_screen() -> str:
    """Paper, the mark, and one measure of a rock beat as a step grid."""
    rows = (("#E3B95B", range(0, 16, 2)), ("#8DB3A4", (4, 12)), ("#D98B6A", (0, 7, 8)))    # hi-hat, snare, bass drum
    cells = ""
    for row, (colour, steps) in enumerate(rows):
        for step in range(16):
            x = 48 + step * 26 + (step // 4) * 8
            fill = f'fill="{colour}"' if step in steps else f'fill="{NAVY}" opacity="0.08"'
            cells += f'  <rect x="{x}" y="{262 + row * 26}" width="20" height="20" rx="5" {fill}/>' + chr(10)
    return f"""<svg width="800" height="380" viewBox="0 0 800 380" fill="none" xmlns="http://www.w3.org/2000/svg">
  <rect width="800" height="380" rx="16" fill="{PAPER}"/>
  <rect x="0.5" y="0.5" width="799" height="379" rx="15.5" stroke="#D8D5CB"/>
  {placed(MARK_SVG, 30, 30, 1.25)}
  <text x="198" y="136" fill="{NAVY}" font-family="Pretendard" font-size="62" font-weight="700">KitNote</text>
  <text x="200" y="172" fill="{NAVY}" opacity="0.6" font-family="Pretendard" font-size="19">Drum notation</text>
{cells}</svg>
"""

def banner() -> str:
    """The picture at the top of the README (and the repository's social preview, 1280 x 640): the logo, the
    name, and two measures of a rock beat as a step grid."""
    rows = (("HH", "#E3B95B", set(range(0, 32, 2))), ("SD", "#8DB3A4", {4, 12, 20, 28, 30}),
            ("BD", "#D98B6A", {0, 7, 8, 16, 23, 24, 26}))
    cells = ""
    for row, (name, colour, steps) in enumerate(rows):
        y = 452 + row * 40
        cells += (f'  <text x="110" y="{y + 21}" fill="{NAVY}" opacity="0.55" font-family="Pretendard" font-size="18" font-weight="700">{name}</text>' + chr(10))
        for step in range(32):
            x = 168 + step * 30 + (step // 4) * 6
            fill = f'fill="{colour}"' if step in steps else f'fill="{NAVY}" opacity="0.07"'
            cells += f'  <rect x="{x}" y="{y}" width="24" height="28" rx="6" {fill}/>' + chr(10)
    return f"""<svg width="1280" height="640" viewBox="0 0 1280 640" fill="none" xmlns="http://www.w3.org/2000/svg">
  <rect width="1280" height="640" fill="{PAPER}"/>
  <rect x="114" y="110" width="281.6" height="281.6" rx="52.8" fill="{NAVY}" opacity="0.06"/>
  <rect x="110" y="100" width="281.6" height="281.6" rx="52.8" fill="#FFFFFF"/>
  {placed(LOGO, 110, 100, 2.2)}
  <rect x="110.5" y="100.5" width="280.6" height="280.6" rx="52.3" stroke="#D8D5CB"/>
  <text x="440" y="226" fill="{NAVY}" font-family="Pretendard" font-size="112" font-weight="700">KitNote</text>
  <text x="444" y="290" fill="{NAVY}" opacity="0.7" font-family="Pretendard" font-size="34">Drum notation editor, built on MuseScore Studio</text>
  <text x="444" y="338" fill="{NAVY}" opacity="0.5" font-family="Pretendard" font-size="28">드럼 악보 전용 편집기</text>
{cells}</svg>
"""


def dmg_background() -> str:
    """The window of the macOS disk image (1178 x 794, drawn at twice the size of the 589 x 397 window): the
    mark and the name, how to install, and the box where the app and the Applications folder sit, with an arrow."""
    return f"""<svg width="1178" height="794" viewBox="0 0 1178 794" fill="none" xmlns="http://www.w3.org/2000/svg">
  <rect width="1178" height="794" fill="#FFFFFF"/>
  {placed(MARK_SVG, 400, 54, 1.0)}
  <text x="540" y="148" fill="{NAVY}" font-family="Pretendard" font-size="72" font-weight="700">KitNote</text>
  <text x="589" y="236" fill="{NAVY}" font-family="Pretendard" font-size="25" text-anchor="middle">To install, drag the KitNote icon onto the Applications folder</text>
  <text x="589" y="276" fill="{NAVY}" opacity="0.6" font-family="Pretendard" font-size="23" text-anchor="middle">KitNote 아이콘을 Applications 폴더로 끌어다 놓으면 설치됩니다</text>
  <rect x="105" y="325" width="968" height="375" rx="28" fill="#F3F3F3" stroke="#B4B4B4" stroke-width="2"/>
  <path d="M518 488H592V466L660 512L592 558V536H518Z" fill="#B8BEC6"/>
</svg>
"""


def render_size(svg: str, width: int, height: int) -> Image.Image:
    renderer = QSvgRenderer(QByteArray(svg.encode("utf-8")))
    img = QImage(width, height, QImage.Format_RGBA8888)
    img.fill(Qt.transparent)
    painter = QPainter(img)
    painter.setRenderHint(QPainter.Antialiasing)
    painter.setRenderHint(QPainter.TextAntialiasing)
    renderer.render(painter)
    painter.end()
    return Image.frombytes("RGBA", (width, height), bytes(img.constBits())[: width * height * 4])


def render(svg: str, size: int) -> Image.Image:
    renderer = QSvgRenderer(QByteArray(svg.encode("utf-8")))
    img = QImage(size, size, QImage.Format_RGBA8888)
    img.fill(Qt.transparent)
    painter = QPainter(img)
    painter.setRenderHint(QPainter.Antialiasing)
    renderer.render(painter)
    painter.end()
    return Image.frombytes("RGBA", (size, size), bytes(img.constBits())[: size * size * 4])


def main() -> None:
    app = QGuiApplication(sys.argv)  # noqa: F841 - Qt needs an application object to render text
    for face in ("Regular", "Bold"):
        QFontDatabase.addApplicationFont(str(ROOT / f"src/framework/ui/data/pretendard/Pretendard-{face}.otf"))
    (ROOT / "src/appshell/qml/KitNote/AppShell/resources/mu_logo.svg").write_text(LOGO, encoding="utf-8")
    (ROOT / "src/appshell/resources/LoadingScreen.svg").write_text(loading_screen(), encoding="utf-8")

    icons = ROOT / "share/icons/AppIcon"
    sizes = (16, 24, 32, 48, 64, 96, 128, 256, 512, 1024, 2048)
    images = {s: render(LOGO, s) for s in sizes}
    for s, im in images.items():
        im.save(icons / f"kitnote_AppIcon_{s}x{s}.png")
    images[256].save(icons / "kitnote_AppIcon.ico", sizes=[(s, s) for s in (16, 24, 32, 48, 64, 96, 128, 256)])
    images[1024].convert("RGB").save(ROOT / "share/icons/kitnote_logo_full.png")
    images[1024].save(icons / "kitnote_AppIcon.icns")
    # Icon Composer lays a layer out at its own size on a 1024 x 1024 canvas, so the mark (drawn on 128 x 128) is
    # scaled up to it, in the proportions of the tile above; at 128 x 128 it is a speck in the middle of the icon.
    (icons / "AppIcon.icon/Assets/1x1 Colour.svg").write_text(
        f'<svg width="1024" height="1024" viewBox="0 0 1024 1024" fill="none" xmlns="http://www.w3.org/2000/svg">'
        f"\n{placed(MARK_SVG, 0, 0, 8)}\n</svg>\n", encoding="utf-8")

    # File-type icons (the Windows resource file references both).
    for folder, stem in (("MsczIcon", "kitnote_MsczIcon"), ("MscxIcon", "kitnote_MscxIcon")):
        target = ROOT / "share/icons" / folder
        (target / f"{stem}.svg").write_text(DOCUMENT, encoding="utf-8")
        render(DOCUMENT, 512).save(target / f"{stem}_512x512.png")
        render(DOCUMENT, 256).save(target / f"{stem}.ico", sizes=[(n, n) for n in (16, 24, 32, 48, 64, 128, 256)])
    (ROOT / "docs/kitnote/banner.svg").write_text(banner(), encoding="utf-8")
    render_size(banner(), 1280, 640).convert("RGB").save(ROOT / "docs/kitnote/banner.png", optimize=True)
    render_size(dmg_background(), 1178, 794).convert("RGB").save(
        ROOT / "buildscripts/packaging/macOS/kitnote-dmg-background.tiff", dpi=(144, 144))
    print("wrote", len(sizes), "icon sizes, the file-type icons, the logo, the loading screen, the README banner and the macOS disk image background")


if __name__ == "__main__":
    main()
