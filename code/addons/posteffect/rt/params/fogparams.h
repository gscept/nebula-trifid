#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::FogParams
    
    Holds all the relevant post effects FOG parameters
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "math/float4.h"
#include "parambase.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class FogParams : public ParamBase
{
    __DeclareClass(FogParams);

public:
    /// constructor
    FogParams();
    /// constructor
    virtual ~FogParams();

	/// copies from one param to another
	virtual void Copy(const Ptr<ParamBase>& rhs);

    /// blend towards a target paramset
    virtual void BlendTo(const Ptr<ParamBase>& p, float l);

    /// comapre
    virtual bool Equals(const Ptr<ParamBase>& v2, float threshold) const;

    /// set fog color (rgb) and intensity (a)
    void SetFogColorAndIntensity(const Math::float4& v);
    /// get fog color (rgb) and intensity (a)
    const Math::float4& GetFogColorAndIntensity() const;
    
    /// set fog near distance
    void SetFogNearDistance(float f);
    /// get fog near distance
    float GetFogNearDistance() const;
    
    /// set fog far distance
    void SetFogFarDistance(float f);
    /// get fog far distance
    float GetFogFarDistance() const;
    
    /// set max fog height above ground
    void SetFogHeight(float f);
    /// get max fog height above ground
    float GetFogHeight() const;

private:
    float fogNearDist;
    float fogFarDist;
    float fogHeight;
    Math::float4 fogColor;
};

//------------------------------------------------------------------------------
/**
*/
inline
void 
FogParams::SetFogColorAndIntensity(const Math::float4& v)
{
    this->fogColor = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::float4& 
FogParams::GetFogColorAndIntensity() const
{
    return this->fogColor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FogParams::SetFogNearDistance(float f)
{
    this->fogNearDist = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float 
FogParams::GetFogNearDistance() const
{
    return this->fogNearDist;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FogParams::SetFogFarDistance(float f)
{
    this->fogFarDist = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float 
FogParams::GetFogFarDistance() const
{
    return this->fogFarDist;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
FogParams::SetFogHeight(float f)
{
    this->fogHeight = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float 
FogParams::GetFogHeight() const
{
    return this->fogHeight;
}
}
//------------------------------------------------------------------------------
