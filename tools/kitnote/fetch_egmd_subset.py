# SPDX-License-Identifier: GPL-3.0-only
"""Downloads a part of the Expanded Groove MIDI Dataset (E-GMD v1.0.0; Callender, Hawthorne, Engel: "Improving
Perceptual Quality of Drum Transcription with the Expanded Groove MIDI Dataset", 2020; Google Magenta;
CC BY 4.0) - the same performances as the Groove MIDI Dataset played back on 43 drum kits - without the whole
96 GB archive: the archive's directory is read over HTTP ranges and only the files chosen are fetched.

    python fetch_egmd_subset.py --out <folder> [--minutes-per-kit 15] [--valid-minutes-per-kit 2]

Writes <out>/e-gmd-v1.0.0/... as the archive has it, and <out>/e-gmd-subset.csv listing what was fetched.
"""
import argparse
import csv
import io
import os
import random
import urllib.request
import zipfile

URL = "https://storage.googleapis.com/magentadata/datasets/e-gmd/v1.0.0/e-gmd-v1.0.0.zip"


class RemoteFile(io.RawIOBase):
    """A file over HTTP ranges, read in blocks, for zipfile."""

    def __init__(self, url, block=4 << 20):
        self.url, self.block, self.pos = url, block, 0
        with urllib.request.urlopen(urllib.request.Request(url, method="HEAD")) as r:
            self.size = int(r.headers["Content-Length"])
        self.cache = {}

    def seekable(self):
        return True

    def readable(self):
        return True

    def seek(self, offset, whence=0):
        self.pos = {0: offset, 1: self.pos + offset, 2: self.size + offset}[whence]
        return self.pos

    def tell(self):
        return self.pos

    def _block(self, i):
        if i not in self.cache:
            if len(self.cache) > 8:
                self.cache.clear()
            start = i * self.block
            end = min(self.size, start + self.block) - 1
            req = urllib.request.Request(self.url, headers={"Range": f"bytes={start}-{end}"})
            with urllib.request.urlopen(req) as r:
                self.cache[i] = r.read()
        return self.cache[i]

    def read(self, n=-1):
        if n is None or n < 0:
            n = self.size - self.pos
        out = bytearray()
        while n > 0 and self.pos < self.size:
            i, off = divmod(self.pos, self.block)
            chunk = self._block(i)[off:off + n]
            out += chunk
            self.pos += len(chunk)
            n -= len(chunk)
        return bytes(out)

    def readinto(self, b):
        data = self.read(len(b))
        b[:len(data)] = data
        return len(data)


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    parser.add_argument("--out", required=True)
    parser.add_argument("--minutes-per-kit", type=float, default=15)
    parser.add_argument("--valid-minutes-per-kit", type=float, default=2)
    parser.add_argument("--seed", type=int, default=1)
    args = parser.parse_args()
    random.seed(args.seed)

    archive = zipfile.ZipFile(RemoteFile(URL))
    names = set(archive.namelist())
    table = next(n for n in names if n.endswith(".csv"))
    rows = list(csv.DictReader(io.TextIOWrapper(archive.open(table), encoding="utf-8")))
    print(f"{len(names)} files in the archive, {len(rows)} performances listed", flush=True)

    chosen = []
    by_kit = {}
    for row in rows:
        by_kit.setdefault((row["kit_name"], row["split"]), []).append(row)
    for (kit, split), kit_rows in sorted(by_kit.items()):
        budget = 60 * {"train": args.minutes_per_kit, "validation": args.valid_minutes_per_kit}.get(split, 0)
        random.shuffle(kit_rows)
        total = 0.0
        for row in kit_rows:
            if total >= budget:
                break
            chosen.append(row)
            total += float(row["duration"])
    print(f"fetching {len(chosen)} performances, {sum(float(r['duration']) for r in chosen) / 3600:.1f} h", flush=True)

    root = os.path.dirname(table)
    members = [f"{root}/{row[key]}" if root else row[key] for row in chosen for key in ("audio_filename", "midi_filename")]
    members = [m for m in members if not os.path.exists(os.path.join(args.out, m))]

    # many at once: each file is a few requests, and one after another they wait on the network
    import threading
    from concurrent.futures import ThreadPoolExecutor
    local = threading.local()

    def fetch(member):
        if not hasattr(local, "archive"):
            local.archive = zipfile.ZipFile(RemoteFile(URL, block=1 << 20))
        target = os.path.join(args.out, member)
        os.makedirs(os.path.dirname(target), exist_ok=True)
        with local.archive.open(member) as src, open(target + ".part", "wb") as dst:
            dst.write(src.read())
        os.replace(target + ".part", target)

    with ThreadPoolExecutor(max_workers=16) as pool:
        for k, _ in enumerate(pool.map(fetch, members)):
            if k % 200 == 0:
                print(f"{k + 1}/{len(members)}", flush=True)
    with open(os.path.join(args.out, "e-gmd-subset.csv"), "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=list(chosen[0].keys()) + ["root"])
        writer.writeheader()
        for row in chosen:
            writer.writerow({**row, "root": root})
    print("done", flush=True)


if __name__ == "__main__":
    main()
