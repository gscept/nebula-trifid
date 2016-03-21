#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::SuperVariationLoader
    
    Loads a super variation from super veriation declaration. 
    This class should be overrided using any application specific loading.
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "io/stream.h"
#include "supervariation.h"
namespace Nody
{
class SuperVariationLoader : public Core::RefCounted
{
	__DeclareClass(SuperVariationLoader);
public:
	/// constructor
	SuperVariationLoader();
	/// destructor
	virtual ~SuperVariationLoader();

    /// load variation
    virtual Ptr<SuperVariation> Load(const Ptr<IO::Stream>& stream);
}; 
} // namespace Nody
//------------------------------------------------------------------------------