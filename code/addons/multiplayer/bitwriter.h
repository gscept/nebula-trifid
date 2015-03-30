#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::BitWriter
    
    Wrapper around RakNet's BitStream    
    
    (C) 2015 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"
#include "util/guid.h"
#include "util/blob.h"
#include "uniqueplayerid.h"
#include "io/stream.h"

namespace RakNet
{
	class BitStream;
}

//------------------------------------------------------------------------------
namespace Multiplayer
{
class BitWriter : public IO::Stream
{
    __DeclareClass(BitWriter);
public:
    /// constructor
    BitWriter();
    /// destructor
    virtual ~BitWriter();

    /// set bitstream
	void SetStream(RakNet::BitStream * stream);

    /// write an 8-bit char to the stream
    void WriteChar(char c);
    /// write an 8-bit unsigned char to the stream
    void WriteUChar(unsigned char c);
    /// write an 16-bit short to the stream
    void WriteShort(short s);
    /// write an 16-bit unsigned short to the stream
    void WriteUShort(unsigned short s);
    /// write an 32-bit int to the stream
    void WriteInt(int i);
    /// write an 32-bit unsigned int to the stream
    void WriteUInt(unsigned int i);
    /// write a float value to the stream
    void WriteFloat(float f);
    /// write a compressed float value to the stream, lossy and needed to be in the range of -1.0 and +1.0
    void WriteFloatAsNormalizedUByte2(float f);
    /// write a compressed float value to the stream, lossy and needed to be in the range of 0.0 and +1.0
    void WriteFloatAsUnsignedNormalizedUByte2(float f);
    /// write a double value to the stream
    void WriteDouble(double d);
    /// write a boolean value to the stream
    void WriteBool(bool b);
    /// write a string to the stream
    void WriteString(const Util::String& s);
    /// write a float value to the stream    
    void WriteFloat2(Math::float2 f);
    /// write a float4 to the stream
    void WriteFloat4(const Math::float4& v);
	/// write a normalized vector to the stream
	void WriteNormal(const Math::vector& v);
    /// write a float4 to the stream
    void WritePoint(const Math::point& v);
    /// write a float4 to the stream
    void WriteVector(const Math::vector& v);
    /// write a matrix44 to the stream
    void WriteMatrix44(const Math::matrix44& m);
	/// write orthogonal matrix to stream, will only serialize rotational part!
	void WriteOrthogonal(const Math::matrix44& m);
    /// write a guid
    void WriteGuid(const Util::Guid& guid);
	/// write a unique player id
	void WritePlayerID(const Multiplayer::UniquePlayerId & id);
    /// write a blob of data
    void WriteBlob(const Util::Blob& blob);

    /// write a single bit
    void WriteBit(bool b);
    /// write raw data
    void WriteRawData(const void* ptr, SizeT numBytes);
    /// write raw data bitwise
    void WriteRawBitData(const unsigned char* ptr, SizeT numBits);

	/// implement stream interface for use with message serialization
	/// return true if the stream supports reading
	virtual bool CanWrite() const;
	/// open the stream
	virtual bool Open();
	/// close the stream
	virtual void Close();
	/// directly write to the stream
	virtual void Write(const void* ptr, Size numBytes);

private:
	RakNet::BitStream * stream;
};


} // namespace Multiplayer
//------------------------------------------------------------------------------
