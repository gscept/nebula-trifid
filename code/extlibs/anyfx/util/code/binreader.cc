//------------------------------------------------------------------------------
//  binreader.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "binreader.h"
#include <sstream>

static char encryptionTable[] = {'A', 'T', 'C', 'G', 'F', 'F', 'W', 'I', 'T', 'C', 'H', 'E', 'R'};
namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
BinReader::BinReader() :
	isOpen(false),
	inputFile(NULL),
	inputString(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
BinReader::~BinReader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
bool 
BinReader::Open()
{
	assert(this->path.size() > 0);
	this->inputFile = new std::ifstream(this->path.c_str(), std::ifstream::in | std::ifstream::binary);
	this->isOpen = this->inputFile->is_open();
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
bool 
BinReader::Open( const char* data, size_t size )
{
	this->inputString = new std::istringstream;
	this->inputString->str(std::string(data, size));
	this->isOpen = true;
	return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
void 
BinReader::Close()
{
	assert(this->isOpen);
	if (this->inputFile) 
	{
		this->inputFile->close();
		delete this->inputFile;
	}
	else if (this->inputString)
	{
		delete this->inputString;
	}
}

//------------------------------------------------------------------------------
/**
*/
int 
BinReader::ReadInt()
{
	assert(this->isOpen);
	int value;
	if (this->inputFile) this->inputFile->read((char*)&value, sizeof(int));
	else this->inputString->read((char*)&value, sizeof(int));
	return value;
}

//------------------------------------------------------------------------------
/**
*/
unsigned 
BinReader::ReadUInt()
{
	assert(this->isOpen);
	unsigned value;
	if (this->inputFile) this->inputFile->read((char*)&value, sizeof(unsigned));
	else this->inputString->read((char*)&value, sizeof(unsigned));
	return value;
}

//------------------------------------------------------------------------------
/**
*/
bool 
BinReader::ReadBool()
{
	assert(this->isOpen);
	bool value;
	if (this->inputFile) this->inputFile->read((char*)&value, sizeof(bool));
	else this->inputString->read((char*)&value, sizeof(bool));
	return value;
}

//------------------------------------------------------------------------------
/**
*/
float 
BinReader::ReadFloat()
{
	assert(this->isOpen);
	float value;
	if (this->inputFile) this->inputFile->read((char*)&value, sizeof(float));
	else this->inputString->read((char*)&value, sizeof(float));
	return value;
}

//------------------------------------------------------------------------------
/**
*/
double 
BinReader::ReadDouble()
{
	assert(this->isOpen);
	double value;
	if (this->inputFile) this->inputFile->read((char*)&value, sizeof(double));
	else this->inputString->read((char*)&value, sizeof(double));
	return value;
}

//------------------------------------------------------------------------------
/**
*/
short 
BinReader::ReadShort()
{
	assert(this->isOpen);
	short value;
	if (this->inputFile) this->inputFile->read((char*)&value, sizeof(int));
	else this->inputString->read((char*)&value, sizeof(int));
	return value;
}

//------------------------------------------------------------------------------
/**
*/
std::string 
BinReader::ReadString()
{
	assert(this->isOpen);

	// first read size
	unsigned len = this->ReadUInt();

	// create buffer
	char* buf = new char[len];

	// finally read to string
	if (this->inputFile) this->inputFile->read(buf, len);
	else this->inputString->read(buf, len);

	// decrypt using simple XOR encryption
	unsigned i;
	for (i = 0; i < len; i++)
	{
		buf[i] = buf[i] ^ encryptionTable[i % sizeof(encryptionTable)];
	}

	std::string result(buf, len);
	delete [] buf;
	return result;
}

//------------------------------------------------------------------------------
/**
*/
char 
BinReader::ReadChar()
{
	assert(this->isOpen);
	char c;
	if (this->inputFile) this->inputFile->get(c);
	else this->inputString->get(c);
	return c;
}

//------------------------------------------------------------------------------
/**
*/
void 
BinReader::Skip( unsigned n )
{
	if (this->inputFile) this->inputFile->ignore(n);
	else this->inputString->ignore(n);
}
} // namespace AnyFX