from copy import deepcopy
from pathlib import Path
from tempfile import NamedTemporaryFile
from zipfile import ZIP_DEFLATED, ZipFile

from lxml import etree


DOCX = Path(r"C:\Data\Projects\面向对象程序设计\EasyBomber\学号_姓名_EasyBomber.docx")
W = "http://schemas.openxmlformats.org/wordprocessingml/2006/main"
NS = {"w": W}


def cell_text(cell):
    return "".join(cell.xpath(".//w:t/text()", namespaces=NS))


def replace_cell_text(cell, text):
    texts = cell.xpath(".//w:t", namespaces=NS)
    if not texts:
        raise RuntimeError("Cover cell has no text node")
    texts[0].text = text
    for extra in texts[1:]:
        extra.text = ""


def main():
    with ZipFile(DOCX, "r") as src:
        entries = {item.filename: src.read(item.filename) for item in src.infolist()}

    root = etree.fromstring(entries["word/document.xml"])
    first_table = root.xpath("//w:body/w:tbl[1]", namespaces=NS)[0]
    rows = first_table.xpath("./w:tr", namespaces=NS)
    if any("GitHub" in cell_text(row) for row in rows):
        print("GitHub row already exists; no change made")
        return

    source_row = rows[-1]
    new_row = deepcopy(source_row)
    cells = new_row.xpath("./w:tc", namespaces=NS)
    if len(cells) != 2:
        raise RuntimeError("Expected a two-column cover information table")
    replace_cell_text(cells[0], "GitHub 项目网址")
    replace_cell_text(cells[1], "________________________________")
    first_table.append(new_row)

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
    print("Added GitHub cover row")


if __name__ == "__main__":
    main()
