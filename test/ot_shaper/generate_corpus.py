#!/usr/bin/env python3
"""Generate syllable test strings for compare_harfbuzz.py.

For each script: every assigned character alone, every consonant with every
dependent sign, every two-consonant conjunct (with and without a pre-base
vowel sign), reph forms, joiner variants, three-consonant clusters, and a
seeded random sample of arbitrary character sequences (broken clusters,
stray marks, repeated signs).

    python3 test/ot_shaper/generate_corpus.py <out dir>
"""

import random
import sys
import unicodedata
from pathlib import Path

SCRIPTS = ["devanagari", "bengali", "gurmukhi", "gujarati", "oriya", "tamil", "telugu", "kannada", "malayalam",
           "sinhala"]
ZWJ, ZWNJ = "‍", "‌"
VIRAMA = {"sinhala": "්"}


def block(i):
    first = 0x0900 + 0x80 * i
    return [chr(c) for c in range(first, first + 0x80) if unicodedata.category(chr(c)) != "Cn"]


def main():
    out = Path(sys.argv[1])
    out.mkdir(parents=True, exist_ok=True)
    rng = random.Random(1757)
    for i, name in enumerate(SCRIPTS):
        chars = block(i)
        consonants = [c for c in chars if unicodedata.category(c) == "Lo" and "LETTER" in unicodedata.name(c, "")
                      and not any(v in unicodedata.name(c) for v in (" A", " AA", " I", " II", " U", " UU", " E",
                                                                      " AI", " O", " AU", " VOCALIC"))]
        vowels = [c for c in chars if unicodedata.category(c) == "Lo" and c not in consonants]
        signs = [c for c in chars if unicodedata.category(c) in ("Mn", "Mc")]
        virama = VIRAMA.get(name) or next(c for c in signs if "VIRAMA" in unicodedata.name(c) or
                                          "SIGN HALANTA" in unicodedata.name(c) or "PULLI" in unicodedata.name(c))
        prebase = [c for c in signs if "VOWEL SIGN" in unicodedata.name(c)]
        ra = next((c for c in consonants if unicodedata.name(c).endswith("LETTER RA")), consonants[0])
        common = consonants[:12]

        lines = set(chars)
        lines.update(c + s for c in consonants for s in signs)
        lines.update(v + s for v in vowels for s in signs)
        lines.update(a + virama + b for a in consonants for b in consonants)
        lines.update(a + virama + b + s for a in common for b in consonants for s in prebase[:6])
        lines.update(ra + virama + c + s for c in consonants for s in [""] + signs)
        lines.update(ra + virama + ZWJ + c for c in consonants)
        lines.update(a + virama + ZWJ + b for a in common for b in consonants)
        lines.update(a + virama + ZWNJ + b for a in common for b in consonants)
        lines.update(a + ZWJ + virama + b for a in common for b in consonants)
        lines.update(a + virama + b + virama + c for a in common for b in common for c in consonants[:20])
        lines.update(ra + virama + a + virama + b + s for a in common for b in common for s in prebase[:3])
        lines.update(c + virama for c in consonants)
        lines.update(c + virama + ZWJ for c in consonants)
        lines.update(c + virama + ZWNJ for c in consonants)
        pool = chars + [ZWJ, ZWNJ, "◌", "।"]
        for _ in range(4000):
            lines.add("".join(rng.choice(pool) for _ in range(rng.randint(2, 7))))
        (out / f"{name}.txt").write_text("\n".join(sorted(lines)) + "\n", encoding="utf-8")
        print(f"{name}: {len(lines)} strings")
    myanmar(out, rng)


def myanmar(out, rng):
    """Myanmar: consonants with every sign, medial combinations, kinzi,
    stacked consonants (U+1039), pre-base vowel and medial ra reordering, and
    random sequences."""
    chars = [chr(c) for c in range(0x1000, 0x10A0) if unicodedata.category(chr(c)) != "Cn"]
    consonants = [chr(c) for c in range(0x1000, 0x1021)]
    signs = [c for c in chars if unicodedata.category(c) in ("Mn", "Mc")]
    medials = ["\u103B", "\u103C", "\u103D", "\u103E", "\u103B\u103D", "\u103B\u103E", "\u103C\u103D",
               "\u103C\u103E", "\u103D\u103E", "\u103B\u103D\u103E", "\u103C\u103D\u103E"]
    vowels = ["", "\u102B", "\u102C", "\u102D", "\u102E", "\u102F", "\u1030", "\u1031", "\u1032", "\u1031\u102C",
              "\u1031\u102C\u103A", "\u102D\u102F", "\u1036", "\u102F\u1036"]
    tones = ["", "\u1037", "\u1038", "\u103A", "\u1037\u103A", "\u103A\u1037"]
    kinzi = "\u1004\u103A\u1039"
    common = consonants[:12]
    lines = set(chars)
    lines.update(c + s for c in consonants for s in signs)
    lines.update(c + m + v for c in consonants for m in medials for v in vowels)
    lines.update(c + m + v + t for c in common for m in medials[:4] for v in vowels for t in tones)
    lines.update(a + "\u1039" + b for a in consonants for b in consonants)
    lines.update(a + "\u1039" + b + v for a in common for b in consonants for v in vowels[1:])
    lines.update(kinzi + c + v for c in consonants for v in vowels)
    lines.update(a + kinzi + b + v for a in common for b in common for v in vowels[:6])
    lines.update(c + "\u103A" for c in consonants)
    lines.update(a + b + "\u103A" + t for a in common for b in consonants for t in tones)
    lines.update(c + v + "\uFE00" for c in common for v in vowels[1:])
    pool = chars + [ZWJ, ZWNJ, "\u25CC"]
    for _ in range(4000):
        lines.add("".join(rng.choice(pool) for _ in range(rng.randint(2, 7))))
    (out / "myanmar.txt").write_text("\n".join(sorted(lines)) + "\n", encoding="utf-8")
    print(f"myanmar: {len(lines)} strings")


if __name__ == "__main__":
    main()
