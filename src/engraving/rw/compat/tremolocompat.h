#ifndef KN_ENGRAVING_TREMOLOCOMPAT_H
#define KN_ENGRAVING_TREMOLOCOMPAT_H

namespace kn::engraving {
class Chord;
class TremoloSingleChord;
class TremoloTwoChord;
}

namespace kn::engraving::compat {
struct TremoloCompat {
    Chord* parent = nullptr;
    TremoloSingleChord* single = nullptr;
    TremoloTwoChord* two = nullptr;
};
}

#endif // KN_ENGRAVING_TREMOLOCOMPAT_H
