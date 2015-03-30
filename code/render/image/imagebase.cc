//------------------------------------------------------------------------------
//  imagebase.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "imagebase.h"

namespace Image
{
__ImplementClass(Image::ImageBase, 'IMBA', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ImageBase::ImageBase() :
	dimension(Dimension2D),
	width(0),
	height(0),
	depth(0),
	bitDepth(8),
	size(0),
	dataOffset(0),
	mipCount(1),
	data(0),
	isCompressed(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ImageBase::~ImageBase()
{
	if (this->data)
	{
		delete [] this->data;
	}
}

//------------------------------------------------------------------------------
/**
*/
bool 
ImageBase::LoadFromStream( const Ptr<IO::Stream>& stream )
{
	n_assert(stream.isvalid());
	if (stream->IsOpen())
	{
		ubyte* data = (ubyte*)stream->Map();
		SizeT size = stream->GetSize();

		this->LoadFromRawData((ubyte*)data, size);
		return true;
	}
	else if (stream->Open())
	{
		ubyte* data = (ubyte*)stream->Map();
		SizeT size = stream->GetSize();

		this->LoadFromRawData((ubyte*)data, size);
		return true;
	}

	// image load failed
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
ImageBase::LoadFromRawData( ubyte* ptr, SizeT size )
{
	// implement in subclass
	n_error("ImageBase::LoadFromRawData() called!");
}
} // namespace Image