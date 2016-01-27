//------------------------------------------------------------------------------
//  binwriter.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "binwriter.h"
#include <assert.h>
#include <sstream>
#include <algorithm>

static char encryptionTable[] = {'A', 'T', 'C', 'G', 'F', 'F', 'W', 'I', 'T', 'C', 'H', 'E', 'R'};
namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
BinWriter::BinWriter() :
	isOpen(false)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BinWriter::~BinWriter()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool
BinWriter::Open()
{
	assert(this->path.size() > 0);
	this->output.open(this->path.c_str(), std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
	this->isOpen = this->output.is_open();
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
void 
BinWriter::Close()
{
	assert(this->isOpen);
	this->output.close();
}

//------------------------------------------------------------------------------
/**
*/
void 
BinWriter::WriteInt( int val )
{
	// convert to char* and write
	this->output.write((const char*)&val, sizeof(int));
}

//------------------------------------------------------------------------------
/**
*/
void 
BinWriter::WriteUInt( unsigned val )
{
	// convert to char* and write
	this->output.write((const char*)&val, sizeof(unsigned));
}

//------------------------------------------------------------------------------
/**
*/
void 
BinWriter::WriteBool( bool val )
{
	// convert to char* and write
	this->output.write((const char*)&val, sizeof(bool));
}

//------------------------------------------------------------------------------
/**
*/
void 
BinWriter::WriteFloat( float val )
{
	// convert to char* and write
	this->output.write((const char*)&val, sizeof(float));
}

//------------------------------------------------------------------------------
/**
*/
void 
BinWriter::WriteDouble( double val )
{
	// convert to char* and write
	this->output.write((const char*)&val, sizeof(double));
}

//------------------------------------------------------------------------------
/**
*/
void 
BinWriter::WriteShort( short val )
{
	// convert to char* and write
	this->output.write((const char*)&val, sizeof(short));
}

//------------------------------------------------------------------------------
/**
*/
void
BinWriter::WriteString(const std::string& s)
{
	// copy string
	std::string copy = s;

	// write size, then string contents	
	size_t len = s.size();

	// write length of string
	this->WriteUInt(len);

	// encrypt characters
	unsigned i;
	for (i = 0; i < len; i++)
	{
		copy[i] = copy[i] ^ encryptionTable[i % sizeof(encryptionTable)];
	}

	// write string
	this->output.write(copy.c_str(), copy.length());
}

//------------------------------------------------------------------------------
/**
*/
void
BinWriter::WriteChar(char c)
{
	this->output.put(c);
}

} // namespace AnyFX