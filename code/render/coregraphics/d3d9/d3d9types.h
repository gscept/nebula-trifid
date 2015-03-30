#pragma once
//------------------------------------------------------------------------------
/**
    @class Win360::D3D9Types
    
    Provides static helper functions to convert from and to Direct3D
    data types and enumerations.
    
    (C) 2007 Radon Labs GmbH
*/
#include "core/types.h"
#include "coregraphics/pixelformat.h"
#include "coregraphics/vertexcomponent.h"
#include "coregraphics/primitivetopology.h"
#include "coregraphics/antialiasquality.h"
#include "coregraphics/imagefileformat.h"
#include "coregraphics/indextype.h"
#include "coregraphics/base/resourcebase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9Types
{
public:
    /// convert Nebula pixel format to D3D9 pixel format
    static D3DFORMAT AsD3D9PixelFormat(CoreGraphics::PixelFormat::Code p);
    /// convert Direct3D to Nebula pixel format
    static CoreGraphics::PixelFormat::Code AsNebulaPixelFormat(D3DFORMAT f);
    /// convert vertex component type to D3D9 declaration type
    static D3DDECLTYPE AsD3D9VertexDeclarationType(CoreGraphics::VertexComponent::Format f);
    /// convert vertex component semantic name as D3D9 declaration usage
    static D3DDECLUSAGE AsD3D9VertexDeclarationUsage(CoreGraphics::VertexComponent::SemanticName n);
    /// convert primitive topology to D3D
    static D3DPRIMITIVETYPE AsD3D9PrimitiveType(CoreGraphics::PrimitiveTopology::Code t);
    /// convert antialias quality to D3D multisample type
    static D3DMULTISAMPLE_TYPE AsD3D9MultiSampleType(CoreGraphics::AntiAliasQuality::Code c);
    /// convert image file format to D3DX file format
    static D3DXIMAGE_FILEFORMAT AsD3DXImageFileFormat(CoreGraphics::ImageFileFormat::Code c);
    /// convert Nebula3 resource usage/access flag pair into D3D9 pool
    static D3DPOOL AsD3D9Pool(Base::ResourceBase::Usage usage, Base::ResourceBase::Access access);
    /// convert Nebula3 resource usage/access flag pair into D3D9 usage flags
    static DWORD AsD3D9Usage(Base::ResourceBase::Usage usage, Base::ResourceBase::Access access);
    /// convert index type to D3DFORMAT
    static D3DFORMAT IndexTypeAsD3D9Format(CoreGraphics::IndexType::Code indexType);
};

} // namespace Direct3D9
//------------------------------------------------------------------------------
