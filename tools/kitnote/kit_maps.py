# SPDX-License-Identifier: GPL-3.0-only
"""What the pieces of the sampled kits are, where their names do not say it well enough: render_kits.py plays a
stroke of the Groove MIDI Dataset on the piece this says, so a wrong entry here is a wrong training label.

Kinds (as sfz_kit.py has them): kick, snare, rim, stick, hh_closed, hh_open, hh_pedal, tom_high, tom_mid, tom_low,
crash (china and splash too), ride, bell. A kind a kit lacks falls back as sfz_kit.FALLBACK says, or the strokes
of it are left out (audio and labels both).
"""
import re

# SFZ kits: (a pattern of the .sfz path, {kind: [keys]}, {controller: value held}, offset opcodes obeyed, a pattern
# of sample paths left out). "*silence" regions only cut other strokes short: one picked for a stroke's microphone
# would leave the stroke without it.
SFZ = [
    # Big Rusty Drums 1.000 (Karoryfer Samples, CC0): its own keymap (Programs/keymap/keymap.sfz); the pedal
    # (CC 4) held open so key 46 is the open hi-hat; no chokes, mechanical noises, brush stirs or percussive clicks
    (r"big-rusty-drums[^/]*/programs/01-full\.sfz$",
     {"kick": [36, 35], "snare": [38, 39], "rim": [40], "stick": [37],
      "hh_closed": [42, 54], "hh_open": [46], "hh_pedal": [44],
      "tom_high": [47], "tom_mid": [45], "tom_low": [43, 41],
      "crash": [49, 65, 57], "ride": [51, 60], "bell": [53, 62]},
     {4: 127}, False, r"\*silence"),
    # Swirly Drums 1.104 (Karoryfer Samples, CC0), played with brushes: pedal held open for key 46; not the cajon
    # kick (35), the snare stirs and flutters, the hi-hat splash and percussion, or the snare buzz layered on the
    # kick (alone, it would be a kick that is only a snare's rattle)
    (r"swirly-drums[^/]*/programs/full_kit\.sfz$",
     {"kick": [36], "snare": [38, 40],
      "hh_closed": [42], "hh_open": [46], "hh_pedal": [44],
      "tom_high": [48, 47], "tom_mid": [45], "tom_low": [43, 41],
      "crash": [49, 55, 53, 57, 59], "ride": [51]},
     {4: 115}, False, r"\*silence|kick_buzz"),
    # Gogodze Phu Vol II 1.001 (Karoryfer Samples, CC0): kick, snare, three toms, hi-hat; no cymbals; not the pedal
    # chick (44), which sounds 30 ms after its start - the pedal plays the closed hi-hat
    (r"gogodze-phu-vol-ii[^/]*/programs/kit\.sfz$",
     {"kick": [36], "snare": [38, 40], "stick": [37],
      "hh_closed": [42], "hh_open": [46],
      "tom_high": [45], "tom_mid": [43], "tom_low": [41]},
     {4: 127}, True, r"\*silence"),
    # Sam's Sonor (Sam Greene, CC BY-SA 4.0): not the snares-off snare (48, 50) or the tom clicks (47)
    (r"samssonor[^/]*/samssonor\.sfz$",
     {"kick": [36], "snare": [38, 40, 39], "rim": [37],
      "hh_closed": [42], "hh_open": [46, 56, 58], "hh_pedal": [44],
      "tom_high": [45], "tom_low": [43],
      "crash": [49, 52], "ride": [51], "bell": [53]},
     {}, True, r"\*silence"),
    # Virtuosity Drums 0.925 (Versilian Studios, Karoryfer Samples; CC0): its offsets obeyed (the hi-hat pedal
    # samples have a count-in); not the hi-hat foot splash (92, a hi-hat, not a crash), the snare flam and roll (95,
    # 96: more than one stroke) or the half-open low tom (91, slow to sound); the pedal chick (44) still sounds
    # 25 ms after its start - the pedal plays the closed hi-hat
    (r"virtuosity[^/]*/programs/02-full-kit\.sfz$",
     {"kick": [36, 35], "snare": [38, 39, 86, 87, 93], "rim": [40], "stick": [37, 85, 88],
      "hh_closed": [42, 94], "hh_open": [46, 90],
      "tom_high": [48, 50], "tom_low": [41, 43, 45, 47, 89],
      "crash": [49, 57], "ride": [51, 59, 55], "bell": [53]},
     {}, True, r"\*silence"),
    # Salamander Drumkit v1 (Alexander Holm, CC BY-SA 3.0): not the snares-off snares (39, 37), which sound as toms
    (r"salamander_drumkit_v1/all\.sfz$",
     {"kick": [35, 36], "snare": [40, 38], "stick": [41],
      "hh_closed": [42], "hh_open": [46], "hh_pedal": [44],
      "tom_high": [45], "tom_low": [43],
      "crash": [55, 54, 57, 56, 62, 59, 58, 60, 61, 63], "ride": [52, 48, 50, 51], "bell": [53, 49]},
     {}, False, None),
]

