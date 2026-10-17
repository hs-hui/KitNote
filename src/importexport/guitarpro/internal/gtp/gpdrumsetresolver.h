#ifndef KN_IMPORTEXPORT_GPDRUMSETRESOLVER_H
#define KN_IMPORTEXPORT_GPDRUMSETRESOLVER_H

#include <map>
#include <list>

#include "types/string.h"

namespace kn::iex::guitarpro {
struct GPDrum {
    int32_t pitch = -1;
    int32_t element = -1;
    int32_t variation = -1;
    kfw::String _name;

    GPDrum() {}
    GPDrum(int32_t pitch, int32_t element, int32_t variation, const kfw::String& name = u"drmkt")
        : pitch(pitch), element(element), variation(variation), _name(name) {}
};

class GPDrumSetResolver
{
public:

    GPDrumSetResolver() = default;
    virtual ~GPDrumSetResolver() = default;

    void initGPDrum();

    int32_t pitch(int32_t element, int32_t variation, const kfw::String& name) const;

private:

    void addDrum(const GPDrum& drum);

    std::map<kfw::String, std::list<GPDrum> > _drum;
};
} // namespace kn::iex::guitarpro
#endif // KN_IMPORTEXPORT_GPDRUMSETRESOLVER_H
