#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9StreamTextureLoader
  
    D3D9/Xbox360 implementation of StreamTextureLoader.

    (C) 2007 Radon Labs GmbH
*/    
#include "resources/streamresourceloader.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9StreamTextureLoader : public Resources::StreamResourceLoader
{
    __DeclareClass(D3D9StreamTextureLoader);
private:
    /// setup the texture from a Nebula3 stream
    virtual bool SetupResourceFromStream(const Ptr<IO::Stream>& stream);
};

} // namespace Direct3D9
//------------------------------------------------------------------------------
