#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharJointComponents
    
    Packs the components of a character joint into an array
    (translation, rotation, scale, variationScale) to enable
    packing of joint components into an array.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "math/float4.h"
#include "math/quaternion.h"

//------------------------------------------------------------------------------
namespace Characters
{
struct CharJointComponents
{
    // NOTE: DO NOT MODIFY THE MEMBERS OF THIS STRUCTURE!
    CharJointComponents() :
        varTranslationX(0.0f),
        varTranslationY(0.0f),
        varTranslationZ(0.0f),
        varTranslationW(0.0f), // for correct alignment, not used
        varScaleX(1.0f),
        varScaleY(1.0f),
        varScaleZ(1.0f),        
        parentJointIndex(InvalidIndex)
        { };

    float varTranslationX, varTranslationY, varTranslationZ, varTranslationW;
    float varScaleX, varScaleY, varScaleZ;    
    int parentJointIndex;
};

} // namespace Characters
//------------------------------------------------------------------------------
