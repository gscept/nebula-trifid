#pragma once
//------------------------------------------------------------------------------
/**
    @class OpenGL4::OGL4StreamTextureLoader
  
    OGL4 implementation of StreamTextureLoader.

    (C) 2007 Radon Labs GmbH
*/    
#include "resources/streamresourceloader.h"

//------------------------------------------------------------------------------
namespace OpenGL4
{
class OGL4StreamTextureLoader : public Resources::StreamResourceLoader
{
    __DeclareClass(OGL4StreamTextureLoader);
private:
    /// setup the texture from a Nebula3 stream
    virtual bool SetupResourceFromStream(const Ptr<IO::Stream>& stream);
};

} // namespace OpenGL4
//------------------------------------------------------------------------------