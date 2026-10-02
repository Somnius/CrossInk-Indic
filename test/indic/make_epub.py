#!/usr/bin/env python3
"""Build an Indic test EPUB from test/indic/texts:

    make_epub.py <out.epub> <lang> <texts-dir>/<language>

Reads <language>-torture.txt (one "label: words" per line) and
<language>-story.txt (first line "title — author", paragraphs after). Chapter 1
is the torture lines, chapter 2 the same words in bold, italic, a heading, a
superscript and mixed with English, chapter 3 the story.
"""
import html
import sys
import zipfile

out, lang, base = sys.argv[1:4]
torture = [l.strip() for l in open(base + "-torture.txt", encoding="utf-8") if l.strip()]
story_lines = open(base + "-story.txt", encoding="utf-8").read().strip().split("\n")
heading = story_lines[0].strip()
title, _, author = heading.rpartition(" — ")
paragraphs = [p.strip() for p in story_lines[1:] if p.strip()]


def esc(s):
    return html.escape(s, quote=False)


def labelled(line):
    label, sep, rest = line.partition(":")
    return f"<p><b>{esc(label)}{sep}</b>{esc(rest)}</p>" if sep else f"<p>{esc(line)}</p>"


first_words = torture[0].partition(":")[2].strip() or torture[0]
c1 = f"<h1>{esc(title)}</h1>" + "".join(labelled(l) for l in torture)
c2 = (f"<h2>{esc(first_words)}</h2>"
      f"<p><b>{esc(first_words)}</b></p><p><i>{esc(first_words)}</i></p>"
      f"<p><b><i>{esc(first_words)}</i></b></p>"
      f"<p>{esc(paragraphs[0][:80])}<sup>1</sup> CrossInk, Xteink X4 Pro, 2026 — {esc(first_words)}.</p>"
      f"<p style=\"text-align:center\">* * *</p><p>{esc(torture[-1].partition(':')[2].strip())}</p>")
c3 = f"<h2>{esc(heading)}</h2>" + "".join(f"<p>{esc(p)}</p>" for p in paragraphs)


def xhtml(body, chapter):
    return (f'<?xml version="1.0" encoding="utf-8"?><html xmlns="http://www.w3.org/1999/xhtml" xml:lang="{lang}" '
            f'lang="{lang}"><head><title>{esc(chapter)}</title><style>p{{text-align:justify}}</style></head>'
            f"<body>{body}</body></html>")


opf = (f'<?xml version="1.0" encoding="utf-8"?><package xmlns="http://www.idpf.org/2007/opf" version="3.0" '
       f'unique-identifier="id"><metadata xmlns:dc="http://purl.org/dc/elements/1.1/">'
       f'<dc:identifier id="id">crossink-indic-test-{lang}</dc:identifier><dc:title>{esc(title)}</dc:title>'
       f"<dc:creator>{esc(author)}</dc:creator><dc:language>{lang}</dc:language>"
       f'<meta property="dcterms:modified">2026-10-02T00:00:00Z</meta></metadata><manifest>'
       f'<item id="nav" href="nav.xhtml" media-type="application/xhtml+xml" properties="nav"/>'
       + "".join(f'<item id="c{i}" href="c{i}.xhtml" media-type="application/xhtml+xml"/>' for i in (1, 2, 3))
       + '</manifest><spine><itemref idref="c1"/><itemref idref="c2"/><itemref idref="c3"/></spine></package>')
names = [torture[0].partition(":")[0], first_words, title]
nav = ('<?xml version="1.0" encoding="utf-8"?><html xmlns="http://www.w3.org/1999/xhtml" '
       'xmlns:epub="http://www.idpf.org/2007/ops"><head><title>toc</title></head><body><nav epub:type="toc"><ol>'
       + "".join(f'<li><a href="c{i}.xhtml">{esc(n)}</a></li>' for i, n in enumerate(names, 1))
       + "</ol></nav></body></html>")
with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
    z.writestr("mimetype", "application/epub+zip", compress_type=zipfile.ZIP_STORED)
    z.writestr("META-INF/container.xml",
               '<?xml version="1.0"?><container version="1.0" '
               'xmlns="urn:oasis:names:tc:opendocument:xmlns:container"><rootfiles><rootfile '
               'full-path="OEBPS/content.opf" media-type="application/oebps-package+xml"/></rootfiles></container>')
    z.writestr("OEBPS/content.opf", opf)
    z.writestr("OEBPS/nav.xhtml", nav)
    for i, (body, name) in enumerate(zip((c1, c2, c3), names), 1):
        z.writestr(f"OEBPS/c{i}.xhtml", xhtml(body, name))
print("wrote", out)
