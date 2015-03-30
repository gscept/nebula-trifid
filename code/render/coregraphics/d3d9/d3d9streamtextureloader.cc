//------------------------------------------------------------------------------
//  d3d9streamtextureloader.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/d3d9/d3d9streamtextureloader.h"
#include "coregraphics/texture.h"
#include "coregraphics/renderdevice.h"
#include "io/ioserver.h"

namespace Direct3D9
{
__ImplementClass(Direct3D9::D3D9StreamTextureLoader, 'D9TL', Resources::StreamResourceLoader);

using namespace CoreGraphics;
using namespace Resources;
using namespace IO;

//------------------------------------------------------------------------------
/**
    This method actually setups the Texture object from the data in the stream.
*/
bool
D3D9StreamTextureLoader::SetupResourceFromStream(const Ptr<Stream>& stream)
{
    n_assert(stream.isvalid());
    n_assert(stream->CanBeMapped());
    HRESULT hr;
    IDirect3DDevice9* d3d9Device = RenderDevice::Instance()->GetDirect3DDevice();
    n_assert(0 != d3d9Device);
    n_assert(this->resource->IsA(Texture::RTTI));
    const Ptr<Texture>& res = this->resource.downcast<Texture>();
    n_assert(!res->IsLoaded());

    stream->SetAccessMode(Stream::ReadAccess);
    if (stream->Open())
    {
        void* srcData = stream->Map();
        UINT srcDataSize = stream->GetSize();

        // first need to check image info whether to determine texture type
        D3DXIMAGE_INFO imageInfo = { 0 };
        hr = D3DXGetImageInfoFromFileInMemory(srcData, srcDataSize, &imageInfo);
        if (FAILED(hr))
        {
            n_error("D3D9StreamTextureLoader: failed to obtain image info from file '%s'!", this->resource->GetResourceId().Value());
            return false;
        }

        // load texture based on texture type
        if (D3DRTYPE_TEXTURE == imageInfo.ResourceType)
        {
            // mipmap usage test enabled? -> DEBUG ONLY!
            Ptr<Stream> mipMapStream;
            bool visualizeMipMaps = RenderDevice::Instance()->GetVisualizeMipMaps();
            if (visualizeMipMaps)
            {
                if ((imageInfo.Width == imageInfo.Height) &&
                    (imageInfo.Width >= 128) && (imageInfo.Width <= 2048) &&
                    ((imageInfo.Format == D3DFMT_DXT1) || (imageInfo.Format == D3DFMT_DXT3)) &&
                    (!Util::String::MatchPattern(stream->GetURI().AsString(), "*dx9*")) &&
                    (!Util::String::MatchPattern(stream->GetURI().AsString(), "*lightmap*")))
                {
                    // overwrite texture data with mipmap test texture
                    Util::String mipTestFilename;
                    mipTestFilename.Format("systex:system/miptest_%d.dds", (imageInfo.Width > imageInfo.Height) ? imageInfo.Width : imageInfo.Height);
                    // create stream
                    mipMapStream = IoServer::Instance()->CreateStream(IO::URI(mipTestFilename));
                    mipMapStream->SetAccessMode(Stream::ReadAccess);
                    mipMapStream->Open();
                    // overwrite data ptr
                    srcData = mipMapStream->Map();
                }
            }
            // load 2D texture
            IDirect3DTexture9* d3d9Texture = 0;
            hr = D3DXCreateTextureFromFileInMemory(d3d9Device, srcData, srcDataSize, &d3d9Texture);
            if (FAILED(hr))
            {
                n_error("D3D9StreamTextureLoader: D3DXCreateTextureFromFileInMemory() failed for file '%s'!", this->resource->GetResourceId().Value());
                return false;
            }
            res->SetupFromD3D9Texture(d3d9Texture);

            if (mipMapStream.isvalid() && visualizeMipMaps)
            {
                mipMapStream->Unmap();
                mipMapStream->Close();
            }
        }
        else if (D3DRTYPE_VOLUMETEXTURE == imageInfo.ResourceType)
        {
            // load 3D texture
            IDirect3DVolumeTexture9* d3d9VolumeTexture = 0;
            hr = D3DXCreateVolumeTextureFromFileInMemory(d3d9Device, srcData, srcDataSize, &d3d9VolumeTexture);
            if (FAILED(hr))
            {
                n_error("D3D9StreamTextureLoader: D3DXCreateVolumeTextureFromFileInMemory() failed for file '%s'!", this->resource->GetResourceId().Value());
                return false;
            }
            res->SetupFromD3D9VolumeTexture(d3d9VolumeTexture);
        }
        else if (D3DRTYPE_CUBETEXTURE == imageInfo.ResourceType)
        {
            // load cube texture
            IDirect3DCubeTexture9* d3d9CubeTexture = 0;
            hr = D3DXCreateCubeTextureFromFileInMemory(d3d9Device, srcData, srcDataSize, &d3d9CubeTexture);
            if (FAILED(hr))
            {
                n_error("D3D9StreamTextureLoader: D3DXCreateCubeTextureFromFileInMemory() failed for file '%s'!", this->resource->GetResourceId().Value());
                return false;
            }
            res->SetupFromD3D9CubeTexture(d3d9CubeTexture);
        }
        stream->Unmap();
        stream->Close();
        return true;
    }
    return false;
}

} // namespace Direct3D9
