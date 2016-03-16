#pragma once
//------------------------------------------------------------------------------
/**
    @class CoreGraphics::PixelFormat
    
    Pixel format enumeration.

    FIXME: use DX10 notations (more flexible but less readable...)

    (C) 2006 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/types.h"
#include "util/string.h"

//------------------------------------------------------------------------------
namespace CoreGraphics
{
class PixelFormat
{
public:
    /// enums
    enum Code
    {
        X8R8G8B8 = 0,
        R8G8B8,
        A8R8G8B8,
        R5G6B5,
        A1R5G5B5,
        A4R4G4B4,
        DXT1,
		DXT1A,
        DXT3,
        DXT5,
		DXT1sRGB,
		DXT1AsRGB,
		DXT3sRGB,
		DXT5sRGB,
		BC7,
		BC7sRGB,
        R16F,                       // 16 bit float, red only
        G16R16F,                    // 32 bit float, 16 bit red, 16 bit green
        A16B16G16R16F,              // 64 bit float, 16 bit rgba each
		A16B16G16R16,				// 64 bit int, 16 bit rgba each
        R32F,                       // 32 bit float, red only
        G32R32F,                    // 64 bit float, 32 bit red, 32 bit green
        A32B32G32R32F,              // 128 bit float, 32 bit rgba each
		R32G32B32F,					// 96 bit float, 32 bit rgb each
		R11G11B10F,					// 32 bit float, 11 bits red and green, 10 bit blue
        A8,
		SRGBA8,
        X2R10G10B10,
        A2R10G10B10,
        G16R16,
        D24X8,
        D24S8,

		D32S8,
		D16S8,
        
        // Xbox360 specific pixel formats
        DXN,
        LINDXN,
        LINDXT1,
        LINDXT3,
        LINDXT5,
        LINA8R8G8B8,
        LINX8R8G8B8,
        LINA16B16G16R16F,
        EDG16R16,
        CTX1,
        LINCTX1,
        D24FS8,

        // Wii specific pixel formats
        I4,
        I8,
        IA4,
        IA8,
        Z8,
        Z16,
        Z24X8,
        R4,
        RA4,
        RA8,
        R8,
        G8,
        B8,
        RG8,
        GB8,
        Z4,
        Z8M,
        Z8L,
        Z16L,    
        RGBA8,
        RGB5A3,
        RGB565,
        CMPR,

        // PS3 specific surface pixel formats
        X1R5G5B5_Z1R5G5B5,
        X1R5G5B5_O1R5G5B5,
        X8R8G8B8_Z8R8G8B8,
        G8B8,
        X8B8G8R8_Z8B8G8R8,
        X8B8G8R8_O8B8G8R8,
        A8B8G8R8,

        // PS3 specific texture pixel formats
        R6G5B5,
        DEPTH24D8,
        DEPTH24D8F,
        DEPTH16,
        DEPTH16F,
        X16,
        COMPRESSED_HILO8,
        COMPRESSED_HILO_S8,
        COMPRESSED_B8R8_G8R8,
        COMPRESSED_R8B8_R8G8,

        NumPixelFormats,
        InvalidPixelFormat,
    };

    /// convert from string
    static Code FromString(const Util::String& str);
    /// convert to string
    static Util::String ToString(Code code);
	/// convert to byte size
	static uint ToSize(Code code);
	/// convert to number of channesl
	static uint ToChannels(Code code);
	/// convert to IL image components
	static uint ToILComponents(Code code);
	/// conver to IL image type
	static uint ToILType(Code code);
};

} // namespace CoreGraphics
//------------------------------------------------------------------------------

    