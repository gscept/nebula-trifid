#pragma once
//------------------------------------------------------------------------------
/**
    @class Multiplayer::UniquePlayerIdBase

    

	(C) 2009 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/    
//------------------------------------------------------------------------------
#include "core/types.h"
#include "network/bitreader.h"
#include "network/bitwriter.h"

namespace Base
{
class UniquePlayerIdBase
{
public:
    /// constructor
    UniquePlayerIdBase();
    /// destructor
    virtual ~UniquePlayerIdBase();
    /// encode id into a stream
    virtual void Encode(const Ptr<Multiplayer::BitWriter>& writer) const;
    /// decode id from a stream
    virtual void Decode(const Ptr<Multiplayer::BitReader>& reader);

protected:
};

} // namespace Multiplayer
//-------------------