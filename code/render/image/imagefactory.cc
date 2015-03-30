//------------------------------------------------------------------------------
//  imagefactory.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "imagefactory.h"
#include "coregraphics/imagefileformat.h"
#include "io/ioserver.h"
#include "bitmapimage.h"
#include "jpegimage.h"
#include "pngimage.h"
#include "ddsimage.h"

using namespace Util;
using namespace IO;
using namespace CoreGraphics;
namespace Image
{
//------------------------------------------------------------------------------
/**
*/
Ptr<ImageBase> 
ImageFactory::CreateImage( const URI& uri )
{
	String extension = uri.AsString().GetFileExtension();
	ImageFileFormat::Code imageFormat = ImageFileFormat::FromString(extension);

	Ptr<ImageBase> image = 0;
	switch (imageFormat)
	{
	case ImageFileFormat::BMP:
		{
			image = BitmapImage::Create();
			break;
		}
	case ImageFileFormat::JPG:
		{
			image = JpegImage::Create();
			break;
		}
	case ImageFileFormat::PNG:
		{
			image = PngImage::Create();
			break;
		}
	case ImageFileFormat::DDS:
		{
			image = DdsImage::Create();
			break;
		}
	}

	// load image if type is valid
	if (image.isvalid())
	{
		Ptr<Stream> stream = IoServer::Instance()->CreateStream(uri);
		image->LoadFromStream(stream);
	}
	
	return image;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ImageBase>
ImageFactory::CreateImage( const Ptr<IO::Stream>& stream )
{
	String extension = stream->GetURI().GetHostAndLocalPath().GetFileExtension();
	ImageFileFormat::Code imageFormat = ImageFileFormat::FromString(extension);

	Ptr<ImageBase> image = 0;
	switch (imageFormat)
	{
	case ImageFileFormat::BMP:
		{
			image = BitmapImage::Create();
			break;
		}
	case ImageFileFormat::JPG:
		{
			image = JpegImage::Create();
			break;
		}
	case ImageFileFormat::PNG:
		{
			image = PngImage::Create();
			break;
		}
	case ImageFileFormat::DDS:
		{
			image = DdsImage::Create();
			break;
		}
	}

	// load image if type is valid
	if (image.isvalid())
	{
		image->LoadFromStream(stream);
	}

	return image;
}

} // namespace Image
