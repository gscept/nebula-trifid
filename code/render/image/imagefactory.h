#pragma once
//------------------------------------------------------------------------------
/**
    @class Image::ImageFactory
    
    Creates images from path
    
    (C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "io/uri.h"
#include "imagebase.h"

namespace Image
{
class ImageFactory
{
public:
	/// loads image from uri, also figures out what file type should be used
	static Ptr<ImageBase> CreateImage(const IO::URI& uri);
	/// loads image from pre-existing stream
	static Ptr<ImageBase> CreateImage(const Ptr<IO::Stream>& stream);
}; 
} // namespace Image
//------------------------------------------------------------------------------