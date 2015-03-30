#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9StreamShaderLoader
    
    D3D9 implementation of StreamShaderLoader.
    
    (C) 2007 Radon Labs GmbH
*/
#include "resources/streamresourceloader.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9StreamShaderLoader : public Resources::StreamResourceLoader
{
    __DeclareClass(D3D9StreamShaderLoader);
public:
    /// return true if asynchronous loading is supported
    virtual bool CanLoadAsync() const;
    
private:
    /// setup the shader from a Nebula3 stream
    virtual bool SetupResourceFromStream(const Ptr<IO::Stream>& stream);
};

} // namespace Direct3D9
//------------------------------------------------------------------------------
    