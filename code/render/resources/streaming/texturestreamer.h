#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::TextureStreamer
  
    Resource loader for loading texture data from a Nebula3 stream. Supports
    synchronous and asynchronous loading. Platform Wrapper.
    
    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/


#if __DX11__
#include "resources/d3d11streaming/d3d11texturestreamer.h"
namespace Resources
{
class TextureStreamer : public Resources::D3D11TextureStreamer
{
    __DeclareClass(TextureStreamer);
};
} // namespace Resources
#elif __DX9__
#include "resources/d3d9streaming/d3d9texturestreamer.h"
namespace Resources
{
class TextureStreamer : public Resources::D3D9TextureStreamer
{
    __DeclareClass(TextureStreamer);
};
} // namespace Resources
#elif __OGL4__
#include "resources/ogl4streaming/ogl4texturestreamer.h"
namespace Resources
{
class TextureStreamer : public Resources::OGL4TextureStreamer
{
    __DeclareClass(TextureStreamer);
};
} // namespace Resources
#else
#error "TextureStreamer class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------