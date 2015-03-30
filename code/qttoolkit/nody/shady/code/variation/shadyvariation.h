#pragma once
//------------------------------------------------------------------------------
/**
    @class Shady::ShadyVariation
    
    Implements a specialized variation which is to be used for shader generation.

    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "variation/variation.h"
namespace Shady
{
class ShadyVariation : public Nody::Variation
{
	__DeclareClass(ShadyVariation);
public:
	/// constructor
	ShadyVariation();
	/// destructor
	virtual ~ShadyVariation();
}; 


} // namespace Shady
//------------------------------------------------------------------------------