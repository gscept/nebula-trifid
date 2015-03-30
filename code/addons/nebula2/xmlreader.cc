//------------------------------------------------------------------------------
//  xmlreader.cc
//  (C) 2010 RadonLabs GmbH
//------------------------------------------------------------------------------

#include "stdneb.h"

// nebula3 includes
#include "io/xmlreader.h"
#include "util/string.h"

// nebula2 includes
#include "mathlib/vector.h"
#include "util/nstring.h"

using namespace Util;

//------------------------------------------------------------------------------
/**
    Get the attribute into a nebula2 string
*/
template<>
nString
IO::XmlReader::Get(const char* attr) const
{
	return nString(this->GetString(attr).AsCharPtr());
}

//------------------------------------------------------------------------------
/**
    Get the attribute into a nebula2 vector3
*/
template<>
vector3 
IO::XmlReader::Get(const char* attr) const
{
	const String& float3String = this->GetString(attr);
	const Array<String> tokens = float3String.Tokenize(", \t");
	n_assert(tokens.Size() == 3);
	return vector3(tokens[0].AsFloat(), tokens[1].AsFloat(), tokens[2].AsFloat());
}

//------------------------------------------------------------------------------
/**
    Get the attribute into a nebula2 vector4
*/
template<>
vector4 
IO::XmlReader::Get(const char* attr) const
{
	const String& float4String = this->GetString(attr);
	const Array<String> tokens = float4String.Tokenize(", \t");
	n_assert(tokens.Size() == 4);
	return vector4(tokens[0].AsFloat(), tokens[1].AsFloat(), tokens[2].AsFloat(), tokens[3].AsFloat());
}

//------------------------------------------------------------------------------
/**
*/
template<>
nFloat4
IO::XmlReader::Get(const char* attr) const
{
    const String& float4String = this->GetString(attr);
    const Array<String> tokens = float4String.Tokenize(", \t");
    n_assert(tokens.Size() == 4);
    nFloat4 f4;
    f4.x = tokens[0].AsFloat();
    f4.y = tokens[1].AsFloat();
    f4.z = tokens[2].AsFloat();
    f4.w = tokens[3].AsFloat();
    return f4;
}
