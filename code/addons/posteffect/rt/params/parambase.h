#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::ParamBase
    
    This is the parent class, for all posteffect parameter settings.
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "timing/time.h"
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class ParamBase : public Core::RefCounted
{
    __DeclareClass(ParamBase);
public:
    /// constructor
    ParamBase();
    /// deconstructor
    virtual ~ParamBase();

	/// copies from one param to another
	virtual void Copy(const Ptr<ParamBase>& rhs);

    /// blend towards a target paramset
    virtual void BlendTo(const Ptr<ParamBase>& p, float l);

    /// compare
    virtual bool Equals(const Ptr<ParamBase>& v2, float threshold) const;
};

}
//------------------------------------------------------------------------------
