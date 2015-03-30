#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::BinWriter
    
    Writes common AnyFX data types to a binary file.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <iostream>
#include <fstream>
namespace AnyFX
{
class BinWriter
{
public:
	/// constructor
	BinWriter();
	/// destructor
	virtual ~BinWriter();

	/// set output stream
	void SetPath(const std::string& path);
	/// opens writer
	bool Open();
	/// closes writer
	void Close();
	/// return true if writer is open
	bool IsOpen() const;

	/// write integer
	void WriteInt(int val);
	/// write unsigned integer
	void WriteUInt(unsigned val);
	/// write boolean
	void WriteBool(bool val);
	/// write float
	void WriteFloat(float val);
	/// write double
	void WriteDouble(double val);
	/// write short
	void WriteShort(short val);
	/// write string
	void WriteString(const std::string& s);
	/// write character
	void WriteChar(char c);


private:
	std::string path;
	std::ofstream output;
	bool isOpen;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
BinWriter::SetPath( const std::string& path )
{
	this->path = path;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
BinWriter::IsOpen() const
{
	return this->isOpen;
}

} // namespace AnyFX
//------------------------------------------------------------------------------