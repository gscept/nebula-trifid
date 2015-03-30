#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::SkinnedMeshRenderer
    
    Wrapper class for platform-specific skinned mesh rendering.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#if ( __WIN32__ || __LINUX__ )
#include "characters/gpuskinnedmeshrenderer.h"
namespace Characters
{
class SkinnedMeshRenderer : public Characters::GPUSkinnedMeshRenderer
{
    __DeclareClass(SkinnedMeshRenderer);
    __DeclareSingleton(SkinnedMeshRenderer);
public:
    /// constructor
    SkinnedMeshRenderer();
    /// destructor
    virtual ~SkinnedMeshRenderer();
};
}
#else
#error "SkinnedMeshRenderer class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------
    