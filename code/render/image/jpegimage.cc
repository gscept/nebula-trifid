//------------------------------------------------------------------------------
//  jpgimage.cc
//  (C) 2012-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "jpegimage.h"
#include "libjpeg/jpeglib.h"

namespace Image
{
__ImplementClass(Image::JpegImage, 'JPGI', Image::ImageBase);

//------------------------------------------------------------------------------
/**
*/
JpegImage::JpegImage()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
JpegImage::~JpegImage()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
JpegImage::LoadFromRawData( ubyte* ptr, SizeT size )
{
	jpeg_decompress_struct info;

	jpeg_create_decompress(&info);
	jpeg_mem_src(&info, ptr, size);
	jpeg_read_header(&info, true);

	this->width = info.output_width;
	this->height = info.output_height;
	this->bitDepth = info.output_components * 8;
	this->size = info.output_width * info.output_components * this->height;

	// allocate data buffer
	this->data = n_new_array(uint, this->size);

	// start jpeg decompression
	jpeg_start_decompress(&info);

	// get size of row
	int rowStride = info.output_width * info.output_components;
	JSAMPARRAY buffer = (*info.mem->alloc_sarray)((j_common_ptr) &info, JPOOL_IMAGE, rowStride, 1);
	
	int y = 0;
	while (info.output_scanline < info.output_height)
	{
		// read to buffer
		jpeg_read_scanlines(&info, buffer, this->height);

		Memory::Copy(buffer[0], (void*)(this->data[y * rowStride]), rowStride);
		y++;
	}

	jpeg_destroy_decompress(&info);
}
} // namespace Image