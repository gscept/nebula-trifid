//------------------------------------------------------------------------------
//  header.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "header.h"
#include "util.h"
#include "typechecker.h"

namespace AnyFX
{

//------------------------------------------------------------------------------
/**
*/
Header::Header() : 
	type(InvalidType)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Header::~Header()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
Header::SetProfile( const std::string& profile )
{
	if (profile == "dx11")
	{
		this->type = HLSL;
		this->major = 5;
		this->minor = 0;
	}
	else if (profile == "dx10")
	{
		this->type = HLSL;
		this->major = 4;
		this->minor = 0;
	}
	else if (profile == "dx9")
	{
		this->type = HLSL;
		this->major = 3;
		this->minor = 0;
	}
	else if (profile == "gl45")
	{
		this->type = GLSL;
		this->major = 4;
		this->minor = 5;
	}
	else if (profile == "gl44")
	{
		this->type = GLSL;
		this->major = 4;
		this->minor = 4;
	}
	else if (profile == "gl43")
	{
		this->type = GLSL;
		this->major = 4;
		this->minor = 3;
	}
	else if (profile == "gl42")
	{
		this->type = GLSL;
		this->major = 4;
		this->minor = 2;
	}
	else if (profile == "gl41")
	{
		this->type = GLSL;
		this->major = 4;
		this->minor = 1;
	}
	else if (profile == "gl40")
	{
		this->type = GLSL;
		this->major = 4;
		this->minor = 0;
	}
	else if (profile == "gl33")
	{
		this->type = GLSL;
		this->major = 3;
		this->minor = 3;
	}
	else if (profile == "gl32")
	{
		this->type = GLSL;
		this->major = 3;
		this->minor = 2;
	}
	else if (profile == "gl31")
	{
		this->type = GLSL;
		this->major = 3;
		this->minor = 1;
	}
	else if (profile == "gl30")
	{
		this->type = GLSL;
		this->major = 3;
		this->minor = 0;
	}
	else if (profile == "gl21")
	{
		this->type = GLSL;
		this->major = 2;
		this->minor = 1;
	}
	else if (profile == "gl20")
	{
		this->type = GLSL;
		this->major = 2;
		this->minor = 0;
	}
	else if (profile == "gl15")
	{
		this->type = GLSL;
		this->major = 1;
		this->minor = 5;
	}
	else if (profile == "gl14")
	{
		this->type = GLSL;
		this->major = 1;
		this->minor = 4;
	}
	else if (profile == "gl13")
	{
		this->type = GLSL;
		this->major = 1;
		this->minor = 3;
	}
	else if (profile == "gl121")
	{
		this->type = GLSL;
		this->major = 1;
		this->minor = 21;
	}
	else if (profile == "gl12")
	{
		this->type = GLSL;
		this->major = 1;
		this->minor = 2;
	}
	else if (profile == "gl11")
	{
		this->type = GLSL;
		this->major = 1;
		this->minor = 1;
	}
	else if (profile == "gl10")
	{
		this->type = GLSL;
		this->major = 1;
		this->minor = 0;
	}
	else if (profile == "ps4")
	{
		this->type = PS;
		this->major = 2;
	}
	else if (profile == "ps3")
	{
		this->type = PS;
		this->major = 1;
	}
	else if (profile == "wii2")
	{
		this->type = Wii;
		this->major = 2;
	}
	else if (profile == "wii1")
	{
		this->type = Wii;
		this->major = 1;
	}
	else
	{
		this->type = InvalidType;
		this->major = -1;
		this->minor = -1;
	}
	this->profileName = profile;
}

//------------------------------------------------------------------------------
/**
*/
void 
Header::TypeCheck( TypeChecker& typechecker )
{
	if (this->type == InvalidType)
	{
		std::string message = Format("Profile '%s' is not supported\n", this->profileName.c_str());
		typechecker.Error(message);
	}
}

//------------------------------------------------------------------------------
/**
*/
void
Header::Compile( BinWriter& writer )
{
	writer.WriteInt('HEAD');
	writer.WriteInt(this->type);
	writer.WriteInt(this->major);
	writer.WriteInt(this->minor);
}

} // namespace AnyFX
