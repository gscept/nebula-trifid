//------------------------------------------------------------------------------
//  emptyresourceloader.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "emptyresourceloader.h"

namespace Vegetation
{
    __ImplementClass(Vegetation::EmptyResourceLoader, 'EMRL', Resources::ResourceLoader);

//------------------------------------------------------------------------------
/**    
*/
bool
EmptyResourceLoader::OnLoadRequested()
{
    return true;
}

} // end Vegetation