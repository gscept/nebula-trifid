#pragma once
//------------------------------------------------------------------------------
/**
    @class Image::ImageBase
    
    Base class for images
    
    (C) 2012-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "io/stream.h"
namespace Image
{
class ImageBase : public Core::RefCounted
{
	__DeclareClass(ImageBase);
public:

	enum Dimension
	{
		Dimension1D,
		Dimension2D,
		Dimension3D,
		DimensionCube,

		NumDimensions
	};

	enum Compression
	{
		NoCompression,
		RLE24,
		RLE8,
		RLE4,
		RGB,
		BC1,
		BC2,
		BC3,
		BPTC,			// this is the same as BC7

		UnknownCompression,

		NumCompressions
	};
	/// constructor
	ImageBase();
	/// destructor
	virtual ~ImageBase();

	/// loads image from stream
	bool LoadFromStream(const Ptr<IO::Stream>& stream);
	/// loads image from raw data pointer
	virtual void LoadFromRawData(ubyte* ptr, SizeT size);

	/// returns width of image
	const uint& GetWidth() const;
	/// returns height of image
	const uint& GetHeight() const;
	/// returns depth of image (only viable for 3D images)
	const uint& GetDepth() const;

	/// returns bit-depth of image
	const uint& GetBitDepth() const;
	/// returns the image dimension (only varying for some image types)
	const Dimension& GetDimension() const;
	/// returns true if image is compressed
	const bool& IsCompressed() const;
	/// returns compression flag
	const Compression& GetCompression() const;

	/// returns byte size of image
	const uint& GetSize() const;
	/// returns pointer to data
	void* GetData() const;

protected:
	Dimension dimension;
	Compression compression;
	uint width;
	uint height;
	uint depth;
	uint bitDepth;
	uint size;
	uint dataOffset;
	uint mipCount;
	uint* data;

	bool isCompressed;
}; 

//------------------------------------------------------------------------------
/**
*/
inline const uint& 
ImageBase::GetWidth() const
{
	return this->width;
}

//------------------------------------------------------------------------------
/**
*/
inline const uint& 
ImageBase::GetHeight() const
{
	return this->height;
}

//------------------------------------------------------------------------------
/**
*/
inline const uint& 
ImageBase::GetDepth() const
{
	return this->depth;
}

//------------------------------------------------------------------------------
/**
*/
inline const uint& 
ImageBase::GetBitDepth() const
{
	return this->bitDepth;
}

//------------------------------------------------------------------------------
/**
*/
inline const ImageBase::Dimension& 
ImageBase::GetDimension() const
{
	return this->dimension;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool& 
ImageBase::IsCompressed() const
{
	return this->isCompressed;
}


//------------------------------------------------------------------------------
/**
*/
inline const ImageBase::Compression& 
ImageBase::GetCompression() const
{
	return this->compression;
}

//------------------------------------------------------------------------------
/**
*/
inline const uint& 
ImageBase::GetSize() const
{
	return this->size;
}

//------------------------------------------------------------------------------
/**
*/
inline void* 
ImageBase::GetData() const
{
	return this->data;
}
} // namespace Image
//------------------------------------------------------------------------------