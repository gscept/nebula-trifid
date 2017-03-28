#pragma once
//------------------------------------------------------------------------------
/**
    @class BaseGameFeature::AudioManager
    
    Manager for the audio system.
	Sets up the FAudio components, loads default audio banks and triggers 
	the server
        
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
#include "game/manager.h"
#include "faudio/audioserver.h"

//------------------------------------------------------------------------------
namespace BaseGameFeature
{
class AudioManager : public Game::Manager
{
    __DeclareClass(AudioManager);
public:
    /// constructor
    AudioManager();
    /// destructor
    virtual ~AudioManager();

    /// called when attached to game server
    virtual void OnActivate();
    /// called when removed from game server
    virtual void OnDeactivate();      
    /// called per-frame by the game server
    virtual void OnFrame();    
    /// called after loading game state
    virtual void OnLoad();
    /// render a debug visualization 
    virtual void OnRenderDebug();

private:
    Ptr<FAudio::AudioServer> audioServer;
};


}; // namespace BaseGameFeature
//------------------------------------------------------------------------------
 
    
    