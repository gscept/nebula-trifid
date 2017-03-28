#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::GraphicsEntityType
    
    Defines graphics entity types.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/types.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class GraphicsEntityType
{
public:
    /// enumeration
    enum Code
    {
        Model = 0,
		Tree,
        Light,
		LightProbe,
        Camera,

        NumTypes,
        InvalidType,
    };
};

} // namespace Graphics
//------------------------------------------------------------------------------
    