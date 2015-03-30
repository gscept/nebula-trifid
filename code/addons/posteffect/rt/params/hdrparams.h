#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::HdrParams
    
    Holds all the relevant post effects HDR parameters
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "math/float4.h"
#include "parambase.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class HdrParams : public ParamBase
{
    __DeclareClass(HdrParams);
public:
    /// constructor
    HdrParams();
    /// constructor
    virtual ~HdrParams();

	/// copies from one param to another
	virtual void Copy(const Ptr<ParamBase>& rhs);

    /// blend towards a target paramset
    virtual void BlendTo(const Ptr<ParamBase>& p, float l);

    /// comapre
    virtual bool Equals(const Ptr<ParamBase>& v2, float threshold) const;

    /// set hdr bloom intensity
    void SetHdrBloomIntensity(float f);
    /// get hdr bloom intensity
    float GetHdrBloomIntensity() const;
    /// set hdr bloom color
    void SetHdrBloomColor(const Math::float4& v);
    /// get hdr bloom color
    const Math::float4& GetHdrBloomColor() const;
    /// set hdr bloom threshold
    void SetHdrBloomThreshold(float f);
    /// get hdr bloom threshold
    float GetHdrBloomThreshold() const;

private:
    float hdrBloomIntensity;
    float hdrBloomThreshold;
    Math::float4 hdrBloomColor;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
HdrParams::SetHdrBloomIntensity(float f)
{
    this->hdrBloomIntensity = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float 
HdrParams::GetHdrBloomIntensity() const
{
    return this->hdrBloomIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
HdrParams::SetHdrBloomColor(const Math::float4& v)
{
    this->hdrBloomColor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::float4&
HdrParams::GetHdrBloomColor() const
{
    return this->hdrBloomColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
HdrParams::SetHdrBloomThreshold(float f)
{
    this->hdrBloomThreshold = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float 
HdrParams::GetHdrBloomThreshold() const
{
    return this->hdrBloomThreshold;
}
}
//------------------------------------------------------------------------------
