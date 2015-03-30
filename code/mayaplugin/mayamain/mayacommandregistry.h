#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaCommandRegistry
    
    Register command objects with the Maya plugin.
    
    (C) 2009 Radon Labs GmbH
*/
#include "core/refcounted.h"
#include "core/singleton.h"

class MFnPlugin;
//------------------------------------------------------------------------------
namespace Maya
{
class MayaCommandRegistry : public Core::RefCounted
{
    __DeclareClass(MayaCommandRegistry);
    __DeclareSingleton(MayaCommandRegistry);
public:
    /// constructor
    MayaCommandRegistry();
    /// destructor
    virtual ~MayaCommandRegistry();
    
    /// setup the command registry (registers commands)
    void Setup(MFnPlugin& fnPlugin);
    /// discard the command registry (unregisters commands)
    void Discard(MFnPlugin& fnPlugin);
    /// return true if object is valid
    bool IsValid() const;
    
private:
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaCommandRegistry::IsValid() const
{
    return this->isValid;
}

} // namespace Maya
//------------------------------------------------------------------------------
    