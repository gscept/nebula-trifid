#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::SkinningTechnique
    
    The various character skinning techniques supported by N3.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/types.h"

//------------------------------------------------------------------------------
namespace Characters
{
class SkinningTechnique
{
public:
    enum Code
    {
        InvalidSkinningTechnique,
        SoftwareSkinning,       // some sort of software-skinning
        GPUSkinning,            // skinning in vertex shader, joint palette in vertex constants
        GPUTextureSkinning,     // skinning in vertex shader, joint palette in vertex texture
		GPUSkinningFeedback,	// skinning once in vertex shader using joint palette, then reuse transform buffer to render color
    };
};

} // namespace Characters
//------------------------------------------------------------------------------
    