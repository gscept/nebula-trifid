//------------------------------------------------------------------------------
//  mayavariableregistry.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mayavariableregistry.h"
#include "util/string.h"

namespace Maya
{
__ImplementClass(Maya::MayaVariableRegistry, 'MVRG', Core::RefCounted);
__ImplementSingleton(Maya::MayaVariableRegistry);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
MayaVariableRegistry::MayaVariableRegistry() :
    isValid(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
MayaVariableRegistry::~MayaVariableRegistry()
{
    if (this->IsValid())
    {
        this->Discard();
    }
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaVariableRegistry::Setup()
{
    n_assert(!this->IsValid());
    this->isValid = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MayaVariableRegistry::Discard()
{
    n_assert(this->IsValid());
    this->isValid = false;
    this->stringVariables.Clear();
    this->boolVariables.Clear();
}

//------------------------------------------------------------------------------
/**
*/
template<> void
MayaVariableRegistry::Set<String>(MayaVariable::Code var, String str)
{
    if (this->stringVariables.Contains(var))
    {
        this->stringVariables[var] = str;
    }
    else
    {
        this->stringVariables.Add(var, str);
    }
}

//------------------------------------------------------------------------------
/**
*/
template<> String
MayaVariableRegistry::Get<String>(MayaVariable::Code var) const
{
    return this->stringVariables[var];
}

//------------------------------------------------------------------------------
/**
*/
template<> bool
MayaVariableRegistry::Exists<String>(MayaVariable::Code var) const
{
    return this->stringVariables.Contains(var);
}

//------------------------------------------------------------------------------
/**
*/
template<> void
MayaVariableRegistry::Set<bool>(MayaVariable::Code var, bool b)
{
    if (this->boolVariables.Contains(var))
    {
        this->boolVariables[var] = b;
    }
    else
    {
        this->boolVariables.Add(var, b);
    }
}

//------------------------------------------------------------------------------
/**
*/
template<> bool
MayaVariableRegistry::Get<bool>(MayaVariable::Code var) const
{
    return this->boolVariables[var];
}

//------------------------------------------------------------------------------
/**
*/
template<> bool
MayaVariableRegistry::Exists<bool>(MayaVariable::Code var) const
{
    return this->boolVariables.Contains(var);
}

} // namespace Maya