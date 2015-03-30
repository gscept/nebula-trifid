#pragma once
//------------------------------------------------------------------------------
/**
    @class Image::JpgImage
    
    Implements a jpeg-image.

	FIXME: implement using libjpg?
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "imagebase.h"
namespace Image
{
class JpegImage : public ImageBase
{
	__DeclareClass(JpegImage);
public:
	/// constructor
	JpegImage();
	/// destructor
	virtual ~JpegImage();

	/// lods jpeg from raw data pointer
	void LoadFromRawData(ubyte* ptr, SizeT size);
}; 
} // namespace Image
//------------------------------------------------------------------------------