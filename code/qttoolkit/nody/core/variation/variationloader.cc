//------------------------------------------------------------------------------
//  variationloader.cc
//  (C) 2012 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "variationloader.h"

namespace Nody
{
__ImplementClass(Nody::VariationLoader, 'VALO', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
VariationLoader::VariationLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VariationLoader::~VariationLoader()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
Ptr<Variation>
VariationLoader::Load( const Ptr<IO::Stream>& stream )
{
	// implement in subclass
	return NULL;
}

} // namespace Nody