#pragma once
//------------------------------------------------------------------------------
/**
    @class Direct3D9::D3D9Texture
    
    D3D9/Xbox360 implementation of Texture class

    FIXME: need to handle DeviceLost through RenderDevice event handler
    (Win32 only)
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/texturebase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9Texture : public Base::TextureBase
{
    __DeclareClass(D3D9Texture);
public:
    /// constructor
    D3D9Texture();
    /// destructor
    virtual ~D3D9Texture();

    /// unload the resource, or cancel the pending load
    virtual void Unload();
    /// map a texture mip level for CPU access
    bool Map(IndexT mipLevel, MapType mapType, MapInfo& outMapInfo);
    /// unmap texture after CPU access
    void Unmap(IndexT mipLevel);
    /// map a cube map face for CPU access
    bool MapCubeFace(CubeFace face, IndexT mipLevel, MapType mapType, MapInfo& outMapInfo);
    /// unmap cube map face after CPU access
    void UnmapCubeFace(CubeFace face, IndexT mipLevel);

    /// get d3d9 base texture pointer
    IDirect3DBaseTexture9* GetD3D9BaseTexture() const;
    /// get d3d9 texture pointer
    IDirect3DTexture9* GetD3D9Texture() const;
    /// get d3d9 cube texture pointer
    IDirect3DCubeTexture9* GetD3D9CubeTexture() const;
    /// get d3d9 volume texture pointer
    IDirect3DVolumeTexture9* GetD3D9VolumeTexture() const;

    /// setup from a IDirect3DTexture9
    void SetupFromD3D9Texture(IDirect3DTexture9* ptr, const bool setLoaded = true);
    /// setup from a IDirect3DCubeTexture
    void SetupFromD3D9CubeTexture(IDirect3DCubeTexture9* ptr, const bool setLoaded = true);
    /// setup from a IDirect3DVolumeTexture
    void SetupFromD3D9VolumeTexture(IDirect3DVolumeTexture9* ptr, const bool setLoaded = true);

protected:
    IDirect3DBaseTexture9* d3d9BaseTexture;
    int mapCount;
    #if __WIN32__
    IDirect3DTexture9* d3d9Texture;                 // valid if type is Texture2D
    IDirect3DCubeTexture9* d3d9CubeTexture;         // valid if type is TextureCube
    IDirect3DVolumeTexture9* d3d9VolumeTexture;     // valid if type is Texture3D
    #endif
};

//------------------------------------------------------------------------------
/**
*/
inline IDirect3DBaseTexture9*
D3D9Texture::GetD3D9BaseTexture() const
{
    n_assert(0 != this->d3d9BaseTexture);
    n_assert(0 == this->mapCount);
    return this->d3d9BaseTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline IDirect3DTexture9*
D3D9Texture::GetD3D9Texture() const
{
    #if __WIN32__
        // need to use QueryInterface()'d pointer under Win32
        n_assert(0 != this->d3d9Texture);
        return this->d3d9Texture;
    #else
        // on Xbox360 a simple cast does the job
        n_assert(Texture2D == this->type);
        n_assert(0 != this->d3d9BaseTexture);
        return (IDirect3DTexture9*) this->d3d9BaseTexture;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
inline IDirect3DCubeTexture9*
D3D9Texture::GetD3D9CubeTexture() const
{
    #if __WIN32__
        // need to use QueryInterface()'d pointer under Win32
        n_assert(0 != this->d3d9CubeTexture);
        return this->d3d9CubeTexture;
    #else
        // on Xbox360 a simple cast does the job
        n_assert(TextureCube == this->type);
        n_assert(0 != this->d3d9BaseTexture);
        return (IDirect3DCubeTexture9*) this->d3d9BaseTexture;
    #endif
}

//------------------------------------------------------------------------------
/**
*/
inline IDirect3DVolumeTexture9*
D3D9Texture::GetD3D9VolumeTexture() const
{
    #if __WIN32__
        // need to use QueryInterface()'d pointer under Win32
        n_assert(0 != this->d3d9VolumeTexture);
        return this->d3d9VolumeTexture;
    #else
        // on Xbox360 a simple cast does the job
        n_assert(0 != this->d3d9BaseTexture);
        n_assert(Texture3D == this->type);
        return (IDirect3DVolumeTexture9*) this->d3d9BaseTexture;
    #endif
}

} // namespace Direct3D9
//------------------------------------------------------------------------------
    