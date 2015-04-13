//------------------------------------------------------------------------------
//  pngimage.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "pngimage.h"
#include "libpng/png.h"

namespace Image
{
__ImplementClass(Image::PngImage, 'PNGI', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
PngImage::PngImage()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
PngImage::~PngImage()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
PngImage::LoadFromRawData( ubyte* ptr, SizeT size )
{
	int isPng = png_sig_cmp(ptr, 0, 8);
	n_assert2(isPng, "Image is not PNG");

	png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0,0,0);
	n_assert(pngPtr);

	png_infop infoPtr = png_create_info_struct(pngPtr);
	n_assert(infoPtr);

	png_set_sig_bytes(pngPtr, sizeof(int));
	png_set_compression_buffer_size(pngPtr, size);

	png_read_png(pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL);

	this->width = png_get_image_width(pngPtr, infoPtr); 
	this->height = png_get_image_height(pngPtr, infoPtr);
	this->bitDepth = png_get_bit_depth(pngPtr, infoPtr);

	// get byte size of row
	size_t rowSize = png_get_rowbytes(pngPtr, infoPtr);

	// total size of image must then be row size times the height
	this->size = rowSize * this->height;

	// allocate data buffer
	this->data = new uint[this->size];

	// rows are saved in two-dimensional array
	png_bytepp rows = png_get_rows(pngPtr, infoPtr);

	// copy all the data to the data buffer
	Memory::Copy(rows, (void*)this->data, this->size);

	png_destroy_read_struct(&pngPtr, &infoPtr, NULL);

}
} // namespace Image