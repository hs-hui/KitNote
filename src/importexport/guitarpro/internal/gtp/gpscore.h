#ifndef KN_IMPORTEXPORT_GPSCORE_H
#define KN_IMPORTEXPORT_GPSCORE_H

#include "types/string.h"

namespace kn::iex::guitarpro {
class GPScore
{
public:

    void setTitle(const kfw::String& title) { _title = title; }
    kfw::String title() const { return _title; }
    void setSubTitle(const kfw::String& subTitle) { _subTitle = subTitle; }
    kfw::String subTitle() const { return _subTitle; }
    void setArtist(const kfw::String& artist) { _artist = artist; }
    kfw::String artist() const { return _artist; }
    void setAlbum(const kfw::String& album) { _album = album; }
    kfw::String album() const { return _album; }
    void setPoet(const kfw::String& poet) { _poet = poet; }
    kfw::String poet() const { return _poet; }
    void setComposer(const kfw::String& composer) { _composer = composer; }
    kfw::String composer() const { return _composer; }
    void setMultiVoice(bool multiVoice) { _multiVoice = multiVoice; }
    bool multiVoice() const { return _multiVoice; }

private:
    kfw::String _title;
    kfw::String _subTitle;
    kfw::String _artist;
    kfw::String _album;
    kfw::String _poet;
    kfw::String _composer;
    bool _multiVoice = false;
};
} // namespace kn::iex::guitarpro

#endif // KN_IMPORTEXPORT_GPSCORE_H
