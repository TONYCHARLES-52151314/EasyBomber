from pathlib import Path
from tempfile import NamedTemporaryFile
from zipfile import ZIP_DEFLATED, ZipFile

from lxml import etree


DOCX = Path(r"C:\Data\Projects\面向对象程序设计\EasyBomber\学号_姓名_EasyBomber.docx")
W = "http://schemas.openxmlformats.org/wordprocessingml/2006/main"
NS = {"w": W}
Q = lambda name: f"{{{W}}}{name}"


def child(parent, name):
    node = parent.find(f"w:{name}", NS)
    if node is None:
        node = etree.SubElement(parent, Q(name))
    return node


def set_val(node, value):
    node.set(Q("val"), str(value))


def paragraph_text(p):
    return "".join(p.xpath(".//w:t/text()", namespaces=NS)).strip()


def format_runs(p, *, size_half_points, bold=False, font_east_asia="SimSun"):
    for run in p.xpath(".//w:r", namespaces=NS):
        rpr = run.find("w:rPr", NS)
        if rpr is None:
            rpr = etree.Element(Q("rPr"))
            run.insert(0, rpr)
        fonts = child(rpr, "rFonts")
        fonts.set(Q("eastAsia"), font_east_asia)
        fonts.set(Q("ascii"), "Times New Roman")
        fonts.set(Q("hAnsi"), "Times New Roman")
        set_val(child(rpr, "sz"), size_half_points)
        set_val(child(rpr, "szCs"), size_half_points)
        set_val(child(rpr, "color"), "000000")
        set_val(child(rpr, "u"), "none")
        b = rpr.find("w:b", NS)
        if bold:
            if b is None:
                b = etree.SubElement(rpr, Q("b"))
            set_val(b, "1")
        elif b is not None:
            rpr.remove(b)


def format_title(p, *, english=False):
    ppr = p.find("w:pPr", NS)
    if ppr is None:
        ppr = etree.Element(Q("pPr"))
        p.insert(0, ppr)
    set_val(child(ppr, "jc"), "center")
    spacing = child(ppr, "spacing")
    spacing.set(Q("before"), "0")
    spacing.set(Q("after"), "100" if not english else "220")
    spacing.set(Q("line"), "240")
    spacing.set(Q("lineRule"), "auto")
    format_runs(p, size_half_points=18 if english else 36,
                bold=not english, font_east_asia="SimHei" if not english else "SimSun")


def format_entry(p):
    text = paragraph_text(p)
    if not text:
        return
    number = text.split()[0]
    level = 2 if "." in number else 1
    ppr = p.find("w:pPr", NS)
    if ppr is None:
        ppr = etree.Element(Q("pPr"))
        p.insert(0, ppr)

    ind = child(ppr, "ind")
    ind.set(Q("left"), "420" if level == 2 else "0")
    ind.set(Q("right"), "80")
    spacing = child(ppr, "spacing")
    spacing.set(Q("before"), "0")
    spacing.set(Q("after"), "0" if level == 2 else "35")
    spacing.set(Q("line"), "220" if level == 2 else "240")
    spacing.set(Q("lineRule"), "auto")

    tabs = ppr.find("w:tabs", NS)
    if tabs is not None:
        ppr.remove(tabs)
    tabs = etree.SubElement(ppr, Q("tabs"))
    tab = etree.SubElement(tabs, Q("tab"))
    tab.set(Q("val"), "right")
    tab.set(Q("leader"), "dot")
    tab.set(Q("pos"), "9180")

    format_runs(p, size_half_points=19 if level == 2 else 21,
                bold=level == 1, font_east_asia="SimSun")


def main():
    with ZipFile(DOCX, "r") as src:
        entries = {item.filename: src.read(item.filename) for item in src.infolist()}

    root = etree.fromstring(entries["word/document.xml"])
    paragraphs = root.xpath("//w:body/w:p", namespaces=NS)
    toc_title = next(p for p in paragraphs if paragraph_text(p).replace(" ", "") == "目录")
    title_index = paragraphs.index(toc_title)
    contents = paragraphs[title_index + 1]
    format_title(toc_title, english=False)
    if paragraph_text(contents).upper() == "CONTENTS":
        format_title(contents, english=True)

    entry_count = 0
    for p in paragraphs[title_index + 2:]:
        text = paragraph_text(p)
        if text.startswith("1  项目概述") and p.find("w:pPr/w:pStyle[@w:val='Heading1']", NS) is not None:
            break
        if "\t" in text or p.find(".//w:tab", NS) is not None:
            format_entry(p)
            entry_count += 1

    if entry_count < 6:
        raise RuntimeError(f"Only found {entry_count} TOC entries; refusing to write")

    entries["word/document.xml"] = etree.tostring(
        root, xml_declaration=True, encoding="UTF-8", standalone="yes"
    )

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
    print(f"Restyled {entry_count} TOC entries in {DOCX}")


if __name__ == "__main__":
    main()
