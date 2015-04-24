//------------------------------------------------------------------------------
//  ddsimage.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "ddsimage.h"

#ifdef RGB
#undef RGB
#endif
#define RGB(r,g,b) (uint)( ((255 & 0xff) << 24) |((b & 0xff) << 16) | ((g & 0xff) << 8) | ((r & 0xff)) )
#define RGBA(r,g,b,a) (uint)( ((a & 0xff) << 24) | ((b & 0xff) << 16) | ((g & 0xff) << 8) | ((r & 0xff)) )

#define RED(rgb) (rgb & 0xff)
#define BLUE(rgb) ((rgb >> 16) & 0xff)
#define GREEN(rgb) ((rgb >> 8) & 0xff)
#define ALPHA(rgb) (rgb >> 24)

namespace Image
{
__ImplementClass(Image::DdsImage, 'DDSI', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
DdsImage::DdsImage()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
DdsImage::~DdsImage()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
DdsImage::LoadFromRawData( ubyte* ptr, SizeT size )
{
	int magic;
	Memory::Copy(ptr, (void*)&magic, sizeof(int));
	n_assert2(magic == 0x20534444, "Image is not of type DDS!");

	// all is fine, let's copy data to our header
	Memory::Copy(ptr + sizeof(int), (void*)&this->header, sizeof(DdsHeader));

	this->width = this->header.width;
	this->height = this->header.height;
	this->depth = this->header.depth;
	this->bitDepth = this->header.bitCount;
	this->mipCount = this->header.mipCount;
	
	this->dataOffset = sizeof(int) + sizeof(DdsHeader);

	// if the header has flag 0x4, the DDS is compressed
	if (this->header.flags & 0x4)
	{
		this->isCompressed = true;

		switch (this->header.fourCC)
		{
		case 'DXT1':
			this->compression = BC1;
			break;
		case 'DXT3':
			this->compression = BC2;
			break;
		case 'DXT5':
			this->compression = BC3;
			break;
		case 'DX10':
			Memory::Copy(ptr + this->dataOffset, (void*)&this->extendedHeader, sizeof(DdsHeaderExtended));
			switch (this->extendedHeader.dimension)
			{
			case 2:
				this->dimension = Dimension1D;
				break;
			case 3:
				this->dimension = Dimension2D;
				break;
			case 4:
				this->dimension = Dimension3D;
				break;
			}

			if (this->extendedHeader.flags & 0x4)
			{
				this->dimension = DimensionCube;
			}

			this->dataOffset += sizeof(DdsHeaderExtended);
			break;
		default:
			this->compression = UnknownCompression;
			break;
		}

		n_assert2(this->compression != UnknownCompression, "DDS compression is unknown!");
	}

	// size of image data should be the size of the stream - the complete header and magic bytes
	this->size = size - this->dataOffset;

	// allocate data buffer
	ubyte* rawData = n_new_array(ubyte, this->size);

	// read data to data pointer
	Memory::Copy(ptr + this->dataOffset, (void*)rawData, this->size);

	uint unpackedSize = this->height * this->width * 16;
	this->data = n_new_array(uint, unpackedSize);

	// if dds is compressed, decompress and read to buffer
	if (this->isCompressed)
	{
		switch (this->compression)
		{
		case BC1:
			{
				this->UnpackDXT1(rawData);
			}
		case BC2:
			{
				this->UnpackDXT3(rawData);
			}
		case BC3:
			{
				this->UnpackDXT5(rawData);
			}
		}
	}
	else
	{
		// if we have no compression, simply copy data from image to member buffer
		Memory::Copy(rawData, (void*)this->data, this->size);
	}

	n_delete_array(rawData);
}

//------------------------------------------------------------------------------
/**
*/
void
DdsImage::UnpackDXT1(ubyte* src)
{
	uint blockCountY = (this->height + 3) / 4;
	uint blockCountX = (this->width + 3) / 4;

	ubyte* currentBlock = src;

	for (uint i = 0; i < blockCountY; i++)
	{
		for (uint j = 0; j < blockCountX; j++)
		{
			this->DecompressDXT1(j*4, i*4, currentBlock + j * 8);
		}

		currentBlock += blockCountX*8;
	}
}


//------------------------------------------------------------------------------
/**
*/
void
DdsImage::DecompressDXT1( uint x, uint y, ubyte* src )
{
	ushort color0, color1;
	color0 = *reinterpret_cast<const ushort *>(src);
	color1 = *reinterpret_cast<const ushort *>(src + 2);

	// sizeof int is two shorts
	uint code = *reinterpret_cast<const unsigned long *>(src + 4);

	uint temp;
	temp = (color0 >> 11) * 255 + 16;
	uchar r0 = (uchar)((temp/32 + temp)/32);
	temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
	uchar g0 = (uchar)((temp/64 + temp)/64);
	temp = (color0 & 0x001F) * 255 + 16;
	uchar b0 = (uchar)((temp/32 + temp)/32);

	temp = (color1 >> 11) * 255 + 16;
	uchar r1 = (uchar)((temp/32 + temp)/32);
	temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
	uchar g1 = (uchar)((temp/64 + temp)/64);
	temp = (color1 & 0x001F) * 255 + 16;
	uchar b1 = (uchar)((temp/32 + temp)/32);

	uint rgb0, rgb1;

	rgb0 = RGB(r0, g0, b0);
	rgb1 = RGB(r1, g1, b1);

	uint color2, color3;

	if (color0 > color1)
	{
		color2 = RGB((2*r0+r1)/3, (2*g0+g1)/3, (2*b0+b1)/3);
		color3 = RGB((r0+2*r1)/3, (g0+2*g1)/3, (b0+2*b1)/3);
	}
	else
	{
		color2 = RGB((r0+r1)/2, (g0+g1)/2, (b0+b1)/2);
		color3 = RGB(0,0,0);
	}

	uint colors[4];
	colors[0] = rgb0;
	colors[1] = rgb1;
	colors[2] = color2;
	colors[3] = color3;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			uchar posCode = (code >> 2*(4*i+j)) & 0x03;
			uint color = colors[posCode];

			if (x + i < width)
			{
				this->data[(y+i)*width + (x+j)] = color;
			}
		}
	}
}


