#pragma once
//------------------------------------------------------------------------------
/**
    @class Audio2::Audio2Interface
  
    Implements the interface to the CoreAudio threaded subsystem. Apps
    don't call directly into Audio2Interface, instead use the provided 
    front-end classes in the Audio subsystem.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2014 Individual contributors, see AUTHORS file
*/    
#include "interface/interfacebase.h"
#include "core/singleton.h"
#include "messaging/batchmessage.h"

//------------------------------------------------------------------------------
namespace Audio2
{
class Audio2Interface : public Interface::InterfaceBase
{
    __DeclareClass(Audio2Interface);
    __DeclareInterfaceSingleton(Audio2Interface);
public:
    /// constructor
    Audio2Interface();
    /// destructor
    virtual ~Audio2Interface();

    /// open the interface object
    virtual void Open();
    /// close the interface object
    virtual void Close();

    /// send a batched messages (call FlushBatchedMessages() to send them to audio thread)
    template<class MESSAGETYPE> void SendBatched(const Ptr<MESSAGETYPE>& msg);
    /// flush batched messages, call once per frame
    void FlushBatchedMessages();

private:
    /// send a batched messages (call FlushBatchedMessages() to send them to audio thread)
    void SendBatchedInternal(const Ptr<Messaging::Message>& msg);

protected:
    Threading::ThreadId mainThreadId;
    Ptr<Messaging::BatchMessage> batchMessage;
};

//------------------------------------------------------------------------------
/**
*/
template<class MESSAGETYPE> inline void 
Audio2Interface::SendBatched(const Ptr<MESSAGETYPE>& msg)
{
    this->SendBatchedInternal((const Ptr<Messaging::Message>&)msg);
}


} // namespace Audio2
//------------------------------------------------------------------------------
