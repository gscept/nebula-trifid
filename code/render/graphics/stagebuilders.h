#pragma once
//------------------------------------------------------------------------------
/**
    @file graphics/stagebuilders.h
    
    Frontend typedefs to Graphics::StageBuilder classes.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "graphics/stagebuilder.h"
#include "graphics/simplestagebuilder.h"
#include "graphics/quadtreestagebuilder.h"

//------------------------------------------------------------------------------
namespace Graphics
{
typedef Graphics::StageBuilder StageBuilder;
typedef Graphics::SimpleStageBuilder SimpleStageBuilder;
typedef Graphics::QuadtreeStageBuilder QuadtreeStageBuilder;
}
//------------------------------------------------------------------------------

    