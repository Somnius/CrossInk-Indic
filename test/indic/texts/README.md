# Indic shaping test texts

These are test texts for checking Indic text shaping in an e-reader. Hindi uses Premchand's story from `test/hindi/texts` (`hindi-*.txt` here). Each language has three files:

- `<language>-story.txt`: running prose (verse for Nepali). The first line is `title — author`, then a blank line, then paragraphs separated by one blank line. The text is Unicode NFC, with wiki markup, page numbers and footnote markers stripped.
- `<language>-source.txt`: the exact source URL, work, author, death year, first publication, the edits made, and why the text is public domain.
- `<language>-torture.txt`: 8–10 short lines that stress the script's hard shaping cases. Each line starts with a label in that language followed by `:`.

All texts were taken from Wikisource through the MediaWiki API (`action=parse&prop=text`) and converted to plain text.

## Public-domain test

Each text had to meet two tests:

- **India:** the author died long enough ago for the life + 60 year term to have run out.
- **US:** the work was first published before 1929.

## Sources

| Language | Work | Author (death year) | Words | Public-domain status |
|---|---|---|---|---|
| Marathi | उलटीकडून सुरुवात (Ulatikadun Suruvat, 'Starting from the wrong end'), short moral story | Hari Narayan Apte (हरि नारायण आपटे) (1919) | 1179 | PD in India since 1980; published 1915 |
| Nepali | भानुभक्तीय रामायण (Bhanubhakta's Ramayana), बाल काण्ड, opening stanzas 1–27 | Bhanubhakta Acharya (भानुभक्त आचार्य) (1868) | 979 | Verse, not prose; died 1868, printed 1887 |
| Bengali | গিন্নি (Ginni), short story | Rabindranath Tagore (রবীন্দ্রনাথ ঠাকুর) (1941) | 997 | PD in India since 2002; published 1891 |
| Assamese | তেজীমলা (Tejimola), folk tale | Lakshminath Bezbaroa (লক্ষ্মীনাথ বেজবৰুৱা) (1938) | 1180 | PD in India since 1999; published 1911 |
| Punjabi | ਬਿਜੈ ਸਿੰਘ (Bijay Singh), novel | Bhai Vir Singh (ਭਾਈ ਵੀਰ ਸਿੰਘ) (1957) | 1069 | Died 1957, after the 1955 cutoff; PD in India since 2018; published 1899 |
| Gujarati | સરસ્વતીચંદ્ર (Saraswatichandra), novel, part 1 | Govardhanram Madhavram Tripathi (ગોવર્ધનરામ ત્રિપાઠી) (1907) | 1177 | PD in India since 1968; published 1887 |
| Odia | ରେବତୀ (Rebati), short story | Fakir Mohan Senapati (ଫକୀରମୋହନ ସେନାପତି) (1918) | 1068 | PD in India since 1979; published 1898 |
| Tamil | காக்காய்ப் பார்லிமெண்ட் (Kakkai Parliament, 'The Crows' Parliament'), short prose sketch | Subramania Bharati (சுப்பிரமணிய பாரதி) (1921) | 642 | PD in India since 1982, and nationalised in 1949; published before 1921 (exact date not verified) |
| Telugu | దిద్దుబాటు (Diddubatu), short story | Gurajada Apparao (గురజాడ అప్పారావు) (1915) | 701 | PD in India since 1976; published 1910 |
| Kannada | ಕೋಟಿ ಚೆನ್ನಯ (Koti Chennaya), a prose retelling of the Tulu twin-hero legend | Panje Mangesha Rao (ಪಂಜೆ ಮಂಗೇಶರಾಯ) (1937) | 1122 | PD in India since 1998; published 1924, PD in the US since 2020 |
| Malayalam | ഇന്ദുലേഖ (Indulekha), novel | O. Chandu Menon (ഒ. ചന്തുമേനോൻ) (1899) | 716 | PD in India since 1960; published 1889 |
| Sinhala | සද්ධර්මරත්නාවලිය (Saddharma Ratnavaliya), classical Buddhist prose | Dharmasena Thera (ධර්මසේන හිමි) (13th century (exact year unknown)) | 656 | 13th-century text; modern-edition source not identified; encoding hand-repaired |

Word counts are whitespace-separated tokens, not counting the title line.

## Exceptions: read before relying on these texts

- **Punjabi:** Bhai Vir Singh died in 1957, after the 1955 cutoff used for the other texts. His works are public domain in India (life + 60, since 2018) and in the US (published 1899).
- **Nepali:** this is verse, not prose. There is no ne.wikisource; the Nepali collection on multilingual wikisource.org is small. My searches found no public-domain pre-1929 Nepali prose there. Bhanubhakta's Ramayana is the nearest safe option.
- **Sinhala:** No text by Piyadasa Sirisena (d. 1946) or another modern public-domain Sinhala author was found on Wikisource or Project Gutenberg.
  - Every Sinhala work on wikisource.org that was checked had been converted from a legacy font. Rakaransaya and yansaya are missing their ZWJ, and stray glyphs are scattered through the text.
  - The text is the 13th-century prose classic Saddharma Ratnavaliya, with about 50 words of a roughly 655-word excerpt repaired by hand. A Sinhala reader should proof it.
  - The edition behind the transcription is unknown. That leaves a small risk of an editorial copyright in its particular readings; the work itself is public domain.
- **Tamil:** the piece was published during Bharati's lifetime (he died in 1921), but its exact first-publication date is unverified.
- **Excerpts:** these stories are cut at a paragraph or sentence boundary, not complete: Assamese, Odia, Gujarati, Marathi, Kannada, Punjabi (chapter 1), Nepali and Sinhala. Bengali, Telugu, Tamil and Malayalam (one chapter) are complete.
- **Source quirks kept on purpose:**
  - Malayalam keeps its 1889 orthography, with word-final ് samvruthokaram (മൂന്നു്).
  - Gujarati keeps a ZWNJ in ર્‌હેતો.
  - Kannada is OCR from a 1924 printing. The systematic error where ೦ stood for ಂ is fixed; a few other OCR slips may remain.

## What the torture files cover

| Shaping case | Examples |
|---|---|
| Conjuncts / clusters | ক্ষ, জ্ঞ, স্ত্র, ష్ట్ర, ಸ್ತ್ರೀ |
| Reph or its local equivalent | Devanagari र्; Assamese ৰ্; Kannada arkavottu; Malayalam chillu-ർ as reph; Sinhala repaya ර්‍; Gurmukhi subjoined ੍ਰ/੍ਹ/੍ਵ instead |
| Pre-base vowel signs | ি, ে, ಿ, െ, ෙ, Gurmukhi ਿ |
| Pre-base ra-sign | Malayalam ്ര |
| Split vowels | Bengali/Assamese ো ৌ; Odia ୋ ୌ; Tamil ொ ோ ௌ; Malayalam ൊ ോ ൗ; Sinhala ො ෝ ෞ; Kannada/Telugu two-part matras |
| Malayalam chillus | ൻ ൾ ൺ ർ ൽ, plus the ന്റ ligature |
| Visible virama / halant | Including ZWNJ forms such as পোস্ট্‌মাস্টার and ર્‌હેતો |
| Nukta | Including NFC-decomposed letters (ড় ঢ় য়, ଡ଼ ଢ଼, ਸ਼ ਖ਼ ਗ਼ ਜ਼ ਫ਼), which must still render as one glyph |
| Signs | Candrabindu, anusvara, visarga, Telugu arasunna ఁ, Gurmukhi tippi/bindi/addak |
| Marathi-specific | Eyelash ऱ्य; candra ॲ/ॅ |
| Digits | Native digits for each script; Tamil ௰ ௱ ௲; Sinhala Lith digits ෦–෯ |
| Punctuation | Danda / double danda; Sinhala kundaliya ෴ |
| Mixed text | One line per language mixing English with the script |

Some languages don't use certain cases natively. Gujarati, Marathi, Kannada and Tamil skip nukta; Tamil skips anusvara. Those lines use the script's own hard cases instead, such as Tamil aytham and the u/uu ligatures, or Gujarati ઋ-signs.
