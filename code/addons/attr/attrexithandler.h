#pragma once
//------------------------------------------------------------------------------
/**
    @class Attr::AttrExitHandler
    
    Exit handler for the Attr subsystem, cleans up dynamic attribute
    definitions before shutdown.
    
    (C) 2008 Radon Labs
*/
#include "core/exithandler.h"

//------------------------------------------------------------------------------
namespace Attr
{
class AttrExitHandler : public Core::ExitHandler
{
public:
    /// called on application exit
    virtual void OnExit() const;
};

} // namespace Attr
//------------------------------------------------------------------------------
    