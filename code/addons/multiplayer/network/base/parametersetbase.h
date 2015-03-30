#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::ParameterSetBase

    This base class should be overwritten by each application to specify the 
    parameters for game search, game host etc mechanism!
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Base
{
class ParameterSetBase : public Core::RefCounted
{
    __DeclareClass(ParameterSetBase);

public:
    /// constructor
    ParameterSetBase();
    /// destructor
    virtual ~ParameterSetBase();

    /// compare operator
    virtual bool IsEqual(const Ptr<Base::ParameterSetBase>& other) const;
};  
}; // namespace Base
//------------------------------------------------------------------------------