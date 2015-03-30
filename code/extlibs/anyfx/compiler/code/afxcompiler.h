//------------------------------------------------------------------------------
/**
    AnyFX compiler header
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include <vector>


struct AnyFXErrorBlob
{
	char* buffer;
	size_t size;

	// constructor
	AnyFXErrorBlob() :
		buffer(0),
		size(0)
	{
		// empty
	}

	// destructor
	~AnyFXErrorBlob()
	{
		if (buffer) delete [] buffer;
	}
};

extern bool AnyFXCompile(const std::string& file, const std::string& output, const std::string& target, const std::string& vendor, const std::vector<std::string>& defines, AnyFXErrorBlob** errorBuffer);
extern void AnyFXBeginCompile();
extern void AnyFXEndCompile();