//------------------------------------------------------------------------------
/**
*/
void
DdsImage::UnpackDXT3(ubyte* src)
{
	uint blockCountY = (this->height + 3) / 4;
	uint blockCountX = (this->width + 3) / 4;

	ubyte* currentBlock = src;

	for (uint i = 0; i < blockCountY; i++)
	{
		for (uint j = 0; j < blockCountX; j++)
		{
			this->DecompressDXT1(j*4, i*4, currentBlock + j * 16);
		}

		currentBlock += blockCountX*16;
	}
}


//------------------------------------------------------------------------------
/**
*/
void
DdsImage::DecompressDXT3( uint x, uint y, ubyte* src )
{
	ubyte alphas[8];
	alphas[0] = *reinterpret_cast<const ubyte *>(src);
	alphas[1] = *reinterpret_cast<const ubyte *>(src+1);
	alphas[2] = *reinterpret_cast<const ubyte *>(src+2);
	alphas[3] = *reinterpret_cast<const ubyte *>(src+3);
	alphas[4] = *reinterpret_cast<const ubyte *>(src+4);
	alphas[5] = *reinterpret_cast<const ubyte *>(src+5);
	alphas[6] = *reinterpret_cast<const ubyte *>(src+6);
	alphas[7] = *reinterpret_cast<const ubyte *>(src+7);

	ushort color0, color1;
	color0 = *reinterpret_cast<const ushort *>(src + 8);
	color1 = *reinterpret_cast<const ushort *>(src + 10);

	uint code = *reinterpret_cast<const uint *>(src + 12);

	uint temp;
	temp = (color0 >> 11) * 255 + 16;
	ubyte r0 = (ubyte)((temp/32 + temp)/32);
	temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
	ubyte g0 = (ubyte)((temp/64 + temp)/64);
	temp = (color0 & 0x001F) * 255 + 16;
	ubyte b0 = (ubyte)((temp/32 + temp)/32);

	temp = (color1 >> 11) * 255 + 16;
	ubyte r1 = (ubyte)((temp/32 + temp)/32);
	temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
	ubyte g1 = (ubyte)((temp/64 + temp)/64);
	temp = (color1 & 0x001F) * 255 + 16;
	ubyte b1 = (ubyte)((temp/32 + temp)/32);

	uint rgb0 = RGB(r0, g0, b0);
	uint rgb1 = RGB(r1, g1, b1);
	uint color2 = RGB((2 * r0 + r1)/3, (2 * g0 + g1)/3, (2 * b0 + b1)/3);
	uint color3 = RGB((r0 + 2 * r1)/3, (g0 + 2 * g1)/3, (b0 + 2 * b1)/3);	

	uint colors[4];
	colors[0] = rgb0;
	colors[1] = rgb1;
	colors[2] = color2;
	colors[3] = color3;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ubyte posCode = (code >> 2*(4*i+j)) & 0x03;
			uint alpha = alphas[posCode] + alphas[posCode+1] * 256;
			uint color = colors[posCode];
			uint red = RED(color);
			uint green = GREEN(color);
			uint blue = BLUE(color);

			this->data[(y+i)*width + (x+j)] = RGBA(red, green, blue, alpha);
		}
	}
}


