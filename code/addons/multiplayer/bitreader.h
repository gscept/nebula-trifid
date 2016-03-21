#pragma once
//------------------------------------------------------------------------------
/**
	@class Multiplayer::BitReader

	Wrapper around RakNet's BitStream

	(C) 2015-2016 Individual contributors, see AUTHORS file
*/

#include "core/refcounted.h"
#include "util/guid.h"
#include "util/blob.h"
#include "uniqueplayerid.h"
#include "io/streamreader.h"

namespace RakNet
{
	class BitStream;
}

//------------------------------------------------------------------------------
namespace Multiplayer
{
class BitReader : public IO::Stream
{
    __DeclareClass(BitReader);
public:
    /// constructor
    BitReader();
    /// destructor
    virtual ~BitReader();

    /// set stream to write to
    void SetStream(RakNet::BitStream * s);    

    /// read an 8-bit char from the stream
    char ReadChar();
    /// read an 8-bit unsigned character from the stream
    unsigned char ReadUChar();
    /// read a 16-bit short from the stream
    short ReadShort();
    /// read a 16-bit unsigned short from the stream
    unsigned short ReadUShort();
    /// read a 32-bit int from the stream
    int ReadInt();
    /// read a 32-bit unsigned int from the stream
    unsigned int ReadUInt();
    /// read a float value from the stream
    float ReadFloat();
    /// read a compressed float value from the stream, lossy and needed to be in the range of -1.0 and +1.0
    float ReadFloatFromNormalizedUByte2();
    /// read a compressed float value from the stream, lossy and needed to be in the range of 0.0 and +1.0
    float ReadFloatFromUnsignedNormalizedUByte2();
    /// read a double value from the stream
    double ReadDouble();
    /// read a bool value from the stream
    bool ReadBool();
    /// read a string from the stream
    Util::String ReadString();        
    /// read a float2 from the stream
    Math::float2 ReadFloat2();
    /// read a float4 from the stream
    Math::float4 ReadFloat4();
	/// read a normalized vector from the stream
	Math::vector ReadNormal();
    /// read a point from the stream, (x,y,z,1.0)
    Math::point ReadPoint();
    /// read a vector from the stream, (x,y,z,0.0)
    Math::vector ReadVector();
    /// read a matrix44 from the stream
    Math::matrix44 ReadMatrix44();
	/// read orthogonal matrix from the stream, will only affect rotational part!
	Math::matrix44 ReadOrthogonal();
    /// read a guid
    Util::Guid ReadGuid();
	/// read a unique player id
	Multiplayer::UniquePlayerId ReadPlayerID();
    /// read a blob of data
    Util::Blob ReadBlob();
    
    /// read a single bit
    bool ReadBit();
    /// read raw data
	bool ReadRawData(void* ptr, SizeT numBytes);
    /// read raw bit data
	bool ReadRawBitData(unsigned char* ptr, SizeT numBits);
    /// ignore bits
    void IgnoreBits(SizeT numBits);

	/// implement stream interface for use with message serialization
	/// return true if the stream supports reading
	virtual bool CanRead() const;	
	/// open the stream
	virtual bool Open();
	/// close the stream
	virtual void Close();
	/// directly read from the stream
	virtual IO::Stream::Size Read(void* ptr, IO::Stream::Size numBytes);

private:
	RakNet::BitStream * stream;
};

} // namespace Multiplayer
//------------------------------------------------------------------------------