# DrumGizmo kits, by the kit's folder name: {GM pitch: instrument} put over what render_kits.WANTED finds
DRUMGIZMO = {
    # ShittyKit 1.2 (Sardonicus, CC BY 4.0): its hi-hat is "HH-*", its cross-stick "snare-RmClk"; the floor tom
    # played right-handed (the two softest left-hand samples start 30 ms late)
    "ShittyKit": {37: "snare-RmClk", 42: "HH-closed", 22: "HH-closed", 46: "HH-open", 26: "HH-open", 44: "HH-pdl",
                  38: "snare-Dyn-LH", 40: "snare-Hrd-LH", 41: "Floor-RH", 43: "Floor-RH", 58: "Floor-RH"},
    # The Aasimonster 2.1 (DrumGizmo, CC BY 4.0): the centre snare, four toms, the 18" china
    "aasimonster2": {38: "snare_on_center", 43: "tom_3", 52: "china_18_inch"},
}

# Hydrogen kits (convert_kits.py), by the kit's <name>: {kind: pattern of instrument names}; instruments no
# pattern takes (claps, percussion, chokes, effects) are left out.
# AVL Drumkits (Glen MacArthur, CC BY-SA 3.0): not the "StickClick" (sticks struck together)
_AVL = {"kick": r".*-kick", "stick": r".*-sidestick", "snare": r".*-snare(center|edge)?",
        "hh_closed": r".*-hatclosed", "hh_open": r".*-hat(semi|swish)", "hh_pedal": r".*-hatpedal",
        "tom_high": r".*-\d+-tom(center|edge)?", "tom_low": r".*-(flrtom|floortom|fl\.tomedge)(center|edge)?",
        "crash": r".*-(\d+-crash|splash|china|chinacrash)(-\d+)?", "ride": r".*-ride(tip|shank)?", "bell": r".*-ridebell"}
