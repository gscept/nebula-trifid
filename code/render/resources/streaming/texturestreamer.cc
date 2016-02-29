//------------------------------------------------------------------------------
//  texturestreamer.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "texturestreamer.h"

namespace Resources
{    
#if __DX11__
__ImplementClass(Resources::TextureStreamer, 'TXST', Resources::D3D11TextureStreamer);
#elif __DX9__
__ImplementClass(Resources::TextureStreamer, 'TXST', Resources::D3D9TextureStreamer);
#elif __OGL4__
__ImplementClass(Resources::TextureStreamer, 'TXST', Resources::OGL4TextureStreamer);
#elif __VULKAN__
__ImplementClass(Resources::TextureStreamer, 'TXST', Vulkan::VkTextureStreamer);
#else
#error "TextureStreamer class not implemented on this platform!"
#endif 
}