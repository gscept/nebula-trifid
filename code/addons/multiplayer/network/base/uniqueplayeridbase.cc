//------------------------------------------------------------------------------
//  uniqueplayeridbase.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "network/base/uniqueplayeridbase.h"

namespace Base
{
//------------------------------------------------------------------------------
/**
*/
UniquePlayerIdBase::UniquePlayerIdBase()
{

}

//------------------------------------------------------------------------------
/**
*/
UniquePlayerIdBase::~UniquePlayerIdBase()
{

}

//------------------------------------------------------------------------------
/**
*/
void 
UniquePlayerIdBase::Encode(const Ptr<Multiplayer::BitWriter>& writer) const
{
    n_error("UniquePlayerIdBase::Encode called!");
}

//------------------------------------------------------------------------------
/**
*/
void 
UniquePlayerIdBase::Decode(const Ptr<Multiplayer::BitReader>& reader)
{
    n_error("UniquePlayerIdBase::Decode called!");
}
} // namespace Multiplayer
