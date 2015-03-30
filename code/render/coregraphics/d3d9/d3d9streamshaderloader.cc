//------------------------------------------------------------------------------
//  d3d9streamshaderloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/d3d9/d3d9streamshaderloader.h"
#include "coregraphics/d3d9/d3d9shader.h"
#include "coregraphics/d3d9/d3d9renderdevice.h"
#include "coregraphics/d3d9/d3d9shaderserver.h"
#include "io/ioserver.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9StreamShaderLoader, 'D9SL', Resources::StreamResourceLoader);

using namespace Resources;
using namespace CoreGraphics;
using namespace IO;

//------------------------------------------------------------------------------
/**
*/
bool
D3D9StreamShaderLoader::CanLoadAsync() const
{
    // no asynchronous loading supported for shader
    return false;
}

//------------------------------------------------------------------------------
/**
    Loads a precompiled shader files from a stream into a D3DXEffect
    object.
*/
bool
D3D9StreamShaderLoader::SetupResourceFromStream(const Ptr<Stream>& stream)
{
    n_assert(stream.isvalid());
    n_assert(stream->CanBeMapped());
    IDirect3DDevice9* d3d9Device = D3D9RenderDevice::Instance()->GetDirect3DDevice();
    n_assert(0 != d3d9Device);
    n_assert(this->resource->IsA(D3D9Shader::RTTI));
    const Ptr<D3D9Shader>& res = this->resource.downcast<D3D9Shader>();
    n_assert(!res->IsLoaded());
    
    // map stream to memory
    stream->SetAccessMode(Stream::ReadAccess);
    if (stream->Open())
    {
        void* srcData = stream->Map();
        UINT srcDataSize = stream->GetSize();

        // get the effect pool from the shader server (contains shared shader parameters)
        ID3DXEffectPool* effectPool = D3D9ShaderServer::Instance()->GetD3D9EffectPool();
        n_assert(0 != effectPool);

        // create the effect
        ID3DXEffect* d3d9Effect = 0;
        HRESULT hr = D3DXCreateEffect(d3d9Device,       // pDevice
                                      srcData,          // pSrcData
                                      srcDataSize,      // SrcDataLen
                                      NULL,             // pDefines
                                      NULL,             // pInclude
                                      0,                // Flags
                                      effectPool,       // pPool
                                      &d3d9Effect,      // ppEffect
                                      0);               // ppCompilationErrors
        stream->Unmap();
        stream->Close();

        // check for failure
        if (FAILED(hr))
        {
            n_error("D3D9StreamShaderLoader: failed to load shader '%s'!", 
                res->GetResourceId().Value());
            return false;
        }
        
        // success, setup our resource object
        n_assert(0 != d3d9Effect);
        res->SetD3D9Effect(d3d9Effect);
        return true;
    }
    return false;
}

} // namespace Direct3D9
