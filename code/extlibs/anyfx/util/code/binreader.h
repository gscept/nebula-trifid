#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::BinReader
    
    A binary reader, can read the basic data types provided in AnyFX from file.
	Also decodes AnyFX XOR:ed strings.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>

namespace AnyFX
{
class BinReader
{
public:
	/// constructor
	BinReader();
	/// destructor
	virtual ~BinReader();
	
	/// sets path to which reader shall read
	void SetPath(const std::string& path);
	/// opens reader from set path
	bool Open();
	/// opens reader on buffer
	bool Open(const char* data, size_t size);
	/// closes reader
	void Close();
	/// return true if writer is open
	bool IsOpen() const;
	/// returns true if writer is at the end of the file
	bool Eof() const;

	/// reads integer
	int ReadInt();
	/// reads unsigned integer
	unsigned ReadUInt();
	/// reads boolean
	bool ReadBool();
	/// reads float
	float ReadFloat();
	/// reads double
	double ReadDouble();
	/// reads short
	short ReadShort();
	/// reads string
	std::string ReadString();
	/// reads character
	char ReadChar();
	/// skips n characters in stream
	void Skip(unsigned n);

private:
	std::string path;
	std::ifstream* inputFile;
	std::istringstream* inputString;
	bool isOpen;
}; 


//------------------------------------------------------------------------------
/**
*/
inline void 
BinReader::SetPath( const std::string& path )
{
	this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
BinReader::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
BinReader::Eof() const
{
	assert(this->isOpen);
	if (this->inputFile) return this->inputFile->eof();
	else return this->inputString->eof();
}

} // namespace AnyFX
//------------------------------------------------------------------------------