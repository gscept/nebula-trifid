//------------------------------------------------------------------------------
//  d3d9types.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

#include "coregraphics/win360/d3d9types.h"

namespace Direct3D9
{
using namespace Base;
using namespace CoreGraphics;

//------------------------------------------------------------------------------
/**
*/
D3DFORMAT
D3D9Types::AsD3D9PixelFormat(PixelFormat::Code p)
{
    switch (p)
    {
        case PixelFormat::X8R8G8B8:         return D3DFMT_X8R8G8B8;        
        case PixelFormat::A8R8G8B8:         return D3DFMT_A8R8G8B8;
        case PixelFormat::R5G6B5:           return D3DFMT_R5G6B5;
        case PixelFormat::A1R5G5B5:         return D3DFMT_A1R5G5B5;
        case PixelFormat::A4R4G4B4:         return D3DFMT_A4R4G4B4;
        case PixelFormat::DXT1:             return D3DFMT_DXT1;
        case PixelFormat::DXT3:             return D3DFMT_DXT3;
        case PixelFormat::DXT5:             return D3DFMT_DXT5;
        case PixelFormat::R16F:             return D3DFMT_R16F;
        case PixelFormat::G16R16F:          return D3DFMT_G16R16F;
        case PixelFormat::A16B16G16R16F:    return D3DFMT_A16B16G16R16F;
        case PixelFormat::R32F:             return D3DFMT_R32F;
        case PixelFormat::G32R32F:          return D3DFMT_G32R32F;
        case PixelFormat::A32B32G32R32F:    return D3DFMT_A32B32G32R32F;
        case PixelFormat::A8:               return D3DFMT_A8;
        case PixelFormat::A2R10G10B10:      return D3DFMT_A2R10G10B10;
        case PixelFormat::G16R16:           return D3DFMT_G16R16;
        case PixelFormat::D24X8:            return D3DFMT_D24X8;
        case PixelFormat::D24S8:            return D3DFMT_D24S8;

        #if __XBOX360__
        case PixelFormat::X2R10G10B10:      return D3DFMT_X2R10G10B10;
        case PixelFormat::DXN:              return D3DFMT_DXN;
        case PixelFormat::LINDXN:           return D3DFMT_LIN_DXN;
        case PixelFormat::LINDXT1:          return D3DFMT_LIN_DXT1;
        case PixelFormat::LINDXT3:          return D3DFMT_LIN_DXT3;
        case PixelFormat::LINDXT5:          return D3DFMT_LIN_DXT5;
        case PixelFormat::LINA8R8G8B8:      return D3DFMT_LIN_A8R8G8B8;
        case PixelFormat::LINX8R8G8B8:      return D3DFMT_LIN_X8R8G8B8;
        case PixelFormat::LINA16B16G16R16F: return D3DFMT_LIN_A16B16G16R16F;
        case PixelFormat::EDG16R16:         return D3DFMT_G16R16_EDRAM;
        case PixelFormat::CTX1:             return D3DFMT_CTX1;
        case PixelFormat::LINCTX1:          return D3DFMT_LIN_CTX1;
        case PixelFormat::D24FS8:           return D3DFMT_D24FS8;
        #else
        case PixelFormat::R8G8B8:           return D3DFMT_R8G8B8;
        #endif
        default:                            return D3DFMT_UNKNOWN;
    }
}

//------------------------------------------------------------------------------
/**
*/
PixelFormat::Code
D3D9Types::AsNebulaPixelFormat(D3DFORMAT f)
{
    switch (f)
    {
        case D3DFMT_X8R8G8B8:           return PixelFormat::X8R8G8B8;
        case D3DFMT_A8R8G8B8:           return PixelFormat::A8R8G8B8;
        case D3DFMT_R5G6B5:             return PixelFormat::R5G6B5;
        case D3DFMT_A1R5G5B5:           return PixelFormat::A1R5G5B5;
        case D3DFMT_A4R4G4B4:           return PixelFormat::A4R4G4B4;
        case D3DFMT_DXT1:               return PixelFormat::DXT1;
        case D3DFMT_DXT3:               return PixelFormat::DXT3;
        case D3DFMT_DXT5:               return PixelFormat::DXT5;
        case D3DFMT_R16F:               return PixelFormat::R16F;
        case D3DFMT_G16R16F:            return PixelFormat::G16R16F;
        case D3DFMT_A16B16G16R16F:      return PixelFormat::A16B16G16R16F;
        case D3DFMT_R32F:               return PixelFormat::R32F;
        case D3DFMT_G32R32F:            return PixelFormat::G32R32F;
        case D3DFMT_A32B32G32R32F:      return PixelFormat::A32B32G32R32F;
        case D3DFMT_A8:                 return PixelFormat::A8;
        case D3DFMT_A2R10G10B10:        return PixelFormat::A2R10G10B10;
        case D3DFMT_G16R16:             return PixelFormat::G16R16;
        case D3DFMT_D24X8:              return PixelFormat::D24X8;
        case D3DFMT_D24S8:              return PixelFormat::D24S8;

        #if __XBOX360__
        case D3DFMT_DXN:                return PixelFormat::DXN;
        case D3DFMT_LIN_DXN:            return PixelFormat::LINDXN;
        case D3DFMT_X2R10G10B10:        return PixelFormat::X2R10G10B10;
        case D3DFMT_LIN_DXT1:           return PixelFormat::LINDXT1;
        case D3DFMT_LIN_DXT3:           return PixelFormat::LINDXT3;
        case D3DFMT_LIN_DXT5:           return PixelFormat::LINDXT5;
        case D3DFMT_LIN_A8R8G8B8:       return PixelFormat::LINA8R8G8B8;
        case D3DFMT_LIN_X8R8G8B8:       return PixelFormat::LINX8R8G8B8;
        case D3DFMT_LIN_A16B16G16R16F:  return PixelFormat::LINA16B16G16R16F;
        case D3DFMT_G16R16_EDRAM:       return PixelFormat::EDG16R16;
        case D3DFMT_CTX1:               return PixelFormat::CTX1;
        case D3DFMT_LIN_CTX1:           return PixelFormat::LINCTX1;
        case D3DFMT_D24FS8:             return PixelFormat::D24FS8;
        #else
        case D3DFMT_R8G8B8:             return PixelFormat::R8G8B8;
        #endif
        default:                        return PixelFormat::InvalidPixelFormat;
    }
}

//------------------------------------------------------------------------------
/**
*/
D3DDECLTYPE
D3D9Types::AsD3D9VertexDeclarationType(VertexComponent::Format f)
{
    switch (f)
    {
        case VertexComponent::Float:    return D3DDECLTYPE_FLOAT1;
        case VertexComponent::Float2:   return D3DDECLTYPE_FLOAT2;
        case VertexComponent::Float3:   return D3DDECLTYPE_FLOAT3;
        case VertexComponent::Float4:   return D3DDECLTYPE_FLOAT4;
        case VertexComponent::UByte4:   return D3DDECLTYPE_UBYTE4;
        case VertexComponent::Short2:   return D3DDECLTYPE_SHORT2;
        case VertexComponent::Short4:   return D3DDECLTYPE_SHORT4;
        case VertexComponent::UByte4N:  return D3DDECLTYPE_UBYTE4N;
        case VertexComponent::Short2N:  return D3DDECLTYPE_SHORT2N;
        case VertexComponent::Short4N:  return D3DDECLTYPE_SHORT4N;
        default:                        
            n_error("D3D9Types::AsDirect3DVertexDeclarationType(): invalid input parameter!");
            return D3DDECLTYPE_UNUSED;
    }
}

//------------------------------------------------------------------------------
/**
*/
D3DDECLUSAGE
D3D9Types::AsD3D9VertexDeclarationUsage(VertexComponent::SemanticName n)
{
    switch (n)
    {
        case VertexComponent::Position:     return D3DDECLUSAGE_POSITION;
        case VertexComponent::Normal:       return D3DDECLUSAGE_NORMAL;
        case VertexComponent::Tangent:      return D3DDECLUSAGE_TANGENT;
        case VertexComponent::Binormal:     return D3DDECLUSAGE_BINORMAL;
        case VertexComponent::TexCoord:     return D3DDECLUSAGE_TEXCOORD;
        case VertexComponent::SkinWeights:  return D3DDECLUSAGE_BLENDWEIGHT;
        case VertexComponent::SkinJIndices: return D3DDECLUSAGE_BLENDINDICES;
        case VertexComponent::Color:        return D3DDECLUSAGE_COLOR;
        default:
            n_error("D3D9Types::AsDirect3DVertexDeclarationUsage(): invalid input parameter!");
            return D3DDECLUSAGE_POSITION;
    }
}

//------------------------------------------------------------------------------
/**
*/
D3DPRIMITIVETYPE
D3D9Types::AsD3D9PrimitiveType(PrimitiveTopology::Code t)
{
    switch (t)
    {
        case PrimitiveTopology::PointList:      return D3DPT_POINTLIST;
        case PrimitiveTopology::LineList:       return D3DPT_LINELIST;
        case PrimitiveTopology::LineStrip:      return D3DPT_LINESTRIP;
        case PrimitiveTopology::TriangleList:   return D3DPT_TRIANGLELIST;
        case PrimitiveTopology::TriangleStrip:  return D3DPT_TRIANGLESTRIP;
        #if __XBOX360__
        case PrimitiveTopology::RectList:       return D3DPT_RECTLIST;
        case PrimitiveTopology::QuadList:       return D3DPT_QUADLIST;
        #endif
        default:
            n_error("D3D9Types::AsDirect3DPrimitiveType(): unsupported topology '%s'!",
                PrimitiveTopology::ToString(t).AsCharPtr());
            return D3DPT_TRIANGLELIST;
    }
}

//------------------------------------------------------------------------------
/**
*/
D3DMULTISAMPLE_TYPE
D3D9Types::AsD3D9MultiSampleType(AntiAliasQuality::Code c)
{
    switch (c)
    {
        case AntiAliasQuality::None:    
            return D3DMULTISAMPLE_NONE;
        case AntiAliasQuality::Low:     
            return D3DMULTISAMPLE_2_SAMPLES;
        
#if __WIN32__
        case AntiAliasQuality::Medium:
            return D3DMULTISAMPLE_4_SAMPLES;    
        case AntiAliasQuality::High:    
            return D3DMULTISAMPLE_8_SAMPLES;    
#else   // xbox 360 only supports up to 4xAA
        case AntiAliasQuality::Medium:      
        case AntiAliasQuality::High:    
            return D3DMULTISAMPLE_4_SAMPLES;
#endif
        
        default:
            n_error("D3D9Types::AsD3D9MultiSampleType(): unsupported AntiAliasQuality!");
            return D3DMULTISAMPLE_NONE;
    }
}

//------------------------------------------------------------------------------
/**
*/
D3DXIMAGE_FILEFORMAT
D3D9Types::AsD3DXImageFileFormat(ImageFileFormat::Code c)
{
    switch (c)
    {
        case ImageFileFormat::BMP:  return D3DXIFF_BMP;
        case ImageFileFormat::JPG:  return D3DXIFF_JPG;
        case ImageFileFormat::PNG:  return D3DXIFF_PNG;
        case ImageFileFormat::DDS:  return D3DXIFF_DDS;
        default:
            n_error("D3D9Types::AsD3DXImageFileFormat(): unsupported ImageFileFormat!");
            return D3DXIFF_BMP;
    }
}

//------------------------------------------------------------------------------
/**
*/
D3DPOOL
D3D9Types::AsD3D9Pool(ResourceBase::Usage usage, ResourceBase::Access access)
{
    #if __WIN32__
        switch (usage)
        {
            case ResourceBase::UsageImmutable:
                n_assert(ResourceBase::AccessNone == access);
                return D3DPOOL_MANAGED;

            case ResourceBase::UsageDynamic:
                n_assert(ResourceBase::AccessWrite == access);
                return D3DPOOL_DEFAULT;

            case ResourceBase::UsageCpu:
                return D3DPOOL_SYSTEMMEM;

            default:
                n_error("D3D9Util::AsD3D9Pool(): invalid usage parameter!");
                return D3DPOOL_SYSTEMMEM;
        }
    #elif __XBOX360__
        // the pool flag is ignored on the Xbox360, so just return "something"
        switch (usage)
        {
            case ResourceBase::UsageImmutable:
                n_assert(ResourceBase::AccessNone == access);
                return D3DPOOL_SYSTEMMEM;

            case ResourceBase::UsageDynamic:
                n_assert(ResourceBase::AccessWrite == access);
                return D3DPOOL_SYSTEMMEM;

            case ResourceBase::UsageCpu:
                return D3DPOOL_SYSTEMMEM;

            default:
                n_error("D3D9Util::AsD3D9Pool(): invalid usage parameter!");
                return D3DPOOL_SYSTEMMEM;
        }
    #else
    #error "D3D9Util::AsD3D9Pool: Unsupported platform!"
    #endif
}

//------------------------------------------------------------------------------
/**
*/
DWORD
D3D9Types::AsD3D9Usage(ResourceBase::Usage usage, ResourceBase::Access access)
{
    #if __WIN32__
        switch (usage)
        {
            case ResourceBase::UsageImmutable:
                n_assert(ResourceBase::AccessNone == access);
                return D3DUSAGE_WRITEONLY;

            case ResourceBase::UsageDynamic:
                n_assert(ResourceBase::AccessWrite == access);
                return D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;

            case ResourceBase::UsageCpu:
                return D3DUSAGE_DYNAMIC;

            default:
                n_error("D3D9Util::AsD3D9Usage(): invalid usage parameter!");
                return D3DPOOL_SYSTEMMEM;
        }
    #elif __XBOX360__
        // only CPU access is relevant on the 360
        switch (usage)
        {
            case ResourceBase::UsageImmutable:
                n_assert(ResourceBase::AccessNone == access);
                return 0;

            case ResourceBase::UsageDynamic:
                n_assert(ResourceBase::AccessWrite == access);
                return 0;

            case ResourceBase::UsageCpu:
                return D3DUSAGE_CPU_CACHED_MEMORY;

            default:
                n_error("D3D9Util::AsD3D9Usage(): invalid usage parameter!");
                return 0;
        }
    #else
    #error "D3D9Util::AsD3D9Usage: Unsupported platform!"
    #endif
}

//------------------------------------------------------------------------------
/**
*/
D3DFORMAT
D3D9Types::IndexTypeAsD3D9Format(IndexType::Code indexType)
{
    return (IndexType::Index16 == indexType) ? D3DFMT_INDEX16 : D3DFMT_INDEX32;
}

} // namespace Win360
