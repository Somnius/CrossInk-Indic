#!/usr/bin/env python3
"""Build a small Hindi test EPUB: make_epub.py out.epub [lang] [story.txt]"""
import sys, zipfile, html
out = sys.argv[1]; lang = sys.argv[2] if len(sys.argv) > 2 else 'hi'
story = open(sys.argv[3], encoding='utf-8').read().strip() if len(sys.argv) > 3 else None
torture = """<h1>हिन्दी आकार परीक्षण</h1>
<p><b>संयुक्ताक्षर:</b> क्षत्रिय, त्रिकोण, ज्ञान, श्रम, द्वार, शुद्ध, ब्रह्म, अङ्क, विद्या, पद्म, स्त्री।</p>
<p><b>रेफ:</b> धर्म, कार्य, पूर्व, सूर्य, वर्ष, आशीर्वाद, निर्णय।</p>
<p><b>इ की मात्रा:</b> किताब, कि, शक्ति, स्थिति, प्रतिदिन, हिन्दी।</p>
<p><b>नुक़्ता:</b> क़लम, ज़िन्दगी, फ़िल्म, ग़ज़ल, ख़ुशी, ड़, ढ़।</p>
<p><b>अनुस्वार और चन्द्रबिन्दु:</b> हँसना, संगीत, अंग्रेज़ी, चाँद, गाँव, मैं, हैं।</p>
<p><b>अंक और विराम:</b> ०१२३४५६७८९ — यह वाक्य है। यह दूसरा है॥</p>
<p><b>मिश्रित:</b> CrossInk पर हिन्दी पढ़ना अब संभव है, Xteink X4 Pro 2026।</p>"""
default_story = """<h2>परीक्षण अनुच्छेद</h2>
<p>हिन्दी भारत की एक प्रमुख भाषा है और देवनागरी लिपि में लिखी जाती है। इस अनुच्छेद का उद्देश्य यह जाँचना है कि लंबे वाक्यों में शब्दों के बीच पंक्ति-विभाजन सही होता है या नहीं, और संयुक्ताक्षर, मात्राएँ तथा अनुस्वार ठीक से दिखाई देते हैं या नहीं।</p>"""
body_story = default_story
if story:
    paras = [p.strip() for p in story.split('\n') if p.strip()]
    body_story = ''.join(f'<p>{html.escape(p)}</p>' for p in paras)
def x(body, title):
    la = f' xml:lang="{lang}" lang="{lang}"' if lang else ''
    return f'<?xml version="1.0" encoding="utf-8"?><html xmlns="http://www.w3.org/1999/xhtml"{la}><head><title>{title}</title><style>p{{text-align:justify}}</style></head><body>{body}</body></html>'
lang_el = f'<dc:language>{lang}</dc:language>' if lang else ''
opf = f'''<?xml version="1.0" encoding="utf-8"?><package xmlns="http://www.idpf.org/2007/opf" version="3.0" unique-identifier="id"><metadata xmlns:dc="http://purl.org/dc/elements/1.1/"><dc:identifier id="id">hindi-test-{lang or "none"}</dc:identifier><dc:title>हिन्दी परीक्षण पुस्तक</dc:title><dc:creator>मुंशी प्रेमचंद</dc:creator><dc:description>CrossInk हिन्दी परीक्षण: संयुक्ताक्षर, रेफ और मात्राएँ।</dc:description>{lang_el}<meta property="dcterms:modified">2026-10-01T00:00:00Z</meta></metadata><manifest><item id="nav" href="nav.xhtml" media-type="application/xhtml+xml" properties="nav"/><item id="c1" href="c1.xhtml" media-type="application/xhtml+xml"/><item id="c2" href="c2.xhtml" media-type="application/xhtml+xml"/></manifest><spine><itemref idref="c1"/><itemref idref="c2"/></spine></package>'''
nav = '<?xml version="1.0" encoding="utf-8"?><html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops"><head><title>सूची</title></head><body><nav epub:type="toc"><ol><li><a href="c1.xhtml">आकार परीक्षण</a></li><li><a href="c2.xhtml">कहानी</a></li></ol></nav></body></html>'
with zipfile.ZipFile(out, 'w') as z:
    z.writestr('mimetype', 'application/epub+zip', compress_type=zipfile.ZIP_STORED)
    z.writestr('META-INF/container.xml', '<?xml version="1.0"?><container version="1.0" xmlns="urn:oasis:names:tc:opendocument:xmlns:container"><rootfiles><rootfile full-path="OEBPS/content.opf" media-type="application/oebps-package+xml"/></rootfiles></container>')
    z.writestr('OEBPS/content.opf', opf); z.writestr('OEBPS/nav.xhtml', nav)
    z.writestr('OEBPS/c1.xhtml', x(torture, 'आकार परीक्षण')); z.writestr('OEBPS/c2.xhtml', x(body_story, 'कहानी'))
print('wrote', out)
