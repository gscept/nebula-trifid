#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::ShaderInstance
    
    A shader instance object is created from a shader and contains a local copy
    of the original shader state which can be modified through ShaderVariable
    objects. Shader instance objects are created directly through the 
    shader server.

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#if __DX11__
#include "coregraphics/d3d11/d3d11shaderinstance.h"
namespace CoreGraphics
{
class ShaderInstance : public Direct3D11::D3D11ShaderInstance
{
	__DeclareClass(ShaderInstance);
};
}
#elif __OGL4__
#include "coregraphics/ogl4/ogl4shaderinstance.h"
namespace CoreGraphics
{
class ShaderInstance : public OpenGL4::OGL4ShaderInstance
{
	__DeclareClass(ShaderInstance);
};
}
#elif __DX9__
#include "coregraphics/d3d9/d3d9shaderinstance.h"
namespace CoreGraphics
{
class ShaderInstance : public Direct3D9::D3D9ShaderInstance
{
    __DeclareClass(ShaderInstance);
};
}
#else
#error "ShaderInstance class not implemented on this platform!"
#endif
//------------------------------------------------------------------------------

