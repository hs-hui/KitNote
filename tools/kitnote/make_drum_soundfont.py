#!/usr/bin/env python3
"""Cuts a SoundFont (.sf2 / .sf3) down to its percussion presets.

KitNote only plays drums, and MuseScore's default SoundFont spends most of its
50 MB on pitched instruments. This keeps every preset in the percussion bank
(128) together with the instruments and samples those presets use, and writes
a new SoundFont with everything else left out.

    python tools/kitnote/make_drum_soundfont.py "share/sound/MS Basic.sf3" "share/sound/KitNote Drums.sf3"
"""
import struct
import sys
from pathlib import Path

PERCUSSION_BANK = 128
GEN_INSTRUMENT = 41
GEN_SAMPLE_ID = 53
COMPRESSED = 0x10        # sampleType flag used by .sf3 for Ogg Vorbis data


def chunks(data: bytes, start: int, end: int):
    pos = start
    while pos + 8 <= end:
        tag, size = data[pos:pos + 4], struct.unpack_from("<I", data, pos + 4)[0]
        yield tag, pos + 8, size
        pos += 8 + size + (size & 1)


def chunk(tag: bytes, body: bytes) -> bytes:
    return tag + struct.pack("<I", len(body)) + body + (b"\0" if len(body) & 1 else b"")


def records(body: bytes, size: int) -> list[bytes]:
    return [body[i:i + size] for i in range(0, len(body) - len(body) % size, size)]


