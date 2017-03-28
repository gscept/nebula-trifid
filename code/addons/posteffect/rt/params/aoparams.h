#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::AoParams
    
    Controls AO parameters through a post effect entity.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "parambase.h"
namespace PostEffect
{
class AoParams : public ParamBase
{
	__DeclareClass(AoParams);
public:
	/// constructor
	AoParams();
	/// destructor
	virtual ~AoParams();

	/// copies from one param to another
	virtual void Copy(const Ptr<ParamBase>& rhs);

    /// blend towards a target paramset
    virtual void BlendTo(const Ptr<ParamBase>& p, float l);

    /// compare
    virtual bool Equals(const Ptr<ParamBase>& v2, float threshold) const;

    /// set strength
    void SetStrength(float f);
    /// get strength
    float GetStrength() const;
    /// set angle bias
    void SetAngleBias(float f);
    /// get angle bias
    float GetAngleBias() const;
    /// set power
    void SetPower(float f);
    /// get power
    float GetPower() const;
    /// set radius
    void SetRadius(float f);
    /// get radius
    float GetRadius() const;

private:
    float strength;
    float angleBias;
    float power;
    float radius;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
AoParams::SetStrength( float f )
{
    this->strength = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
AoParams::GetStrength() const
{
    return this->strength;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AoParams::SetAngleBias( float f )
{
    this->angleBias = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
AoParams::GetAngleBias() const
{
    return this->angleBias;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AoParams::SetPower( float f )
{
    this->power = f;
}  

//------------------------------------------------------------------------------
/**
*/
inline float 
AoParams::GetPower() const
{
    return this->power;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AoParams::SetRadius( float f )
{
    this->radius = f;
}

//------------------------------------------------------------------------------
/**
*/
inline float 
AoParams::GetRadius() const
{
    return this->radius;
}
} // namespace PostEffect
//------------------------------------------------------------------------------