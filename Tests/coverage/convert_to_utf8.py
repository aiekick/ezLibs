#!/usr/bin/env python3
import sys, io

if len(sys.argv) != 3:
    print("usage: convert_to_utf8.py <in> <out>", file=sys.stderr)
    sys.exit(2)

src, dst = sys.argv[1], sys.argv[2]

def read_any(path):
    # essaie utf-8 puis latin-1 en fallback
    try:
        with io.open(path, 'r', encoding='utf-8') as f:
            return f.read()
    except UnicodeDecodeError:
        with io.open(path, 'r', encoding='latin-1', errors='replace') as f:
            return f.read()

text = read_any(src)
with io.open(dst, 'w', encoding='utf-8') as g:
    g.write(text)
