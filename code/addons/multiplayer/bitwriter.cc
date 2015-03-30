//------------------------------------------------------------------------------
//  bitwriter.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "bitwriter.h"
#include "BitStream.h"
#include "RakString.h"

namespace Multiplayer
{
__ImplementClass(Multiplayer::BitWriter, 'IMBW', Core::RefCounted);

using namespace Util;
using namespace System;
using namespace Math;
using namespace RakNet;

//------------------------------------------------------------------------------
/**
*/
BitWriter::BitWriter() : 
	stream(NULL)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
BitWriter::~BitWriter()
{
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteChar(char c)
{
	this->stream->Write(c);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteUChar(unsigned char c)
{   
	this->stream->Write(c);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteShort(short s)
{
	this->stream->Write(s);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteUShort(unsigned short s)
{
	this->stream->Write(s);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteInt(int i)
{
	this->stream->Write(i);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteUInt(unsigned int i)
{    
	this->stream->Write(i);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteFloat(float f)
{
	this->stream->Write(f);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteFloatAsNormalizedUByte2(float f)
{
    n_assert(f >= -1.0f && f <= 1.0f);
	this->stream->WriteCompressed(f);
}

//------------------------------------------------------------------------------
/**
*/
void 
BitWriter::WriteFloatAsUnsignedNormalizedUByte2(float f)
{
    n_assert(f >= 0.0f && f <= 1.0f);
	this->stream->WriteFloat16(f, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteDouble(double d)
{
	this->stream->Write(d);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteBool(bool b)
{
	this->stream->Write(b);
}

//------------------------------------------------------------------------------
/**    
*/
void
BitWriter::WriteString(const Util::String& s)
{
	RakString rs = s.AsCharPtr();
	this->stream->Write(rs);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteBlob(const Util::Blob& blob)
{
    const char* blobT = (const char*)blob.GetPtr();
    this->WriteUShort((ushort)blob.Size());
	this->stream->Write(blobT, blob.Size());    
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteGuid(const Util::Guid& guid)
{
    const unsigned char* ptr;
    SizeT size = guid.AsBinary(ptr);
    Util::Blob blob(ptr, size);
    this->WriteBlob(blob);
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WritePlayerID(const Multiplayer::UniquePlayerId & id)
{
	this->stream->Write(id.GetRaknetGuid());
}

//------------------------------------------------------------------------------
/**
*/
void 
BitWriter::WriteFloat2(Math::float2 f)
{
    this->WriteFloat(f.x());
    this->WriteFloat(f.y());
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteFloat4(const float4& v)
{
    this->WriteFloat(v.x());
    this->WriteFloat(v.y());
    this->WriteFloat(v.z());
    this->WriteFloat(v.w());
}    

//------------------------------------------------------------------------------
/**
*/
void 
BitWriter::WritePoint(const Math::point& v)
{
    this->WriteFloat(v.x());
    this->WriteFloat(v.y());
    this->WriteFloat(v.z());
}

//------------------------------------------------------------------------------
/**
*/
void 
BitWriter::WriteVector(const Math::vector& v)
{
    this->WriteFloat(v.x());
    this->WriteFloat(v.y());
    this->WriteFloat(v.z());
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteNormal(const Math::vector& v)
{
	this->stream->WriteNormVector(v.x(), v.y(), v.z());
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteMatrix44(const matrix44& m)
{
    this->WriteFloat4(m.getrow0());
    this->WriteFloat4(m.getrow1());
    this->WriteFloat4(m.getrow2());
    this->WriteFloat4(m.getrow3());
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteOrthogonal(const matrix44& m)
{	
	const float4 &r0 = m.getrow0();
	const float4 &r1 = m.getrow1();
	const float4 &r2 = m.getrow2();
	this->stream->WriteOrthMatrix(r0.x(), r0.y(), r0.z(), r1.x(), r1.y(), r1.z(), r2.x(), r2.y(), r2.z());
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::WriteRawData(const void* ptr, SizeT numBytes)
{
	this->stream->Write((char*)ptr, numBytes);
}

//------------------------------------------------------------------------------
/**
*/
void 
BitWriter::WriteBit( bool b )
{
	this->WriteBool(b);
}

//------------------------------------------------------------------------------
/**
*/
void 
BitWriter::WriteRawBitData( const unsigned char* ptr, SizeT numBits )
{
	this->stream->WriteBits(ptr, numBits);
}

//------------------------------------------------------------------------------
/**
*/
void 
BitWriter::SetStream( RakNet::BitStream * s)
{
    this->stream = s;
}

//------------------------------------------------------------------------------
/**
*/
bool
BitWriter::CanWrite() const
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
BitWriter::Open()
{
	return IO::Stream::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::Close()
{
	IO::Stream::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
BitWriter::Write(const void* ptr, Size numBytes)
{
	this->WriteRawData(ptr, numBytes);
}

} // namespace Multiplayer
