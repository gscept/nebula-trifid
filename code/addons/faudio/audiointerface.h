#pragma once
//------------------------------------------------------------------------------
/**
    @class FAudio::AudioInterface
  
    Implements the interface to the subsystem.

    (C) 2014 Individual contributors, see AUTHORS file
*/    
#include "messaging/port.h"
#include "core/singleton.h"

//------------------------------------------------------------------------------
namespace FAudio
{
class AudioHandler;

class AudioInterface : public Messaging::Port
{
	__DeclareClass(AudioInterface);
	__DeclareSingleton(AudioInterface);
public:
    /// constructor
	AudioInterface();
    /// destructor
	virtual ~AudioInterface();

    /// open the interface object
    virtual void Open();
    /// close the interface object
    virtual void Close();

private:
    Ptr<FAudio::AudioHandler> handler;
};

} // namespace FAudio
//------------------------------------------------------------------------------
