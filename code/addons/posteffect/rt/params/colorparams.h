#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::ColorParams
    
    Holds all the relevant post effects COLOR parameters
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "math/float4.h"
#include "parambase.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class ColorParams : public ParamBase
{
    __DeclareClass(ColorParams);
public:
    /// constructor
    ColorParams();
    /// constructor
    virtual ~ColorParams();

	/// copies from one param to another
	virtual void Copy(const Ptr<ParamBase>& rhs);

    /// blend towards a target paramset
    virtual void BlendTo(const Ptr<ParamBase>& p, float l);

    /// comapre
    virtual bool Equals(const Ptr<ParamBase>& v2, float threshold) const;

    /// set color saturation
    void SetColorSaturation(float f);
    /// get color saturation
    const float GetColorSaturation() const;
	/// set color luminance
	void SetColorMaxLuminance(const float f);
	/// get color luminance
	const float GetColorMaxLuminance() const;
    /// set color balance
    void SetColorBalance(const Math::float4& v);
    /// get color balance
    const Math::float4& GetColorBalance() const;

private:
    float colorSaturation;
    Math::float4 colorBalance;
	float colorMaxLuminance;
};

//------------------------------------------------------------------------------
/**
*/
inline void
ColorParams::SetColorSaturation(float f)
{
    this->colorSaturation = f;
}

//------------------------------------------------------------------------------
/**
*/
inline const float 
ColorParams::GetColorSaturation() const
{
    return this->colorSaturation;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ColorParams::SetColorMaxLuminance( const float f )
{
	this->colorMaxLuminance = f;
}

//------------------------------------------------------------------------------
/**
*/
inline const float
ColorParams::GetColorMaxLuminance() const
{
	return this->colorMaxLuminance;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
ColorParams::SetColorBalance(const Math::float4& v)
{
    this->colorBalance = v;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4& 
ColorParams::GetColorBalance() const
{
    return this->colorBalance;
}
}
//------------------------------------------------------------------------------
