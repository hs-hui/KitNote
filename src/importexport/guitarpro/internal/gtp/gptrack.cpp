#include "gptrack.h"

namespace kn::iex::guitarpro {
void GPTrack::addSound(Sound sound)
{
    kfw::String key = sound.path + u";" + sound.name + u";" + sound.role;

    _sounds.insert({ key, sound });
}
} // namespace kn::iex::guitarpro
