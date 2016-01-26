#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::AudioHandler
    
    Message handler for the Audio subsystem.

    (C) 2015 Individual contributors, see AUTHORS file
*/

#include "messaging/port.h"
#include "messaging/message.h"
#include "faudio/audioprotocol.h"
#include "http/httpserverproxy.h"
#include "timing/timer.h"
#include "messaging/staticmessagehandler.h"

namespace FAudio { class EventInstance; }
namespace Debug { class FmodStudioPageHandler; }

//------------------------------------------------------------------------------
namespace FAudio
{

class AudioHandler : public Messaging::Handler
{
	__DeclareClass(AudioHandler);
public:
    /// constructor
	AudioHandler();
    /// destructor
	virtual ~AudioHandler();

    /// open the handler
    virtual void Open();
    /// close the handler
    virtual void Close();
    /// handle a message, return true if handled
    virtual bool HandleMessage(const Ptr<Messaging::Message>& msg);    

private:        	       
	friend class Messaging::StaticMessageHandler;
#if __NEBULA3_HTTP__
    Ptr<Debug::FmodStudioPageHandler> debugPageHandler;
#endif
};

} // namespace FAudio
//------------------------------------------------------------------------------

