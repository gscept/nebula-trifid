#pragma once
#ifndef REMOTE_REMOTEINTERFACE_H
#define REMOTE_REMOTEINTERFACE_H
//------------------------------------------------------------------------------
/**
    @class Remote::RemoteInterface
    
    The RemoteInterface launches the RemoteControl thread and is the communication
    interface with the RemoteControl thread.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "interface/interfacebase.h"
#include "core/singleton.h"

//------------------------------------------------------------------------------
namespace Remote
{
class RemoteInterface : public Interface::InterfaceBase
{
    __DeclareClass(RemoteInterface);
    __DeclareInterfaceSingleton(RemoteInterface);
public:
    /// constructor
    RemoteInterface();
    /// destructor
    virtual ~RemoteInterface();
    /// open the interface object
    virtual void Open();
    /// set port number
    virtual void SetPortNumber(ushort portNumber);
    /// get port number
    virtual ushort GetPortNumber() const;
    
private:
    ushort portNum;
};

//------------------------------------------------------------------------------
/**	
*/
inline void
RemoteInterface::SetPortNumber(ushort portNumber)
{
    n_assert(!this->IsOpen());
    this->portNum = portNumber;
}

//------------------------------------------------------------------------------
/**	
*/
inline ushort
RemoteInterface::GetPortNumber() const
{
    return this->portNum;
}

} // namespace Remote
//------------------------------------------------------------------------------
#endif