def subset(src: bytes) -> tuple[bytes, dict]:
    if src[:4] != b"RIFF" or src[8:12] != b"sfbk":
        raise ValueError("not a SoundFont file")
    info = b""
    smpl = b""
    pdta: dict[bytes, bytes] = {}
    for tag, pos, size in chunks(src, 12, len(src)):
        if tag != b"LIST":
            continue
        kind = src[pos:pos + 4]
        if kind == b"INFO":
            info = src[pos + 4:pos + size]
        for sub, spos, ssize in chunks(src, pos + 4, pos + size):
            if kind == b"sdta" and sub == b"smpl":
                smpl = src[spos:spos + ssize]
            elif kind == b"pdta":
                pdta[sub] = src[spos:spos + ssize]

    phdr = [struct.unpack("<20sHHHIII", r) for r in records(pdta[b"phdr"], 38)]
    pbag = [struct.unpack("<HH", r) for r in records(pdta[b"pbag"], 4)]
    pmod = records(pdta[b"pmod"], 10)
    pgen = [struct.unpack("<HH", r) for r in records(pdta[b"pgen"], 4)]
    inst = [struct.unpack("<20sH", r) for r in records(pdta[b"inst"], 22)]
    ibag = [struct.unpack("<HH", r) for r in records(pdta[b"ibag"], 4)]
    imod = records(pdta[b"imod"], 10)
    igen = [struct.unpack("<HH", r) for r in records(pdta[b"igen"], 4)]
    shdr = [struct.unpack("<20sIIIIIBbHH", r) for r in records(pdta[b"shdr"], 46)]

    keep_presets = [i for i, p in enumerate(phdr[:-1]) if p[2] == PERCUSSION_BANK]
    if not keep_presets:
        raise ValueError("the SoundFont has no percussion presets (bank 128)")

    # Which instruments do the kept presets use, and which samples do those use?
    keep_inst: list[int] = []
    for pi in keep_presets:
        for bi in range(phdr[pi][3], phdr[pi + 1][3]):
            for gi in range(pbag[bi][0], pbag[bi + 1][0]):
                if pgen[gi][0] == GEN_INSTRUMENT and pgen[gi][1] not in keep_inst:
                    keep_inst.append(pgen[gi][1])
    keep_samples: list[int] = []
    for ii in keep_inst:
        for bi in range(inst[ii][1], inst[ii + 1][1]):
            for gi in range(ibag[bi][0], ibag[bi + 1][0]):
                if igen[gi][0] == GEN_SAMPLE_ID and igen[gi][1] not in keep_samples:
                    keep_samples.append(igen[gi][1])
    for si in list(keep_samples):                       # stereo partners
        link = shdr[si][8]
        if shdr[si][9] & 0x6 and link < len(shdr) - 1 and link not in keep_samples:
            keep_samples.append(link)
    inst_map = {old: new for new, old in enumerate(keep_inst)}
    sample_map = {old: new for new, old in enumerate(keep_samples)}

    # Samples: copy the audio and renumber.
    new_smpl = bytearray()
    new_shdr = bytearray()
    for si in keep_samples:
        name, start, end, loop_start, loop_end, rate, pitch, corr, link, kind = shdr[si]
        if kind & COMPRESSED:
            # .sf3: start/end are byte offsets of an Ogg stream; loop points are relative to it.
            data = smpl[start:end]
            at = len(new_smpl)
            new_smpl += data
            entry = (name, at, at + len(data), loop_start, loop_end)
        else:
            data = smpl[start * 2:end * 2]
            at = len(new_smpl) // 2
            new_smpl += data + b"\0" * 92                # 46 zero samples after each sample
            entry = (name, at, at + (end - start), loop_start - start + at, loop_end - start + at)
        new_shdr += struct.pack("<20sIIIIIBbHH", *entry, rate, pitch, corr, sample_map.get(link, 0), kind)
    new_shdr += struct.pack("<20sIIIIIBbHH", b"EOS", 0, 0, 0, 0, 0, 0, 0, 0, 0)
    if len(new_smpl) & 1:
        new_smpl += bytes(1)      # some loaders do not skip RIFF pad bytes; keep the chunk even instead

    def rebuild(headers, bags, gens, mods, keep, pack_header, remap_oper, remap):
        out_h, out_b, out_g, out_m = bytearray(), bytearray(), bytearray(), bytearray()
        n_bags = n_gens = n_mods = 0
        for index in keep:
            out_h += pack_header(headers[index], n_bags)
            for bi in range(headers[index][-1 if len(headers[index]) == 2 else 3], headers[index + 1][-1 if len(headers[index]) == 2 else 3]):
                out_b += struct.pack("<HH", n_gens, n_mods)
                n_bags += 1
                for gi in range(bags[bi][0], bags[bi + 1][0]):
                    oper, amount = gens[gi]
                    if oper == remap_oper:
                        amount = remap[amount]
                    out_g += struct.pack("<HH", oper, amount)
                    n_gens += 1
                for mi in range(bags[bi][1], bags[bi + 1][1]):
                    out_m += mods[mi]
                    n_mods += 1
        out_b += struct.pack("<HH", n_gens, n_mods)
        out_g += struct.pack("<HH", 0, 0)
        out_m += b"\0" * 10
        return out_h, out_b, out_g, out_m, n_bags

    ih, ib, ig, im, n = rebuild(inst, ibag, igen, imod, keep_inst,
                                lambda h, bag: struct.pack("<20sH", h[0], bag), GEN_SAMPLE_ID, sample_map)
    ih += struct.pack("<20sH", b"EOI", n)
    ph, pb, pg, pm, n = rebuild(phdr, pbag, pgen, pmod, keep_presets,
                                lambda h, bag: struct.pack("<20sHHHIII", h[0], h[1], h[2], bag, h[4], h[5], h[6]), GEN_INSTRUMENT, inst_map)
    ph += struct.pack("<20sHHHIII", b"EOP", 0, 0, n, 0, 0, 0)

    body = b"sfbk" + chunk(b"LIST", b"INFO" + info) + chunk(b"LIST", b"sdta" + chunk(b"smpl", bytes(new_smpl))) + chunk(
        b"LIST", b"pdta" + chunk(b"phdr", bytes(ph)) + chunk(b"pbag", bytes(pb)) + chunk(b"pmod", bytes(pm)) + chunk(b"pgen", bytes(pg))
        + chunk(b"inst", bytes(ih)) + chunk(b"ibag", bytes(ib)) + chunk(b"imod", bytes(im)) + chunk(b"igen", bytes(ig))
        + chunk(b"shdr", bytes(new_shdr)))
    names = [phdr[i][0].split(b"\0")[0].decode("latin-1") for i in keep_presets]
    return b"RIFF" + struct.pack("<I", len(body)) + body, {"presets": names, "instruments": len(keep_inst), "samples": len(keep_samples)}


def main() -> None:
    src, dst = Path(sys.argv[1]), Path(sys.argv[2])
    data, stats = subset(src.read_bytes())
    dst.write_bytes(data)
    print(f"{src.name}: {src.stat().st_size / 1e6:.1f} MB -> {dst.name}: {len(data) / 1e6:.1f} MB")
    print(f"{len(stats['presets'])} presets, {stats['instruments']} instruments, {stats['samples']} samples")
    print(", ".join(stats["presets"]))


if __name__ == "__main__":
    main()
