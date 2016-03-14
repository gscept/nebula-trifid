//------------------------------------------------------------------------------
//  texturecreator.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "texturecreator.h"

#if __DX11__
namespace Resources
{
  __ImplementClass(Resources::TextureCreator, 'TXCT', Resources::D3D11TextureCreator);
}
#elif __DX9__
namespace Resources
{
  __ImplementClass(Resources::TextureCreator, 'TXCT', Resources::D3D9TextureCreator);
}
#elif __OGL4__
namespace Resources
{
  __ImplementClass(Resources::TextureCreator, 'TXCT', Resources::OGL4TextureCreator);
}
#else
#error "TextureCreator class not implemented on this platform!"
#endif