//------------------------------------------------------------------------------
/**
*/
void
DdsImage::UnpackDXT5(ubyte* src)
{
	uint blockCountY = (this->height + 3) / 4;
	uint blockCountX = (this->width + 3) / 4;

	ubyte* currentBlock = src;

	for (uint i = 0; i < blockCountY; i++)
	{
		for (uint j = 0; j < blockCountX; j++)
		{
			this->DecompressDXT1(j*4, i*4, currentBlock + j * 16);
		}

		currentBlock += blockCountX*16;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
DdsImage::DecompressDXT5( uint x, uint y, ubyte* src )
{
	uchar alpha0 = *reinterpret_cast<const uchar *>(src);
	uchar alpha1 = *reinterpret_cast<const uchar *>(src + 1);

	const uchar *bits = src + 2;
	uint alphaCode1 = bits[2] | (bits[3] << 8) | (bits[4] << 16) | (bits[5] << 24);
	ushort alphaCode2 = bits[0] | (bits[1] << 8);

	ushort color0 = *reinterpret_cast<const ushort *>(src + 8);
	ushort color1 = *reinterpret_cast<const ushort *>(src + 10);	

	uint code = *reinterpret_cast<const uint *>(src + 12);

	uint temp;
	temp = (color0 >> 11) * 255 + 16;
	uchar r0 = (uchar)((temp/32 + temp)/32);
	temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
	uchar g0 = (uchar)((temp/64 + temp)/64);
	temp = (color0 & 0x001F) * 255 + 16;
	uchar b0 = (uchar)((temp/32 + temp)/32);

	temp = (color1 >> 11) * 255 + 16;
	uchar r1 = (uchar)((temp/32 + temp)/32);
	temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
	uchar g1 = (uchar)((temp/64 + temp)/64);
	temp = (color1 & 0x001F) * 255 + 16;
	uchar b1 = (uchar)((temp/32 + temp)/32);

	uint rgb0 = RGB(r0, g0, b0);
	uint rgb1 = RGB(r1, g1, b1);
	uint color2 = RGB((2 * r0 + r1)/3, (2 * g0 + g1)/3, (2 * b0 + b1)/3);
	uint color3 = RGB((r0 + 2 * r1)/3, (g0 + 2 * g1)/3, (b0 + 2 * b1)/3);

	uint colors[4];
	colors[0] = rgb0;
	colors[1] = rgb1;
	colors[2] = color2;
	colors[3] = color3;

	uchar alpha2, alpha3, alpha4, alpha5, alpha6, alpha7;

	if (alpha0 > alpha1)
	{
		alpha2 = (6 * alpha0 + 1 * alpha1)/7;
		alpha3 = (5 * alpha0 + 2 * alpha1)/7;
		alpha4 = (4 * alpha0 + 3 * alpha1)/7;
		alpha5 = (3 * alpha0 + 4 * alpha1)/7;
		alpha6 = (2 * alpha0 + 5 * alpha1)/7;
		alpha7 = (1 * alpha0 + 6 * alpha1)/7;
	}

	else
	{
		alpha2 = (4 * alpha0 + 1 * alpha1)/5;
		alpha3 = (3 * alpha0 + 2 * alpha1)/5;
		alpha4 = (2 * alpha0 + 3 * alpha1)/5;
		alpha5 = (1 * alpha0 + 4 * alpha1)/5;
		alpha6 = 0;
		alpha7 = 255;
	}

	uchar alphas[8];
	alphas[0] = alpha0;
	alphas[1] = alpha1;
	alphas[2] = alpha2;
	alphas[3] = alpha3;
	alphas[4] = alpha4;
	alphas[5] = alpha5;
	alphas[6] = alpha6;
	alphas[7] = alpha7;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			int alphaCodeIndex = 3*(4*i+j);
			int alphaCode;

			if (alphaCodeIndex <= 12)
			{
				alphaCode = (alphaCode2 >> alphaCodeIndex) & 0x07;
			}
			else if (alphaCodeIndex == 15)
			{
				alphaCode = (alphaCode2 >> 15) | ((alphaCode1 << 1) & 0x06);
			}
			else // alphaCodeIndex >= 18 && alphaCodeIndex <= 45
			{
				alphaCode = (alphaCode1 >> (alphaCodeIndex - 16)) & 0x07;
			}

			uchar alpha = alphas[alphaCode];
			uchar posCode = (code >> 2*(4*i+j)) & 0x03;
			uint color = colors[posCode];
			uint red = RED(color);
			uint green = GREEN(color);
			uint blue = BLUE(color);


			this->data[(y + i)*width + (x + j)] = RGBA(red, green, blue, alpha);
		}
	}
}

} // namespace Image