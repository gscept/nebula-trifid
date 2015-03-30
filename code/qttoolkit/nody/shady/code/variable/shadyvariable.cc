//------------------------------------------------------------------------------
//  shadyvariable.cc
//  (C) 2014 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "shadyvariable.h"

namespace Shady
{
__ImplementClass(Shady::ShadyVariable, 'SVAR', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ShadyVariable::ShadyVariable()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShadyVariable::~ShadyVariable()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
ShadyVariable::Result 
ShadyVariable::ResultFromString( const Util::String& string )
{
    if (string == "vertex")         return VertexResult;
    else if (string == "hull")      return HullResult;
    else if (string == "domain")    return HullResult;
    else if (string == "geometry")  return GeometryResult;
    else if (string == "pixel")     return PixelResult;
    else
    {
        n_error("Unknown input result '%s'", string.AsCharPtr());
        return UnknownResult;
    }
}

} // namespace Shady