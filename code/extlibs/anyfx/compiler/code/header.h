#pragma once
//------------------------------------------------------------------------------
/**
    @class Header
    
    Containts settings neccessary to compile an AnyFX file.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include "compileable.h"
namespace AnyFX
{

class Header : public Compileable
{
public:

	enum Type
	{
		HLSL,
		GLSL,
		PS,
		Wii,

		InvalidType,

		NumTypes
	};

	/// constructor
	Header();
	/// destructor
	virtual ~Header();

	/// sets the effect profile
	void SetProfile(const std::string& profile);
	/// get the effect type
	const Type& GetType() const;

	/// gets the profile major number
	int GetMajor() const;
	/// gets the profile minor number
	int GetMinor() const;

	/// typecheck header
	void TypeCheck(TypeChecker& typechecker);
	/// compile header
	void Compile(BinWriter& writer);

private:
	std::string profileName;
	int major;
	int minor;
	Type type;
}; 


//------------------------------------------------------------------------------
/**
*/
inline const Header::Type& 
Header::GetType() const
{
	return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline int
Header::GetMajor() const
{
	return this->major;
}

//------------------------------------------------------------------------------
/**
*/
inline int
Header::GetMinor() const
{
	return this->minor;
}

} // namespace AnyFX
//------------------------------------------------------------------------------
