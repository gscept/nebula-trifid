#pragma once
//------------------------------------------------------------------------------
/**
    @class Managers::SequencerFeatureUnit
    
    (C) 2008 Radon Labs GmbH
*/
#include "application/game/featureunit.h"

namespace Sequencer
{
class SequencerManager;

class SequencerFeatureUnit : public Game::FeatureUnit
{
    __DeclareClass(SequencerFeatureUnit);
    __DeclareSingleton(SequencerFeatureUnit);

public:
    /// constructor
    SequencerFeatureUnit();
    /// destructor
    virtual ~SequencerFeatureUnit();

	/// called from GameServer::ActivateProperties()
    virtual void OnActivate();  
    /// called when removed from game server
    virtual void OnDeactivate();
    /// called after world is loaded
    virtual void OnStart();

    /// called on begin of frame
    virtual void OnBeginFrame();
    /// called in the middle of the feature trigger cycle
    virtual void OnFrame();        
    /// called at the end of the feature trigger cycle
    virtual void OnEndFrame();     

private:
	SequencerManager *sequencManager;
};

}//End Sequencer Namespace