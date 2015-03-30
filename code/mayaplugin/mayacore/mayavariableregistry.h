#pragma once
//------------------------------------------------------------------------------
/**
    @class Maya::MayaVariablesRegistry
  
    A central registry for option and settings varaibles.
    
    (C) 2009 Radon Labs GmbH
*/    
#include "core/refcounted.h"
#include "core/singleton.h"
#include "mayacore/mayavariable.h"

//------------------------------------------------------------------------------
namespace Maya
{
class MayaVariableRegistry : public Core::RefCounted
{
    __DeclareClass(MayaVariableRegistry);
    __DeclareSingleton(MayaVariableRegistry);
public:
    /// constructor
    MayaVariableRegistry();
    /// destructor
    virtual ~MayaVariableRegistry();
    
    /// setup the variable registry
    void Setup();
    /// discard the variable registry
    void Discard();
    /// return true if variable registry is valid
    bool IsValid() const;
    
    /// set a variable value
    template<class TYPE> void Set(MayaVariable::Code var, TYPE value);
    /// test if variable exists
    template<class TYPE> bool Exists(MayaVariable::Code var) const;
    /// get a variable string value
    template<class TYPE> TYPE Get(MayaVariable::Code var) const;

private:
    Util::Dictionary<MayaVariable::Code, Util::String> stringVariables;
    Util::Dictionary<MayaVariable::Code, bool> boolVariables;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
MayaVariableRegistry::IsValid() const
{
    return this->isValid;
}

} // namespace Maya
//------------------------------------------------------------------------------
