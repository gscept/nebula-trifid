//------------------------------------------------------------------------------
//  supervariationloader.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "supervariationloader.h"

namespace Nody
{
__ImplementClass(Nody::SuperVariationLoader, 'SYVL', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
SuperVariationLoader::SuperVariationLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
SuperVariationLoader::~SuperVariationLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<SuperVariation> 
SuperVariationLoader::Load( const Ptr<IO::Stream>& stream )
{
    // implement in subclass
    return NULL;
}
} // namespace Nody