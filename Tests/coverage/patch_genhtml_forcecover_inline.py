#!/usr/bin/env python3
# Patch visuel des pages genhtml: pour chaque ligne marquée "0" (via fix_coverage)
# on force la classe 'lineNoCov' dans le HTML. N'affecte PAS les stats.
import sys, os, re, io

if len(sys.argv) != 3:
    print("usage: patch_genhtml_forcecover_inline.py coverage.utf8.txt coverage_lcov_dir", file=sys.stderr)
    sys.exit(0)

covtxt, htmldir = sys.argv[1], sys.argv[2]

re_file = re.compile(r'^(/.*?):\s*$')
re_line = re.compile(r'^\s*(\d+)\|\s*0\|')
miss = {}
cur = None
with io.open(covtxt, 'r', encoding='utf-8', errors='replace') as f:
    for line in f:
        if line.startswith('===== '):  # sép exe
            continue
        m = re_file.match(line)
        if m:
            cur = os.path.abspath(m.group(1))
            miss.setdefault(cur, set())
            continue
        m = re_line.match(line)
        if m and cur:
            miss[cur].add(int(m.group(1)))

# genhtml nomme les fichiers en remplaçant / par _
def html_for(path):
    return os.path.join(htmldir, path.lstrip('/').replace('/', '_') + '.gcov.html')

for sf, zeros in miss.items():
    html = html_for(sf)
    if not os.path.exists(html) or not zeros:
        continue
    with io.open(html, 'r', encoding='utf-8', errors='replace') as f:
        s = f.read()
    changed = False
    for ln in sorted(zeros):
        # pattern très simple: remplace la cellule de compteur par 0 noCov
        s2 = re.sub(rf'(<a name="L{ln}"></a>\s*<td class="lineCount">)\s*[- ]*(</td>)',
                    r'\g<1><span class="lineNoCov">0</span>\2', s)
        if s2 != s:
            s = s2
            changed = True
    if changed:
        with io.open(html, 'w', encoding='utf-8') as g:
            g.write(s)
