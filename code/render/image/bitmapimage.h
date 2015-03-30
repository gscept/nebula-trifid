#pragma once
//------------------------------------------------------------------------------
/**
    @class Image::BitmapImage
    
    Implements a bitmap image
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "imagebase.h"
namespace Image
{
class BitmapImage : public ImageBase
{
	__DeclareClass(BitmapImage);
public:
	/// constructor
	BitmapImage();
	/// destructor
	virtual ~BitmapImage();

	/// load bitmap image from raw data pointer
	void LoadFromRawData(ubyte* ptr, SizeT size);

private:

	struct BitmapHeader
	{
		uint fileSize;
		ushort reserved1, reserved2;
		uint offset;
		uint headerSize;
		int width, height;
		ushort planes;
		ushort bits;
		uint compression;
		uint imageSize;
		int xres, yres;
		uint colorCount;
		uint importantColors;
	};

	BitmapHeader header;
}; 
} // namespace Image
//------------------------------------------------------------------------------