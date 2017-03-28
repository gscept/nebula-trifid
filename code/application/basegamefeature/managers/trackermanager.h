#pragma once
//------------------------------------------------------------------------------
/**
@class Tracking::TrackerManager

(C) 2016 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "game/entity.h"
#include "core/singleton.h"
#include "attr/attrid.h"
#include "io/stream.h"
#include "util/array.h"
#include "util/fixedarray.h"
#include "util/keyvaluepair.h"
#include "io/textwriter.h"
#include "io/textreader.h"
#include <functional>

namespace Tracking
{
enum TrackingMode
{
    IDLE,
    RECORD,
    PLAYBACK
};

class TrackerManager : public Game::Manager
{
    __DeclareClass(TrackerManager);
    __DeclareSingleton(TrackerManager);
public:

    
    /// constructor
    TrackerManager();
    /// destructor
    ~TrackerManager();

    ///
    void OnBeginFrame();
    ///
    void OnActivate();
    ///
    void OnDeactivate();

    ///
    void AddTracking(const Ptr<Game::Entity> & entity, const Attr::AttrId& id);
    ///
    void AddReplay(unsigned int spot, const Ptr<Game::Entity>& entity, const Attr::AttrId& id);
    ///
    void SetFile(const IO::URI& stream);

    ///
    void SetMode(TrackingMode mode);

    std::function<void(TrackingMode)> callback;
private:
    ///
    void LoadTrackingData();
    typedef struct
    {
        Timing::Tick tick;
        Util::Array<Util::String> vals;
    } TrackingEntry;

    IO::URI uri;
    Ptr<IO::Stream> stream;
    Ptr<IO::TextWriter> writer;
    Ptr<IO::TextReader> reader;
    Util::Array<Util::KeyValuePair<Ptr<Game::Entity>, Attr::AttrId>> attributes;
    TrackingMode mode;
    Util::Array<TrackingEntry> replay;
    Util::FixedArray<Attr::AttrId> attrSlots;
    Util::FixedArray<Util::Array<Util::KeyValuePair<Ptr<Game::Entity>, Attr::AttrId>>> targets;
    IndexT nextSample;
    Timing::Tick sampleStart;
    
};
}