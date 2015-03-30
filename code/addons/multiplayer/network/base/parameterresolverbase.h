#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::ParameterResolverBase

    This class is the base class for using session parameter
    (Like game mode, etc).

    At first supply an API specific implementation, then add a application specific
    ParameterSetBase subclass impelemtation.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "network/base/parametersetbase.h"
#include "util/dictionary.h"
#include "util/stringatom.h"

//------------------------------------------------------------------------------
namespace Base
{
class ParameterResolverBase : public Core::RefCounted
{
    __DeclareClass(ParameterResolverBase);

public:
    /// constructor
    ParameterResolverBase();
    /// destructor
    virtual ~ParameterResolverBase();

    /// set parameter set from godsend
    void SetParameterSet(const Ptr<Base::ParameterSetBase>& paraSet);
    /// get the parameter set from godsend
    const Ptr<Base::ParameterSetBase>& GetParameterSet() const;

    /// compare data to 
    bool CompareParameterSet(const Ptr<ParameterResolverBase>& other);

protected:
    Ptr<Base::ParameterSetBase> parameterSet;
}; 
//------------------------------------------------------------------------------
/**
*/
inline void 
ParameterResolverBase::SetParameterSet( const Ptr<Base::ParameterSetBase>& paraSet )
{
    this->parameterSet = paraSet;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Base::ParameterSetBase>& 
ParameterResolverBase::GetParameterSet() const
{
    return this->parameterSet;
}
}; // namespace Base
//------------------------------------------------------------------------------