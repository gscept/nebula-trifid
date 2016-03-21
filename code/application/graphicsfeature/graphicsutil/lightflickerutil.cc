//------------------------------------------------------------------------------
//  lightflickerutil.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicsutil/lightflickerutil.h"
#include "math/noise.h"
#include "basegametiming/gametimesource.h"
#include "math/scalar.h"

namespace GraphicsFeature
{
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
LightFlickerUtil::LightFlickerUtil() :
    enabled(false),
    frequency(5.0f),
    intensityAmplitude(0.5f),
    positionAmplitude(0.25f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void 
LightFlickerUtil::SetLightEntity( const Ptr<Graphics::AbstractLightEntity>& e )
{
    if (this->lightEntity.isvalid())
    {
        this->lightEntity->SetTransform(this->origTrans);
        this->lightEntity->SetColor(this->origColor);
    }
    this->lightEntity = e;
}

//------------------------------------------------------------------------------
/**
    This changes the transformation offset matrix and the intensity
    modulation of the light entity.
*/
void
LightFlickerUtil::Update()
{
    if (this->enabled)
    {
        n_assert(this->lightEntity.isvalid());
        float t = float(BaseGameFeature::GameTimeSource::Instance()->GetTime()) * this->frequency;
        
        // TODO:
        // get perlin noise 
        float n = noise::gen(t, 0.0f, 0.0f);
        float intensityModulate = 1.0f + (this->intensityAmplitude * n);
        intensityModulate = n_max(0.0f, intensityModulate);
        this->lightEntity->SetColor(this->origColor * intensityModulate);
        
        matrix44 m = this->origTrans;
        vector v(this->positionAmplitude * noise::gen(t, 1.1f, 0.0f), 
                  this->positionAmplitude * noise::gen(t, 2.2f, 0.0f), 
                  this->positionAmplitude * noise::gen(t, 3.3f, 0.0f));
        m.translate(v);
        this->lightEntity->SetTransform(m);
    }
}


} // namespace GraphicsFeature
