//------------------------------------------------------------------------------
//  bitreader.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "bitreader.h"
#include "BitStream.h"
#include "RakString.h"

namespace Multiplayer
{
__ImplementClass(Multiplayer::BitReader, 'IMBR', IO::StreamReader);

using namespace Util;
using namespace System;
using namespace Math;
using namespace RakNet;


//------------------------------------------------------------------------------
/**
*/
BitReader::BitReader() : 
	stream(NULL)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
BitReader::~BitReader()
{
}


//------------------------------------------------------------------------------
/**
*/
char
BitReader::ReadChar()
{
    unsigned char out;
	this->stream->Read(out);    
    return (char)out;
}

//------------------------------------------------------------------------------
/**
*/
unsigned char
BitReader::ReadUChar()
{
    unsigned char out;
	this->stream->Read(out);
    return out;
}

//------------------------------------------------------------------------------
/**
*/
short
BitReader::ReadShort()
{
    short out;
	this->stream->Read(out);
    return out;
}

//------------------------------------------------------------------------------
/**
*/ 
unsigned short
BitReader::ReadUShort()
{
    unsigned short out;
	this->stream->Read(out);
    return out;
}

//------------------------------------------------------------------------------
/**
*/
int
BitReader::ReadInt()
{
    int out;
	this->stream->Read(out);
    return out;
}

//------------------------------------------------------------------------------
/**
*/
unsigned int
BitReader::ReadUInt()
{
    unsigned int out;
	this->stream->Read(out);
    return out;
}

//------------------------------------------------------------------------------
/**
*/
float
BitReader::ReadFloat()
{
    float out;
	this->stream->Read(out);
    return out;
}   

//------------------------------------------------------------------------------
/**
*/
float
BitReader::ReadFloatFromNormalizedUByte2()
{
	float result;
	this->stream->ReadCompressed(result);    
    return result;
}

//------------------------------------------------------------------------------
/**
*/
float
BitReader::ReadFloatFromUnsignedNormalizedUByte2()
{
	float result;
	this->stream->ReadFloat16(result, 0.0f, 1.0f);    
    return result;
}

//------------------------------------------------------------------------------
/**
*/
double
BitReader::ReadDouble()
{
    double out;
	this->stream->Read(out);
    return out;
}

//------------------------------------------------------------------------------
/**
*/
bool
BitReader::ReadBool()
{    
	bool out;
	this->stream->Read(out);    
    return out;
}

//------------------------------------------------------------------------------
/**
*/
Util::String
BitReader::ReadString()
{
	RakString ret;
	this->stream->Read(ret);
	Util::String string(ret.C_String());
    return string;
}

//------------------------------------------------------------------------------
/**
*/
Util::Blob
BitReader::ReadBlob()
{
    // read size
    ushort size = this->ReadUShort();
	Util::Blob blob;
	blob.Reserve(size);
	this->stream->Read((char*)blob.GetPtr(), size);
	return blob;
}

//------------------------------------------------------------------------------
/**
*/
Util::Guid
BitReader::ReadGuid()
{
    Util::Blob blob = this->ReadBlob();
    return Util::Guid::FromBinary((const unsigned char*)blob.GetPtr(), blob.Size());
}

//------------------------------------------------------------------------------
/**
*/
Multiplayer::UniquePlayerId
BitReader::ReadPlayerID()
{
	Multiplayer::UniquePlayerId id;
	RakNetGUID guid;
	stream->Read(guid);
	id.SetRaknetGuid(guid);
	return id;
}

//------------------------------------------------------------------------------
/**
*/
Math::float2 
BitReader::ReadFloat2()
{
	Math::float2 f(this->ReadFloat(), this->ReadFloat());    
    return f;
}

//------------------------------------------------------------------------------
/**
*/
Math::float4
BitReader::ReadFloat4()
{    
    Math::float4 f;
    f.x() = this->ReadFloat();
    f.y() = this->ReadFloat();
    f.z() = this->ReadFloat();
    f.w() = this->ReadFloat();
    return f;
}  

//------------------------------------------------------------------------------
/**
*/
Math::point 
BitReader::ReadPoint()
{
    Math::point f;
    f.x() = this->ReadFloat();
    f.y() = this->ReadFloat();
    f.z() = this->ReadFloat();
    return f;
}

//------------------------------------------------------------------------------
/**
*/
Math::vector 
BitReader::ReadVector()
{
    Math::vector f;
    f.x() = this->ReadFloat();
    f.y() = this->ReadFloat();
    f.z() = this->ReadFloat();
    return f;
}

//------------------------------------------------------------------------------
/**
*/
Math::vector
BitReader::ReadNormal()
{
	float x, y, z;
	this->stream->ReadNormVector(x, y, z);
	Math::vector f(x, y, z);	
	return f;
}

//------------------------------------------------------------------------------
/**
*/
Math::matrix44
BitReader::ReadMatrix44()
{
    Math::matrix44 m;
    m.setrow0(this->ReadFloat4());
    m.setrow1(this->ReadFloat4());
    m.setrow2(this->ReadFloat4());
    m.setrow3(this->ReadFloat4());
    return m;
}

//------------------------------------------------------------------------------
/**
*/
Math::matrix44
BitReader::ReadOrthogonal()
{
	float m00, m01, m02, m10, m11, m12, m20, m21, m22;
	this->stream->ReadOrthMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);
	Math::matrix44 m;
	m.setrow0(float4(m00, m01, m02, 0.0f));
	m.setrow1(float4(m10, m11, m12, 0.0f));
	m.setrow2(float4(m20, m21, m22, 0.0f));
	return m;
}

//------------------------------------------------------------------------------
/**
*/
bool 
BitReader::ReadBit()
{
    return this->ReadBool();
}

//------------------------------------------------------------------------------
/**
*/
bool
BitReader::ReadRawData(void* ptr, SizeT numBytes)
{
	return this->stream->Read((char*)ptr, numBytes);    
}

//------------------------------------------------------------------------------
/**
*/
bool
BitReader::ReadRawBitData( unsigned char* ptr, SizeT numBits )
{
	return this->stream->ReadBits(ptr, numBits);    
}

//------------------------------------------------------------------------------
/**
*/
void 
BitReader::SetStream(RakNet::BitStream *s)
{    
    this->stream = s;
}


//------------------------------------------------------------------------------
/**
*/
void 
BitReader::IgnoreBits(SizeT numBits)
{
	this->stream->IgnoreBits(numBits);
}

//------------------------------------------------------------------------------
/**
*/
bool
BitReader::CanRead() const
{
	return true;
}

//------------------------------------------------------------------------------
/**
*/
bool
BitReader::Open()
{
	return IO::Stream::Open();
}

//------------------------------------------------------------------------------
/**
*/
void
BitReader::Close()
{
	IO::Stream::Close();
}

//------------------------------------------------------------------------------
/**
*/
IO::Stream::Size
BitReader::Read(void* ptr, IO::Stream::Size numBytes)
{
	if (this->ReadRawData(ptr, numBytes))
	{
		return numBytes;
	}
	else
	{
		return 0;
	}
}


} // namespace Multiplayer
