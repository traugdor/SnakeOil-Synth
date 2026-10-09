"""Builds the SnakeOil Synth user manual (HTML -> PDF).

Usage (from the repo root):  .venv/Scripts/python.exe docs/manual/build_manual.py

The content lives in manual_content.py. This script numbers the sections, builds the
table of contents and the index, renders the HTML to a PDF with headless Microsoft
Edge, reads the page of every heading back out of the PDF and renders a second time so
the contents and index carry real page numbers.
"""
import html
import json
import re
import subprocess
import sys
import zlib
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
EDGE_CANDIDATES = (
    r"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe",
    r"C:\Program Files\Microsoft\Edge\Application\msedge.exe",
)

# Screenshot geometry (assets/standalone-ui.png is 1470 x 808).
IMG_W, IMG_H = 1470, 808
BOXES = {  # name: (x1, y1, x2, y2) in screenshot pixels
    "Oscillator 1": (8, 62, 214, 287), "Oscillator 2": (220, 62, 553, 287),
    "Modulation": (560, 62, 830, 287), "Master": (837, 62, 1091, 287),
    "Tempo": (1098, 62, 1226, 287), "Noise": (1233, 62, 1452, 287),
    "Filter": (8, 292, 214, 624), "Filter Env": (220, 292, 553, 624),
    "Amp Envelope": (560, 292, 830, 624), "LFO": (837, 292, 1091, 624),
    "Mod Matrix": (1098, 292, 1452, 624), "Effects": (8, 630, 830, 800),
    "Unison": (837, 630, 1091, 800), "Glide": (1098, 630, 1452, 800),
}
BOX_ORDER = list(BOXES)


class Manual:
    def __init__(self):
        self.parts = []
        self.sections = []      # (id, number, title, level)
        self.counters = [0, 0, 0]
        self.index = {}         # term -> set(section ids)
        self.pending_index = []  # (term, section id)

    # ---- structure ----------------------------------------------------
    def h(self, level, sid, title, new_page=None):
        if level == 1:
            self.counters = [self.counters[0] + 1, 0, 0]
        elif level == 2:
            self.counters[1] += 1
            self.counters[2] = 0
        else:
            self.counters[2] += 1
        n = ".".join(str(c) for c in self.counters[:level])
        self.sections.append((sid, n, title, level))
        cls = " newpage" if (level == 1 if new_page is None else new_page) else ""
        self.parts.append(
            '<h%d id="%s" class="sec%s"><span class="num">%s</span> %s</h%d>'
            % (level + 0, sid, cls, n, html.escape(title), level))
        self._current = sid

    def raw(self, text):
        self.parts.append(text)

    def p(self, text):
        self.parts.append("<p>%s</p>" % text)

    def ul(self, items):
        self.parts.append("<ul>%s</ul>" % "".join("<li>%s</li>" % i for i in items))

    def ol(self, items):
        self.parts.append("<ol>%s</ol>" % "".join("<li>%s</li>" % i for i in items))

    def note(self, kind, text):
        label = {"note": "Note", "tip": "Tip", "warn": "Caution"}[kind]
        self.parts.append('<div class="callout %s"><b>%s.</b> %s</div>' % (kind, label, text))

    def table(self, header, rows, cls="", widths=None):
        out = ['<table class="%s">' % cls]
        if widths:
            out.append("<colgroup>%s</colgroup>" % "".join('<col style="width:%s">' % w for w in widths))
        out.append("<thead><tr>%s</tr></thead><tbody>" % "".join("<th>%s</th>" % c for c in header))
        for row in rows:
            out.append("<tr>%s</tr>" % "".join("<td>%s</td>" % c for c in row))
        out.append("</tbody></table>")
        self.parts.append("".join(out))

    def controls(self, rows):
        """Control table: (name, range, default, description)."""
        self.table(("Control", "Range", "Default", "What it does"),
                   rows, cls="controls", widths=("17%", "17%", "11%", "55%"))

    def crop(self, box_name, caption=None):
        x1, y1, x2, y2 = BOXES[box_name]
        w, h = x2 - x1, y2 - y1
        s = min(1.0, 600.0 / w)
        self.parts.append(
            '<figure class="crop"><div class="cropimg" style="width:%.0fpx;height:%.0fpx;'
            'background-image:url(assets/standalone-ui.png);background-size:%.1fpx %.1fpx;'
            'background-position:-%.1fpx -%.1fpx"></div><figcaption>%s</figcaption></figure>'
            % (w * s, h * s, IMG_W * s, IMG_H * s, x1 * s, y1 * s,
               caption or "The %s box." % box_name))

    def tag(self, *terms):
        for term in terms:
            self.pending_index.append((term, self._current))

    # ---- output -------------------------------------------------------
    def ref(self, sid, numbers):
        return numbers.get(sid, "?")

    def render(self, pages=None):
        numbers = {sid: n for sid, n, _t, _l in self.sections}
        pages = pages or {}
        body = "".join(self.parts)
        body = re.sub(r"\[\[ref:([\w-]+)\]\]",
                      lambda m: '<a class="xref" href="#%s">%s</a>' % (m.group(1), numbers.get(m.group(1), "?")), body)
        toc = ['<h1 class="toc-title">Contents</h1><div class="toc">']
        for sid, n, title, level in self.sections:
            if level > 2:
                continue
            toc.append('<div class="toc%d"><a href="#%s"><span class="tn">%s</span><span class="tt">%s</span>'
                       '<span class="tp">%s</span></a></div>'
                       % (level, sid, n, html.escape(title), pages.get(sid, "")))
        toc.append("</div>")
        index = {}
        for term, sid in self.pending_index:
            index.setdefault(term, [])
            if sid not in index[term]:
                index[term].append(sid)
        idx = ['<h1 class="sec newpage" id="index">Index</h1><div class="index">']
        letter = ""
        for term in sorted(index, key=lambda t: t.lower()):
            first = term[0].upper()
            if first != letter:
                letter = first
                idx.append('<div class="letter">%s</div>' % letter)
            links = ", ".join(
                '<a href="#%s">%s</a>' % (sid, pages.get(sid, numbers.get(sid, "?"))) for sid in index[term])
            idx.append('<div class="ientry"><span>%s</span><span class="ipages">%s</span></div>' % (html.escape(term), links))
        idx.append("</div>")
        from manual_content import COVER, FRONT, CSS
        return ("<!doctype html><html lang=\"en\"><head><meta charset=\"utf-8\">"
                "<title>SnakeOil Synth User Manual</title><style>%s</style></head><body>%s%s%s%s%s</body></html>"
                % (CSS, COVER, FRONT, "".join(toc), body, "".join(idx)))


