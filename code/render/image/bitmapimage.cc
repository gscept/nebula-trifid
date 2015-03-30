//------------------------------------------------------------------------------
//  bitmapimage.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "bitmapimage.h"

namespace Image
{
__ImplementClass(Image::BitmapImage, 'BMPI', Image::ImageBase);

//------------------------------------------------------------------------------
/**
*/
BitmapImage::BitmapImage()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BitmapImage::~BitmapImage()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
BitmapImage::LoadFromRawData( ubyte* ptr, SizeT size )
{
	ushort magic;
	Memory::Copy(ptr, (void*)&magic, sizeof(ushort));
	n_assert2(magic == 'BM', "Image is not of type BMP!");

	// all is fine, let's copy data to our header
	Memory::Copy(ptr + sizeof(ushort), (void*)&this->header, sizeof(BitmapHeader));

	this->width = this->header.width;
	this->height = this->header.height;
	this->bitDepth = this->header.bits;

	this->dataOffset = sizeof(ushort) + sizeof(BitmapHeader);

	if (this->header.compression)
	{
		this->isCompressed = true;

		switch (this->header.compression)
		{
		case 1:
			this->compression = RLE8;
			break;
		case 2:
			this->compression = RLE4;
			break;
		case 3:
			this->compression = RGB;
			break;
		default:
			this->compression = UnknownCompression;
			break;
		}

		n_assert2(this->compression != UnknownCompression, "BMP compression is unknown!");
	}

	// image data size is plainly saved in BMP
	this->size = this->header.imageSize;

	// allocate data buffer
	this->data = new uint[this->size];

	// read data to data pointer
	Memory::Copy(ptr + this->dataOffset, (void*)this->data, this->size);
}

} // namespace Image