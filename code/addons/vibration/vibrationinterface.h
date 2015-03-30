#pragma once
//------------------------------------------------------------------------------
/**
    @class Vibration::VibrationInterface
  
    Implements the interface to the Vibration subsystem as a standalone thread
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
#include "interface/interfacebase.h"
#include "core/singleton.h"
#include "messaging/batchmessage.h"

//------------------------------------------------------------------------------
namespace Vibration
{
class VibrationInterface : public Interface::InterfaceBase
{
    __DeclareClass(VibrationInterface);
    __DeclareInterfaceSingleton(VibrationInterface);
public:
    /// constructor
    VibrationInterface();
    /// destructor
    virtual ~VibrationInterface();

    /// open the interface object
    virtual void Open();
    /// close the interface object
    virtual void Close();

    /// send a batched messages (call FlushBatchedMessages() to send them to audio thread)
    void SendBatched(const Ptr<Messaging::Message>& msg);
    /// flush batched messages, call once per frame
    void FlushBatchedMessages();

protected:
    Threading::ThreadId mainThreadId;
    Ptr<Messaging::BatchMessage> batchMessage;
};

} // namespace Vibration
//------------------------------------------------------------------------------
