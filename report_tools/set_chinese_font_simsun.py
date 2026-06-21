from pathlib import Path
from tempfile import NamedTemporaryFile
from zipfile import ZIP_DEFLATED, ZipFile

from lxml import etree


DOCX = Path(r"C:\Data\Projects\面向对象程序设计\EasyBomber\学号_姓名_EasyBomber.docx")
W = "http://schemas.openxmlformats.org/wordprocessingml/2006/main"
NS = {"w": W}
Q = lambda name: f"{{{W}}}{name}"


def ensure_rfonts(rpr):
    fonts = rpr.find("w:rFonts", NS)
    if fonts is None:
        fonts = etree.Element(Q("rFonts"))
        rpr.insert(0, fonts)
    fonts.set(Q("eastAsia"), "SimSun")
    fonts.attrib.pop(Q("eastAsiaTheme"), None)


def patch_xml(data):
    root = etree.fromstring(data)
    changed = 0

    # Direct text runs, including hyperlinks, tables, headers, footers, and text boxes.
    for run in root.xpath(".//w:r", namespaces=NS):
        rpr = run.find("w:rPr", NS)
        if rpr is None:
            rpr = etree.Element(Q("rPr"))
            run.insert(0, rpr)
        ensure_rfonts(rpr)
        changed += 1

    # Style/default run properties control text that has no direct font formatting.
    for rpr in root.xpath(".//w:style/w:rPr | .//w:docDefaults/w:rPrDefault/w:rPr", namespaces=NS):
        ensure_rfonts(rpr)
        changed += 1

    return etree.tostring(root, xml_declaration=True, encoding="UTF-8", standalone="yes"), changed


def main():
    with ZipFile(DOCX, "r") as src:
        entries = {item.filename: src.read(item.filename) for item in src.infolist()}

    eligible = [
        name for name in entries
        if name == "word/document.xml"
        or name == "word/styles.xml"
        or name.startswith("word/header") and name.endswith(".xml")
        or name.startswith("word/footer") and name.endswith(".xml")
        or name in {
            "word/footnotes.xml", "word/endnotes.xml", "word/comments.xml",
            "word/numbering.xml", "word/glossary/document.xml",
        }
    ]

    totals = {}
    for name in eligible:
        patched, count = patch_xml(entries[name])
        if count:
            entries[name] = patched
            totals[name] = count

    with NamedTemporaryFile(delete=False, suffix=".docx", dir=DOCX.parent) as tmp:
        temp_path = Path(tmp.name)
    try:
        with ZipFile(temp_path, "w", ZIP_DEFLATED) as dst:
            for name, data in entries.items():
                dst.writestr(name, data)
        temp_path.replace(DOCX)
    finally:
        if temp_path.exists():
            temp_path.unlink()

    print("Patched parts:")
    for name, count in totals.items():
        print(f"  {name}: {count} run/style properties")


if __name__ == "__main__":
    main()
