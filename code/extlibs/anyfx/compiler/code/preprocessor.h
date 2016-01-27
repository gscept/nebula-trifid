#pragma once
//------------------------------------------------------------------------------
/**
    @class AnyFX::Preprocessor
    
    Contains GPP preprocessor information
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
namespace AnyFX
{
class Preprocessor
{
public:
	/// constructor
	Preprocessor();
	/// destructor
	virtual ~Preprocessor();

	/// sets contents
	void SetContents(const std::string& contents);

private:
	std::string contents;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void
Preprocessor::SetContents(const std::string& contents)
{
	this->contents = contents;
}

} // namespace AnyFX
//------------------------------------------------------------------------------