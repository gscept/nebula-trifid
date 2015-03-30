#pragma once

//------------------------------------------------------------------------------
/**
    @class Test::FAudioApplication
  
    Testapplication for FMOD Studio
    
	(C) 2015 Individual contributors, see AUTHORS file
*/    
#include "apprender/viewerapplication.h"
#include "faudio/audiointerface.h"
#include "faudio/audioserver.h"
#include "faudio/eventinstance.h"
#include "faudio/audiolistener.h"

//------------------------------------------------------------------------------
namespace Test
{
class FAudioApplication : public App::ViewerApplication
{
public:
    /// constructor
	FAudioApplication();
    /// destructor
	virtual ~FAudioApplication();
    /// open the application
    virtual bool Open();
    /// close the application
    virtual void Close();

private:
	/// process input (called before rendering)
    void OnProcessInput();
    /// called when frame should be updated
    void OnUpdateFrame();

	Ptr<FAudio::AudioDevice> audioInterface;
	Ptr<FAudio::AudioServer> audioServer;	
	Ptr<FAudio::AudioEmitter> emitter;
	Ptr<FAudio::AudioListener> listener;	

	Util::String debugText;
};

} // namespace Test
//------------------------------------------------------------------------------