HYDROGEN = {
    "AVLDrumkits-BlackPearl-2023": _AVL,
    "AVLDrumkits-BlondeBop": _AVL,
    "AVLDrumkits-BlondeBop-HotRod": _AVL,
    "AVLDrumkits-RedZeppelin-2023": _AVL,
    # BJA Pacific (Bransin James Anderson, CC BY-SA 3.0 US): not its "Snare Dry" - no snare wires, it sounds as a tom
    "BJA_Pacific": {"kick": r"bassdrum", "snare": r"snare", "hh_closed": r"hi hat closed",
                    "hh_open": r"hi hat (opened|free)", "tom_high": r"tom", "tom_low": r"floor tom",
                    "crash": r"crash (left|right)", "ride": r"ride( 2)?", "bell": r"ride bell"},
    # ArtemioLabs (GPL): an emulated TR-808 and two synthesised electro kits (no toms or cymbals in those two)
    "TR808EmulationKit": {"kick": r"kick (long|short)", "snare": r"snare \d", "hh_closed": r"closed hat",
                          "hh_open": r"open hat", "hh_pedal": r"pedal hat", "tom_high": r"tom hi",
                          "tom_mid": r"tom mid", "tom_low": r"tom low", "crash": r"cymbal"},
    "ElectricEmpireKit": {"kick": r"kick .*", "snare": r"snare .*", "hh_closed": r"hat closed .*",
                          "hh_open": r"hat op .*"},
    "HardElectro1": {"kick": r"kick \d", "snare": r"snare", "hh_closed": r"closed hh \d|side hh",
                     "hh_open": r"open hh|side ohh"},
    # VariBreaks (Sineshine, GPL): synthesised on a Roland V-Synth; no cymbals
    "VariBreaks": {"kick": r"varibreaks kick \d", "snare": r"varibreaks snare \d",
                   "hh_closed": r"varibreaks hat \d cl", "hh_open": r"varibreaks hat 1 op",
                   "hh_pedal": r"varibreaks hat 1 pd", "tom_high": r"varibreaks tom hi",
                   "tom_mid": r"varibreaks tom mid", "tom_low": r"varibreaks tom low"},
    # Millo MultiLayered 3 (Emillo, GPL): a Ludwig kit; not the snare roll
    "Millo_MultiLayered3": {"kick": r"kick", "stick": r"stick", "snare": r"snare rock", "hh_closed": r"closed hh",
                            "hh_open": r"open hh", "hh_pedal": r"pedal hh", "tom_high": r"tom hi", "tom_mid": r"tom mid",
                            "tom_low": r"tom low", "crash": r"crash", "ride": r"ride jazz"},
    # FreePats synthesizer percussion (FreePats project, CC0)
    "FreePats synthesizer percussion 20220718": {
        "kick": r"bass drum \d", "snare": r"snare drum \d", "hh_closed": r"closed hi hat \d", "hh_open": r"open hi hat",
        "tom_high": r"high tom \d", "tom_mid": r"mid tom \d", "tom_low": r"low tom \d", "crash": r"cymbal \d"},
    # SF3007-2011-Set-03 (Rainer Steffen Hain, WTFPL): a Sonor Force 3007 with many cymbals; not the rototoms,
    # trash hats or percussion
    "SF3007-2011-Set-03": {"kick": r"bassdrum .*", "snare": r"snaredrum .*", "stick": r"rimclick .*",
                           "hh_closed": r"hihat closed|stagg hihat 10\" closed",
                           "hh_open": r"hihat (half open|open short|open long)|zildjian hihat 10\" open",
                           "tom_high": r"tom (8|10)\"", "tom_mid": r"tom (12|13)\"", "tom_low": r"tom (14|16)\"",
                           "crash": r"(?!.*hihat)(masterworks|anatolian|stagg|ozone|istanbul|paiste|wuhan|meinl).*",
                           "ride": r"ride cymbal 22\"", "bell": r"ride bell 22\""},
}

# Drumlabooh kits (convert_kits.py, from psemiletov/drum_sklad), by folder name: {kind: pattern of instrument names}
DRUMLABOOH = {
    # Gretsch Jazzkit: Freesound packs "Gretsch drums" (bigjoedrummer) and "Jazz Gretsch Big Cymbals" (shpira), CC0
    "Jazzkit": {"kick": r"bass drum", "snare": r"snare", "tom_low": r"floor tom", "tom_mid": r'tom "12',
                "tom_high": r'tom "(10|8)', "hh_closed": r"hihat close( hit)?", "hh_open": r"hihat (open|close half)",
                "crash": r"cymbal (low|big)", "ride": r"cymbal small"},
    # El Nacho Drumkit (copyc4t, Freesound pack 22277, CC BY 4.0)
    "Copyc4t El Nacho Drumkit": {"kick": r"kick", "snare": r"snare", "tom_low": r"low tom", "tom_mid": r"mid tom",
                                 "tom_high": r"high tom", "hh_closed": r"hihats close", "hh_open": r"hihats open",
                                 "crash": r"cymbal"},
    # Avangard (Peter Semiletov, made with Uhhyou Web Synthesizers; public domain): kicks, snares, a crash
    "Avangard": {"kick": r"bass drum( 2)?", "snare": r"(techno|coupled|roughly) snare", "crash": r"crash cymbal"},
}

# SoundFont drum kits (convert_kits.py): GM keys by kind - GM 40 is a second (electric) snare, 52 a china, 55 a splash
GM_KINDS = {35: "kick", 36: "kick", 37: "stick", 38: "snare", 40: "snare", 42: "hh_closed", 44: "hh_pedal",
            46: "hh_open", 41: "tom_low", 43: "tom_low", 45: "tom_mid", 47: "tom_mid", 48: "tom_high",
            50: "tom_high", 49: "crash", 57: "crash", 52: "crash", 55: "crash", 51: "ride", 59: "ride", 53: "bell"}


def sfz_map(path):
    """(keymap, controllers held, offsets, skip) for an .sfz file, or (None, None, False, None) to guess by file
    names."""
    p = path.replace("\\", "/").lower()
    for pattern, keymap, cc, offsets, skip in SFZ:
        if re.search(pattern, p):
            return keymap, cc, offsets, skip
    return None, None, False, None
