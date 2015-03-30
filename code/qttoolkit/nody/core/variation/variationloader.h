#pragma once
//------------------------------------------------------------------------------
/**
    @class Nody::VariationLoader
    
    A VariationLoader can be inherited to provide extensive loading information. 
	The basic loader (this) just loads inputs/outputs/constants/parameters
    
    (C) 2012 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "io/stream.h"
#include "variation.h"
namespace Nody
{
class VariationLoader : public Core::RefCounted
{
	__DeclareClass(VariationLoader);
public:
	/// constructor
	VariationLoader();
	/// destructor
	virtual ~VariationLoader();

	/// loads a variation
	virtual Ptr<Variation> Load(const Ptr<IO::Stream>& stream);
}; 
} // namespace Nody
//------------------------------------------------------------------------------