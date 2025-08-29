#!/usr/bin/env python3
# Patch genhtml en posant <span class="lineNoCov">0</span> sur les lignes "0"
# détectées dans coverage.utf8.txt (après fix_coverage.py). Mapping via lcov.info.
import io, os, re, sys, time

def usage():
    print("usage: patch_genhtml_forcecover_inline.py --genhtml-dir DIR --fixed-txt coverage.utf8.txt --repo-root ROOT --lcov-info lcov.info", file=sys.stderr)
    sys.exit(2)

args = sys.argv[1:]
def take(flag):
    if flag in args:
        i = args.index(flag); v = args[i+1]; del args[i:i+2]; return v
    return None

if not all(f in args for f in ["--genhtml-dir","--fixed-txt","--repo-root","--lcov-info"]):
    usage()

GEN_DIR  = os.path.abspath(take("--genhtml-dir"))
FIX_TXT  = os.path.abspath(take("--fixed-txt"))
REPOROOT = os.path.abspath(take("--repo-root"))
LCOVINFO = os.path.abspath(take("--lcov-info"))

def lcov_sources(path):
    res=[]
    with io.open(path,'r',encoding='utf-8',errors='replace') as f:
        for line in f:
            if line.startswith("SF:"):
                res.append(os.path.abspath(line[3:].strip()))
    return res

def common_prefix_dir(paths):
    if not paths: return ""
    parts=[p.split(os.sep) for p in paths]
    m=min(len(p) for p in parts)
    out=[]
    for i in range(m):
      seg=parts[0][i]
      if all(p[i]==seg for p in parts):
        out.append(seg)
      else:
        break
    return os.sep.join(out)

LCOV_SF = lcov_sources(LCOVINFO)
LCOV_BASE = common_prefix_dir(LCOV_SF) or REPOROOT

re_file = re.compile(r'^(/.*?):\s*$')
re_line = re.compile(r'^\s*(\d+)\|\s*([^|]+)\|')
miss = {}  # abs_path -> set(lines)
cur = None
with io.open(FIX_TXT, 'r', encoding='utf-8', errors='replace') as f:
    for line in f:
        if line.startswith('===== '): 
            continue
        m = re_file.match(line)
        if m:
            cur = os.path.abspath(m.group(1)); miss.setdefault(cur,set()); continue
        m = re_line.match(line)
        if m and cur:
            try: hits = int(m.group(2).strip())
            except ValueError: continue
            if hits == 0: miss[cur].add(int(m.group(1)))

def html_for_source(abs_src):
    rel = abs_src
    if abs_src.startswith(LCOV_BASE):
        rel = os.path.relpath(abs_src, LCOV_BASE)
    elif abs_src.startswith(REPOROOT):
        rel = os.path.relpath(abs_src, REPOROOT)
    cand = os.path.join(GEN_DIR, rel) + ".gcov.html"
    if os.path.exists(cand): return cand
    # fallback: cherche par nom
    target = os.path.basename(abs_src) + ".gcov.html"
    for root,_,files in os.walk(GEN_DIR):
        if target in files:
            return os.path.join(root, target)
    return None

def patch_row_for_line(html_text, line_no):
    anchor_rx = re.compile(rf'<a[^>]+(?:name|id)=["\']?L{line_no}["\']?[^>]*>', re.IGNORECASE)
    m = anchor_rx.search(html_text)
    if not m: return html_text, False
    start_tr = html_text.rfind("<tr", 0, m.start())
    end_tr   = html_text.find("</tr>", m.end())
    if start_tr == -1 or end_tr == -1: return html_text, False
    end_tr += len("</tr>")
    row = html_text[start_tr:end_tr]
    tds = list(re.finditer(r'<td\b[^>]*>', row, re.IGNORECASE))
    if len(tds) < 2: return html_text, False
    td2 = tds[1]
    end_td2 = row.find("</td>", td2.end())
    if end_td2 == -1: return html_text, False
    content = row[td2.end():end_td2].strip()
    to_write = None
    if not content or content in ("&nbsp;", "-", "—"):
        to_write = '<span class="lineNoCov">0</span>'
    else:
        try:
            v = int(re.sub(r'[^\d-]', '', content))
            if v == 0: to_write = '<span class="lineNoCov">0</span>'
        except Exception:
            to_write = '<span class="lineNoCov">0</span>'
    if to_write is None: return html_text, False
    new_row = row[:td2.end()] + to_write + row[end_td2:]
    new_html = html_text[:start_tr] + new_row + html_text[end_tr:]
    return new_html, True

targets = [(src, html_for_source(src)) for src in miss.keys()]
targets = [(s,h) for (s,h) in targets if h]

print(f"[force-cover] patch start: html_dir={GEN_DIR}")
print(f"[force-cover] lcov base = {LCOV_BASE}")
print(f"[force-cover] files_with_zeros that exist in HTML = {len(targets)} / {len(miss)}")

patched_pages = 0
patched_lines = 0
last_log = time.time()

for idx, (src, html_path) in enumerate(sorted(targets)):
    if time.time() - last_log > 2.0:
        print(f"[force-cover] progress: {idx}/{len(targets)}")
        last_log = time.time()
    zeros = miss[src]
    try:
        with io.open(html_path, 'r', encoding='utf-8', errors='replace') as f:
            t = f.read()
        changed_any = False
        for ln in sorted(zeros):
            t2, changed = patch_row_for_line(t, ln)
            if changed:
                patched_lines += 1
                changed_any = True
                t = t2
        if changed_any:
            with io.open(html_path, 'w', encoding='utf-8') as g:
                g.write(t)
            patched_pages += 1
    except Exception:
        pass

print(f"[force-cover] patch done: pages={patched_pages}, lines_set_to_zero={patched_lines}")
