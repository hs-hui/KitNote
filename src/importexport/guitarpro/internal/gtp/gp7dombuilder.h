#ifndef KN_IMPORTEXPORT_GP7DOMBUILDER_H
#define KN_IMPORTEXPORT_GP7DOMBUILDER_H

#include "gpdommodel.h"
#include "gp67dombuilder.h"

namespace kn::iex::guitarpro {
class GP7DomBuilder : public GP67DomBuilder
{
public:
    GP7DomBuilder() = default;

private:
    std::pair<int, std::unique_ptr<GPTrack> > createGPTrack(kfw::XmlDomNode* trackNode, kfw::XmlDomNode* versionNode) override;

    int readMidiChannel(kfw::XmlDomNode* trackChildNode) const;
    int readMidiProgramm(kfw::XmlDomNode* trackChildNode) const;
    GPTrack::Sound readSounds(kfw::XmlDomNode* trackChildNode) const;
    GPTrack::SoundAutomation readTrackAutomation(kfw::XmlDomNode* automationNode) const;
};
} // namespace kn::iex::guitarpro
#endif // KN_IMPORTEXPORT_GP7DOMBUILDER_H
