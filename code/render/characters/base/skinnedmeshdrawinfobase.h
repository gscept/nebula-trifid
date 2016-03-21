#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::SkinnedMeshDrawInfoBase
    
    Contains per-instance information for a skinned-mesh draw operation. 
    Add platform-specific attributes by deriving from this class.
    This is usually only used by platforms which use software
    skinning.
    
    (C) 2009 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "characters/characterinstance.h"
#include "coregraphics/mesh.h"

//------------------------------------------------------------------------------
namespace Base
{
struct SkinnedMeshDrawInfoBase
{
    Ptr<Characters::CharacterInstance> charInst;
    Ptr<CoreGraphics::Mesh> mesh;
};

} // namespace Base
//------------------------------------------------------------------------------
