#pragma once
//------------------------------------------------------------------------------
/**
    @class Header
    
    Containts settings neccessary to compile an AnyFX file.
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include <string>
#include <vector>
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

	enum Flags
	{
		NoFlags = 0 << 0,
		NoSubroutines = 1 << 1,					// tell compiler to convert used subroutines into new shader programs instead
		PutGlobalVariablesInBlock = 1 << 2,		// tell compiler to put variables outside variable buffer blocks into a global block, named GlobalBlock

		NumFlags
	};

	/// constructor
	Header();
	/// destructor
	virtual ~Header();

	/// sets the effect profile
	void SetProfile(const std::string& profile);
	/// set the effect defines
	void SetFlags(const std::vector<std::string>& defines);
	/// get the effect type, is extracted from the type
	const Type& GetType() const;

	/// get compiler flags
	const int& GetFlags() const;

	/// gets the profile major number
	int GetMajor() const;
	/// gets the profile minor number
	int GetMinor() const;

	/// typecheck header
	void TypeCheck(TypeChecker& typechecker);
	/// compile header
	void Compile(BinWriter& writer);

private:
	std::string profile;
	int major;
	int minor;
	Type type;
	int flags;
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
inline const int&
Header::GetFlags() const
{
	return this->flags;
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
