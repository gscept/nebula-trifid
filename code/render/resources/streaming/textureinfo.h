#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::TextureInfo

    TextureInfo is a platform wrapper for ResourceInfo classes specialized for textures like D3D9TextureInfo.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/

//------------------------------------------------------------------------------
#if __DX11__
#include "resources/d3d11streaming/d3d11textureinfo.h"
namespace Resources
{
	class TextureInfo : public Resources::D3D11TextureInfo
	{
	}; // namespace Resources
}
#elif __OGL4__
#include "resources/ogl4streaming/ogl4textureinfo.h"
namespace Resources
{
	class TextureInfo : public Resources::OGL4TextureInfo
	{
	}; // namespace Resources
}
#elif __VULKAN__
#include "resources/vkstreaming/vktextureinfo.h"
namespace Resources
{
	class TextureInfo : public Vulkan::VkTextureInfo
	{
	}; // namespace Resources
}
#elif __DX9__
#include "resources/d3d9streaming/d3d9textureinfo.h"
namespace Resources
{
	class TextureInfo : public Resources::D3D9TextureInfo
	{
	}; // namespace Resources
}
#else
#error "TextureInfo class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------