#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaNebula3Runtime
  
    Sets up the Nebula3 runtime for the Maya plugin. This is the first
    object that's setup from MayaPlugin.
    
    (C) 2009 Radon Labs GmbH
*/    
#include "core/refcounted.h"
#include "core/singleton.h"
#include "core/coreserver.h"
#include "io/ioserver.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaNebula3Runtime : public Core::RefCounted
{
    __DeclareClass(MayaNebula3Runtime);
    __DeclareSingleton(MayaNebula3Runtime);
public:
    /// constructor
    MayaNebula3Runtime();
    /// destructor
    virtual ~MayaNebula3Runtime();
    
    /// setup the Nebula3 runtime
    void Setup();
    /// discard the Nebula3 runtime
    void Discard();
    /// return true if valid
    bool IsValid() const;

private:
    Ptr<Core::CoreServer> coreServer;
    Ptr<IO::IoServer> ioServer;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaNebula3Runtime::IsValid() const
{
    return this->isValid;
}

} // namespace Maya
//------------------------------------------------------------------------------

