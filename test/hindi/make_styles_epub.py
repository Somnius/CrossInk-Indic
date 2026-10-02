#!/usr/bin/env python3
"""Build books/hindi-styles.epub: Hindi in regular, bold, italic, bold italic,
superscript and subscript, and mixed with English italic."""
import os
import zipfile

STYLES = '''<h2>शैली परीक्षण</h2>
<p>सामान्य: क्षत्रिय धर्म कार्य स्थिति हिन्दी।</p>
<p><b>बोल्ड: क्षत्रिय धर्म कार्य स्थिति हिन्दी।</b></p>
<p><i>इटैलिक: क्षत्रिय धर्म कार्य स्थिति हिन्दी।</i></p>
<p><b><i>बोल्ड इटैलिक: क्षत्रिय धर्म कार्य स्थिति हिन्दी।</i></b></p>
<p>सुपरस्क्रिप्ट: अध्याय<sup>क्ष१</sup> और सबस्क्रिप्ट: जल<sub>द्व२</sub> समाप्त।</p>
<p>मिश्रित: <i>English italic</i> और <b>हिन्दी बोल्ड</b> एक ही पंक्ति में, <i>हिन्दी इटैलिक</i> भी।</p>'''


def xhtml(body, title):
    return ('<?xml version="1.0" encoding="utf-8"?><html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hi">'
            f'<head><title>{title}</title></head><body>{body}</body></html>')


OPF = ('<?xml version="1.0" encoding="utf-8"?><package xmlns="http://www.idpf.org/2007/opf" version="3.0" '
       'unique-identifier="id"><metadata xmlns:dc="http://purl.org/dc/elements/1.1/"><dc:identifier id="id">'
       'hindi-styles</dc:identifier><dc:title>शैली परीक्षण</dc:title><dc:creator>CrossInk</dc:creator>'
       '<dc:language>hi</dc:language><meta property="dcterms:modified">2026-10-02T00:00:00Z</meta></metadata>'
       '<manifest><item id="nav" href="nav.xhtml" media-type="application/xhtml+xml" properties="nav"/>'
       '<item id="c1" href="c1.xhtml" media-type="application/xhtml+xml"/></manifest>'
       '<spine><itemref idref="c1"/></spine></package>')
NAV = ('<?xml version="1.0" encoding="utf-8"?><html xmlns="http://www.w3.org/1999/xhtml" '
       'xmlns:epub="http://www.idpf.org/2007/ops"><head><title>सूची</title></head><body><nav epub:type="toc">'
       '<ol><li><a href="c1.xhtml">शैली परीक्षण</a></li></ol></nav></body></html>')

out = os.path.join(os.path.dirname(os.path.abspath(__file__)), "books", "hindi-styles.epub")
with zipfile.ZipFile(out, "w") as z:
    z.writestr(zipfile.ZipInfo("mimetype"), "application/epub+zip")
    z.writestr("META-INF/container.xml",
               '<?xml version="1.0"?><container version="1.0" xmlns="urn:oasis:names:tc:opendocument:xmlns:container">'
               '<rootfiles><rootfile full-path="OEBPS/content.opf" media-type="application/oebps-package+xml"/>'
               '</rootfiles></container>')
    z.writestr("OEBPS/content.opf", OPF)
    z.writestr("OEBPS/nav.xhtml", NAV)
    z.writestr("OEBPS/c1.xhtml", xhtml(STYLES, "शैली परीक्षण"))
print("wrote", out)
