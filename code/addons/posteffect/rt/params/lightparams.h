#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::LightParams
    
    Holds all the relevant post effects light parameters 
    
    (C) 2006 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "math/matrix44.h"
#include "parambase.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class LightParams : public ParamBase
{
    __DeclareClass(LightParams);
public:
    /// constructor
    LightParams();
    /// constructor
    virtual ~LightParams();

	/// copies from one param to another
	virtual void Copy(const Ptr<ParamBase>& rhs);

    /// blend towards a target paramset
    virtual void BlendTo(const Ptr<ParamBase>& p, float l);

    /// comapre
    virtual bool Equals(const Ptr<ParamBase>& v2, float threshold) const;

    /// set light directional matrix
    void SetLightTransform(const Math::matrix44& m);
    /// get light directional matrix
    const Math::matrix44& GetLightTransform() const;
 
    /// set light color
    void SetLightColor(const Math::float4& v);
    /// get light color
    const Math::float4& GetLightColor() const;
    
    /// set opposite light color
    void SetLightOppositeColor(const Math::float4& v);
    /// get opposite light color
    const Math::float4& GetLightOppositeColor() const;
    /// set light ambient color
    void SetLightAmbientColor(const Math::float4& v);
    /// get light ambient color
    const Math::float4& GetLightAmbientColor() const;
    
    /// set light shadow intensity
    void SetLightShadowIntensity(float f);
    /// get light shadow intensity
    float GetLightShadowIntensity() const;
	/// set if the light should cast shadows
	void SetLightCastShadows(bool b);
	/// get if light casts shadows
	const bool GetLightCastsShadows() const;
	/// set the light shadow bias
	void SetLightShadowBias(float f);
	/// get light shadow bias
	const float GetLightShadowBias() const;
    /// set light intensity
    void SetLightIntensity(float f);
    /// get light intensity
    const float GetLightIntensity() const;

    /// set factor for back light color
    void SetBackLightFactor(float f);
    /// get factor for back light color
    float GetBackLightFactor() const;

private:
    Math::matrix44 lightTransform;
    float backLightOffset;
    float lightShadowIntensity;
	float lightShadowBias;
    float lightIntensity;
	bool lightCastsShadows;
    Math::float4 lightColor;
    Math::float4 lightOppositeColor;
    Math::float4 lightAmbientColor;
};

//------------------------------------------------------------------------------
/**
*/
inline void
LightParams::SetLightTransform(const Math::matrix44& m)
{
    this->lightTransform = m;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
LightParams::GetLightTransform() const
{
    return this->lightTransform;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
LightParams::SetLightColor(const Math::float4& v)
{
    this->lightColor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4& 
LightParams::GetLightColor() const
{
    return this->lightColor;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
LightParams::SetLightOppositeColor(const Math::float4& v)
{
    this->lightOppositeColor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4&
LightParams::GetLightOppositeColor() const
{
    return this->lightOppositeColor;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
LightParams::SetLightAmbientColor(const Math::float4& v)
{
    this->lightAmbientColor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4& 
LightParams::GetLightAmbientColor() const
{
    return this->lightAmbientColor;
}

//------------------------------------------------------------------------------
/**
*/
inline void
LightParams::SetLightShadowIntensity(float f)
{
    this->lightShadowIntensity = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float
LightParams::GetLightShadowIntensity() const
{
    return this->lightShadowIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
LightParams::SetLightShadowBias( float f )
{
	this->lightShadowBias = f;
}

//------------------------------------------------------------------------------
/**
*/
inline const float 
LightParams::GetLightShadowBias() const
{
	return this->lightShadowBias;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
LightParams::SetLightIntensity( float f )
{
    this->lightIntensity = f;
}

//------------------------------------------------------------------------------
/**
*/
inline const float 
LightParams::GetLightIntensity() const
{
    return this->lightIntensity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
LightParams::SetLightCastShadows( bool b )
{
	this->lightCastsShadows = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
LightParams::GetLightCastsShadows() const
{
	return this->lightCastsShadows;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
LightParams::SetBackLightFactor(float f)
{
    this->backLightOffset = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
LightParams::GetBackLightFactor() const
{
    return this->backLightOffset;
}

}
//------------------------------------------------------------------------------
