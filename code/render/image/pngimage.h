#pragma once
//------------------------------------------------------------------------------
/**
    @class Image::PngImage
    
    Implements a png image
    
	FIXME: implement using libpng?

    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "imagebase.h"
namespace Image
{
class PngImage : public ImageBase
{
	__DeclareClass(PngImage);
public:
	/// constructor
	PngImage();
	/// destructor
	virtual ~PngImage();

	/// loads image from raw pointer
	void LoadFromRawData(ubyte* ptr, SizeT size);
}; 
} // namespace Image
//------------------------------------------------------------------------------