def find_edge():
    for c in EDGE_CANDIDATES:
        if Path(c).exists():
            return c
    sys.exit("Microsoft Edge not found; install it or set EDGE_CANDIDATES.")


def render_pdf(html_path, pdf_path):
    subprocess.run([find_edge(), "--headless=new", "--disable-gpu", "--no-pdf-header-footer",
                    "--print-to-pdf=%s" % pdf_path, html_path.as_uri()],
                   check=True, capture_output=True, timeout=300)


def pdf_page_texts(pdf_path):
    """Plain text of every page (Chromium PDFs: CID glyph id = character code - 29)."""
    data = Path(pdf_path).read_bytes()
    pages = []
    for m in re.finditer(rb"stream\r?\n(.*?)\r?\nendstream", data, re.S):
        try:
            s = zlib.decompress(m.group(1))
        except zlib.error:
            continue
        if b"BT" in s and b"Tj" in s:
            txt = ""
            for h in re.findall(rb"<([0-9A-F]+)>\s*Tj", s):
                txt += "".join(chr(int(h[i:i + 4], 16) + 29) for i in range(0, len(h), 4))
            pages.append(re.sub(r"\s+", "", txt))
    return pages


def main():
    sys.path.insert(0, str(HERE))
    import manual_content
    m = Manual()
    manual_content.build(m)
    html_path = HERE / "manual.html"
    pdf_path = HERE / "SnakeOil-Synth-User-Manual.pdf"
    html_path.write_text(m.render(), encoding="utf-8")
    render_pdf(html_path, pdf_path)
    texts = pdf_page_texts(pdf_path)
    pages = {}
    # the body starts on the page of the first chapter heading that is not in the contents
    first = m.sections[0]
    first_key = re.sub(r"\s+", "", "%s %s" % (first[1], first[2]))
    toc_pages = [i + 1 for i, t in enumerate(texts) if first_key in t]
    body_start = toc_pages[-1] if len(toc_pages) > 1 else toc_pages[0]
    for sid, n, title, level in m.sections:
        key = re.sub(r"\s+", "", "%s %s" % (n, title))
        hits = [i + 1 for i, t in enumerate(texts) if key in t and i + 1 >= body_start]
        if hits:
            pages[sid] = hits[0]
    missing = [s for s, *_ in m.sections if s not in pages]
    html_path.write_text(m.render(pages), encoding="utf-8")
    render_pdf(html_path, pdf_path)
    texts2 = pdf_page_texts(pdf_path)
    print("pages: %d (first pass %d); headings located: %d/%d; missing: %s"
          % (len(texts2), len(texts), len(pages), len(m.sections), missing))
    json.dump(pages, open(HERE / "pages.json", "w"))


if __name__ == "__main__":
    main()
