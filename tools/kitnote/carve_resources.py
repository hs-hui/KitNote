# Carve uncompressed Qt resource payloads ([u32 BE length][bytes]) out of a PE file.
import sys, os, re, hashlib, json
exe, out = sys.argv[1], sys.argv[2]
data = open(exe, 'rb').read()
os.makedirs(out, exist_ok=True)
found = {}
def try_blob(start):
    if start < 4: return None
    n = int.from_bytes(data[start-4:start], 'big')
    if not (16 <= n <= 8_000_000) or start + n > len(data): return None
    return n
# text payloads
for m in re.finditer(rb'(?:import Qt|import Muse|pragma Singleton|<\?xml|<svg|\{\s*"|/\*\s*\n \* SPDX)', data):
    p = m.start()
    lo = max(0, p - 6000)
    # payload start = first byte after the last non-text byte before p
    q = p
    while q > lo and (data[q-1] in (9, 10, 13) or 32 <= data[q-1] < 127 or data[q-1] >= 0x80): q -= 1
    for s in (q, q + 1, q + 2, q + 3, q + 4):
        n = try_blob(s)
        if n and s <= p < s + n:
            blob = data[s:s+n]
            try: blob.decode('utf-8')
            except UnicodeDecodeError: continue
            found.setdefault(s, n)
            break
# fonts and images
for sig, ext in ((b'OTTO', 'otf'), (b'\x00\x01\x00\x00\x00', 'ttf'), (b'\x89PNG\r\n\x1a\n', 'png'), (b'wOFF', 'woff')):
    for m in re.finditer(re.escape(sig), data):
        n = try_blob(m.start())
        if n and n > 2000: found.setdefault(m.start(), n)
index = []
for s, n in sorted(found.items()):
    blob = data[s:s+n]
    head = blob[:8]
    ext = 'otf' if head[:4] == b'OTTO' else 'ttf' if head[:5] == b'\x00\x01\x00\x00\x00' else 'png' if head[:4] == b'\x89PNG' else 'woff' if head[:4]==b'wOFF' else \
          'svg' if b'<svg' in blob[:300] else 'xml' if blob[:5] == b'<?xml' else 'json' if blob.lstrip()[:1] in (b'{', b'[') else 'qml'
    name = f'{s:08x}.{ext}'
    open(os.path.join(out, name), 'wb').write(blob)
    index.append({'offset': f'0x{s:08x}', 'bytes': n, 'file': name, 'sha256': hashlib.sha256(blob).hexdigest()})
json.dump(index, open(os.path.join(out, 'index.json'), 'w'), indent=1)
from collections import Counter
print(len(index), Counter(i['file'].split('.')[-1] for i in index))
