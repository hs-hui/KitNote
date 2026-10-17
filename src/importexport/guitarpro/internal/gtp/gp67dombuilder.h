#ifndef KN_IMPORTEXPORT_GP67DOMBUILDER_H
#define KN_IMPORTEXPORT_GP67DOMBUILDER_H

#include <memory>

#include "igpdombuilder.h"
#include "gpdommodel.h"
#include "gptrack.h"

namespace kn::iex::guitarpro {
class GP67DomBuilder : public IGPDomBuilder
{
public:
    GP67DomBuilder();

    void buildGPDomModel(kfw::XmlDomElement* domElem) override;
    std::unique_ptr<GPDomModel> getGPDomModel() override;

protected:

    void buildGPScore(kfw::XmlDomNode* scoreNode);
    void buildGPMasterTracks(kfw::XmlDomNode* masterTrack);
    void buildGPAudioTracks(kfw::XmlDomNode* audioTrack);
    void buildGPTracks(kfw::XmlDomNode* tracksNode, kfw::XmlDomNode* versionNode);
    void buildGPMasterBars(kfw::XmlDomNode* masterBars);
    void buildGPBars(kfw::XmlDomNode* bars);
    void buildGPVoices(kfw::XmlDomNode* voicesNode);
    void buildGPBeats(kfw::XmlDomNode* beats);
    void buildGPNotes(kfw::XmlDomNode* notesNode);
    void buildGPRhythms(kfw::XmlDomNode* rhythms);

    void breakLyricsOnBeatsIfNeed();
    bool isLyricsOnBeats() const;

    virtual std::pair<int, std::unique_ptr<GPTrack> > createGPTrack(kfw::XmlDomNode* trackNode, kfw::XmlDomNode* versionNode) = 0;

    std::unique_ptr<GPMasterTracks> createGPMasterTrack(kfw::XmlDomNode* metadata);
    std::unique_ptr<GPMasterBar> createGPMasterBar(kfw::XmlDomNode* masterBarNode);
    std::pair<int, std::unique_ptr<GPBar> > createGPBar(kfw::XmlDomNode* barNode);
    std::pair<int, std::unique_ptr<GPVoice> > createGPVoice(kfw::XmlDomNode* voiceNode);
    std::pair<int, std::shared_ptr<GPBeat> > createGPBeat(kfw::XmlDomNode* beatNode);
    std::pair<int, std::shared_ptr<GPNote> > createGPNote(kfw::XmlDomNode* noteNode);
    std::pair<int, std::shared_ptr<GPRhythm> > createGPRhythm(kfw::XmlDomNode* rhythmNode);

    void readNoteXProperties(const kfw::XmlDomNode& propertiesNode, GPNote* n);
    void readNoteProperties(kfw::XmlDomNode* propertiesNode, GPNote* n);
    void readBeatXProperties(const kfw::XmlDomNode& propertiesNode, GPBeat* b);
    std::unique_ptr<GPNote::Bend> createBend(kfw::XmlDomNode* propertyNode);
    void readHarmonic(kfw::XmlDomNode* propertyNode, GPNote* note) const;

    std::vector<GPMasterTracks::Automation> readTempoMap(kfw::XmlDomNode* currentNode);
    GPTrack::RSE readTrackRSE(kfw::XmlDomNode* trackChildNode) const;
    GPMasterBar::KeySig readKeySig(kfw::XmlDomNode* keyNode) const;
    bool readUseFlats(kfw::XmlDomNode* keyNode) const;
    GPMasterBar::TimeSig readTimeSig(kfw::XmlDomNode* timeNode) const;
    void readTrackProperties(kfw::XmlDomNode* propertiesNode, GPTrack* track, bool ignoreTuningFlats) const;
    void readBeatProperties(const kfw::XmlDomNode& propertiesNode, GPBeat* beat) const;
    void readDiagram(const kfw::XmlDomNode& items, GPTrack* track) const;
    void readLyrics(const kfw::XmlDomNode& items, GPTrack* track) const;
    std::vector<GPMasterBar::Fermata> readFermatas(kfw::XmlDomNode* fermatasNode) const;
    std::vector<GPMasterBar::Direction> readRepeatsJumps(kfw::XmlDomNode* repeatsJumpsNode) const;
    std::pair<kfw::String, kfw::String> readMasterBarSection(const kfw::XmlDomNode& sectionNode) const;
    GPMasterBar::Repeat readRepeat(kfw::XmlDomNode* repeatNode) const;
    std::vector<int> readEnding(kfw::XmlDomNode* endNode) const;

    std::unordered_map<int, std::shared_ptr<GPNote> > _notes;
    std::unordered_map<int, std::shared_ptr<GPRhythm> > _rhythms;
    std::unordered_map<int, std::shared_ptr<GPBeat> > _beats;
    std::unordered_map<int, std::unique_ptr<GPVoice> > _voices;
    std::unordered_map<int, std::unique_ptr<GPBar> > _bars;

    std::unique_ptr<GPDomModel> _gpDom;
};
} // kn::iex::guitarpro
#endif // KN_IMPORTEXPORT_GP67DOMBUILDER_H
