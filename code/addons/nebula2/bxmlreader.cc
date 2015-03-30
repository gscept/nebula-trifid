//------------------------------------------------------------------------------
//  bxmlreader.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"

// N3 includes
#include "io/bxmlreader.h"

// N2 includes
#include "mathlib/vector.h"

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
template<>
vector3
IO::BXmlReader::Get(const char* attr) const
{
	const String& float3String = this->GetString(attr);
	const Array<String> tokens = float3String.Tokenize(", \t");
	n_assert(tokens.Size() == 3);
	return vector3(tokens[0].AsFloat(), tokens[1].AsFloat(), tokens[2].AsFloat());
}

