#pragma once
//------------------------------------------------------------------------------
/*
    @class Resources::TexturCreator

    A specialized creator for textures.

    (C) 2010 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
    */

#if __DX11__
#include "resources/d3d11streaming/d3d11texturecreator.h"
namespace Resources
{
class TextureCreator : public Resources::D3D11TextureCreator
{
    __DeclareClass(TextureCreator);
};
} 
#elif __DX9__
#include "resources/d3d9streaming/d3d9texturecreator.h"
namespace Resources
{
class TextureCreator : public Resources::D3D9TextureCreator
{
    __DeclareClass(TextureCreator);
};
} 
#elif __OGL4__
#include "resources/ogl4streaming/ogl4texturecreator.h"
namespace Resources
{
class TextureCreator : public Resources::OGL4TextureCreator
{
    __DeclareClass(TextureCreator);
};
} // namespace Resources

#else
#error "TextureCreator class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------