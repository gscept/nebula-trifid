//------------------------------------------------------------------------------
//  skyparams.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "skyparams.h"

namespace PostEffect
{
__ImplementClass(PostEffect::SkyParams, 'PESP', PostEffect::ParamBase);

using namespace Math;

//------------------------------------------------------------------------------
/**
*/
SkyParams::SkyParams() :
    textureBlendFactor(0.0f),
    skyContrast(1.0f),
    skyBrightness(1.0f),
	skyTexture("tex:system/sky")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
SkyParams::~SkyParams()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
SkyParams::Copy( const Ptr<ParamBase>& rhs )
{
	n_assert(rhs->IsA(SkyParams::RTTI));

	if (this != rhs.get())
	{
		this->textureBlendFactor = rhs.cast<SkyParams>()->textureBlendFactor;
		this->skyTexture = rhs.cast<SkyParams>()->skyTexture;
		this->skyContrast = rhs.cast<SkyParams>()->skyContrast;
		this->skyBrightness = rhs.cast<SkyParams>()->skyBrightness;
	}
	ParamBase::Copy(rhs);
}

//------------------------------------------------------------------------------
/**
    NOTE: this will not change the name of the sky texture!
*/
void
SkyParams::BlendTo(const Ptr<ParamBase>& p, float l)
{
    n_assert(p->IsA(SkyParams::RTTI));

    this->textureBlendFactor    = n_lerp(this->textureBlendFactor, 1.0f, l);
    this->skyContrast           = n_lerp(this->skyContrast, p.cast<SkyParams>()->skyContrast, l);
    this->skyBrightness         = n_lerp(this->skyBrightness, p.cast<SkyParams>()->skyBrightness, l);
}

//------------------------------------------------------------------------------
/**
    Compares NOT the sky texture string
*/
bool
SkyParams::Equals(const Ptr<ParamBase>& v2, float threshold) const
{
    n_assert(v2->IsA(SkyParams::RTTI));
    const Ptr<SkyParams> temp = v2.cast<SkyParams>();

    // special case, return true, if they are the same
    if(v2.get() == this)
    {
        return true;
    }

    if( Math::n_abs(temp->GetSkyBrightness() - this->GetSkyBrightness()) < threshold &&
        Math::n_abs(temp->GetSkyContrast() - this->GetSkyContrast()) < threshold &&
        Math::n_abs(1.0f - this->GetTextureBlendFactor()) < threshold)
    {
        return true;
    }
    return false;
}
} // namespace PostEffects
