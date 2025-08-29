#!/usr/bin/env python3
import sys, io, codecs

if len(sys.argv) != 3:
    print("usage: convert_to_utf8.py in.txt out.utf8.txt", file=sys.stderr)
    sys.exit(1)

src, dst = sys.argv[1], sys.argv[2]

# lis n'importe quoi et ré-écris en UTF-8 (remplacement des caractères invalides)
with open(src, 'rb') as f:
    raw = f.read()
text = raw.decode('utf-8', errors='replace')

with io.open(dst, 'w', encoding='utf-8', newline='\n') as g:
    g.write(text)
