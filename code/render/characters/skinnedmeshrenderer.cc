//------------------------------------------------------------------------------
//  skinnedmeshrenderer.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "characters/skinnedmeshrenderer.h"

namespace Characters
{
#if (__WIN32__ || __LINUX__ )
__ImplementClass(Characters::SkinnedMeshRenderer, 'SMRR', Characters::GPUSkinnedMeshRenderer);
#else
#error "SkinnedMeshRenderer class not implemented on this platform!"
#endif

__ImplementSingleton(Characters::SkinnedMeshRenderer);

//------------------------------------------------------------------------------
/**
*/
SkinnedMeshRenderer::SkinnedMeshRenderer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
    __DestructSingleton;
}

} // namespace Characters
