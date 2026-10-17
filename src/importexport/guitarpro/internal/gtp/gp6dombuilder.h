#ifndef KN_IMPORTEXPORT_GP6DOMBUILDER_H
#define KN_IMPORTEXPORT_GP6DOMBUILDER_H

#include "gp67dombuilder.h"

namespace kn::iex::guitarpro {
class GP6DomBuilder : public GP67DomBuilder
{
public:
    GP6DomBuilder() = default;

private:
    virtual std::pair<int, std::unique_ptr<GPTrack> > createGPTrack(kfw::XmlDomNode* trackNode, kfw::XmlDomNode* versionNode);
    void setUpInstrument(kfw::XmlDomNode* trackChildNode, GPTrack* track);
    GPTrack::SoundAutomation readRsePickUp(kfw::XmlDomNode& rseNode) const;
};
} // namespace kn::iex::guitarpro
#endif // KN_IMPORTEXPORT_GP6DOMBUILDER_H
