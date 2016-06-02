#pragma once
//------------------------------------------------------------------------------
/**
    @class PostEffect::SkyParams
    
    Holds all the relevant post effects SKY parameters
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "util/string.h"
#include "parambase.h"

//------------------------------------------------------------------------------
namespace PostEffect
{
class SkyParams : public ParamBase
{
    __DeclareClass(SkyParams);
public:
    /// constructor
    SkyParams();
    /// constructor
    virtual ~SkyParams();

	/// copies from one param to another
	virtual void Copy(const Ptr<ParamBase>& rhs);

    /// blend towards a target paramset
    virtual void BlendTo(const Ptr<ParamBase>& p, float l);

    /// comapre
    virtual bool Equals(const Ptr<ParamBase>& v2, float threshold) const;

    /// set texture blend factor
    void ResetTextureBlendFactor();
    /// get texture blend factor
    float GetTextureBlendFactor() const;

    /// set sky texture path
    void SetSkyTexturePath(const Util::String& p);
    /// get sky texture path
    const Util::String& GetSkyTexturePath() const;

    /// set default reflectance texture path
    void SetReflectanceTexturePath(const Util::String& p);
    /// get default reflectance texture path
    const Util::String& GetReflectanceTexturePath() const;

    /// set default irradiance texture path
    void SetIrradianceTexturePath(const Util::String& p);
    /// get default irradiance texture path
    const Util::String& GetIrradianceTexturePath() const;

    /// set sky contrast
    void SetSkyContrast(float f);
    /// get sky contrast
    float GetSkyContrast() const;

    /// set sky brightness
    void SetSkyBrightness(float f);
    /// get sky brightness
    float GetSkyBrightness() const;

    /// set sky rotation factor
    void SetSkyRotationFactor(float f);
    /// get sky rotation factor
    float GetSkyRotationFactor() const;

private:
    float textureBlendFactor;
    float skyContrast;
    float skyBrightness;
    float skyRotationFactor;
    Util::String skyTexture;
    Util::String reflTexture;
    Util::String irrTexture;
};

//------------------------------------------------------------------------------
/**
*/
inline
void
SkyParams::ResetTextureBlendFactor()
{
    this->textureBlendFactor = 0.0f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
SkyParams::GetTextureBlendFactor() const
{
    return this->textureBlendFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void 
SkyParams::SetSkyTexturePath(const Util::String& p)
{
    this->skyTexture = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
SkyParams::GetSkyTexturePath() const
{
    return this->skyTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
SkyParams::SetReflectanceTexturePath(const Util::String& p)
{
    this->reflTexture = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
SkyParams::GetReflectanceTexturePath() const
{
    return this->reflTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
SkyParams::SetIrradianceTexturePath(const Util::String& p)
{
    this->irrTexture = p;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
SkyParams::GetIrradianceTexturePath() const
{
    return this->irrTexture;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
SkyParams::SetSkyContrast(float f)
{
    this->skyContrast = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
SkyParams::GetSkyContrast() const
{
    return this->skyContrast;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
SkyParams::SetSkyBrightness(float f)
{
    this->skyBrightness = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
SkyParams::GetSkyBrightness() const
{
    return this->skyBrightness;
}
//------------------------------------------------------------------------------
/**
*/
inline
void
SkyParams::SetSkyRotationFactor(float f)
{
    this->skyRotationFactor = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
SkyParams::GetSkyRotationFactor() const
{
    return this->skyRotationFactor;
}
}
//------------------------------------------------------------------------------
