//------------------------------------------------------------------------------
//  resourcelump.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/resourcelump.h"
#include "resources/resource.h"

namespace Resources
{
__ImplementClass(Resources::ResourceLump, 'RSLP', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
ResourceLump::ResourceLump() : 
    arenaStartAddress(0),
    firstBlockIndex(InvalidIndex),
    numBlocks(0),
    blockSize(0),
    isLocked(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ResourceLump::~ResourceLump()
{
    n_assert(!this->IsBound());
    n_assert(!this->IsValid());
    n_assert(!this->IsLocked());
}

//------------------------------------------------------------------------------
/**
    Setup the resource lump. This just associates the lump with a 
    range in the resource memory arena, it does NOT bind the lump
    to an actual resource.
*/
void
ResourceLump::Setup(void* arenaStartAddress_, IndexT firstBlockIndex_, SizeT numBlocks_, SizeT blockSize_)
{
    n_assert(!this->IsValid());
    n_assert(!this->IsBound());
    n_assert((arenaStartAddress_ != 0) && (firstBlockIndex_ >= 0) && (numBlocks_ > 0) && (blockSize_ > 0));

    this->arenaStartAddress = (uchar*) arenaStartAddress_;
    this->firstBlockIndex = firstBlockIndex_;
    this->numBlocks = numBlocks_;
    this->blockSize = blockSize_;
    this->isLocked = false;
}

//------------------------------------------------------------------------------
/**
    Resize an unbound lump.
*/
void
ResourceLump::Resize(SizeT newNumBlocks)
{
    n_assert(this->IsValid());
    n_assert(!this->IsBound());
    n_assert(!this->IsLocked());
    n_assert(newNumBlocks > 0);
    this->numBlocks = newNumBlocks;
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceLump::Discard()
{
    n_assert(this->IsValid());
    n_assert(!this->IsBound());
    n_assert(!this->IsLocked());
    this->arenaStartAddress = 0;
    this->firstBlockIndex = InvalidIndex;
    this->numBlocks = 0;
    this->blockSize = 0;
}

//------------------------------------------------------------------------------
/**
*/
bool
ResourceLump::IsBound() const
{
    return this->resource.isvalid();
}

//------------------------------------------------------------------------------
/**
    Bind the lump to a specific resource id. This marks the lump as reserved.
*/
void
ResourceLump::Bind(const Ptr<Resource>& res)
{
    n_assert(res.isvalid());
    n_assert(this->IsValid());
    n_assert(!this->IsLocked());
    n_assert(!this->IsBound());
    this->resource = res;
}

//------------------------------------------------------------------------------
/**
    Unbind the lump from its resource. This marks the lump as free.
*/
void
ResourceLump::Unbind()
{
    n_assert(this->IsValid());
    n_assert(this->IsBound());
    n_assert(!this->IsLocked());
    this->resource = 0;
}

//------------------------------------------------------------------------------
/**
    Get the resource that is currently bound to the lump.
*/
const Ptr<Resource>&
ResourceLump::GetBoundResource() const
{
    n_assert(this->IsBound());
    return this->resource;
}

//------------------------------------------------------------------------------
/**
    Lock a bound resource lump in place. A locked lump cannot be moved
    around. Lumps are usually locked while a IO request to load the
    lump's data is pending.
*/
void
ResourceLump::Lock()
{
    n_assert(this->IsValid());
    n_assert(this->IsBound());
    n_assert(!this->IsLocked());
    this->isLocked = true;
}

//------------------------------------------------------------------------------
/**
    Unlock a locked lump.
*/
void
ResourceLump::Unlock()
{
    n_assert(this->IsValid());
    n_assert(this->IsBound());
    n_assert(this->IsLocked());
    this->isLocked = false;
}

//------------------------------------------------------------------------------
/**
    This method is called when a resource lump has been moved by
    the resource allocator.
*/
void
ResourceLump::OnMoved(IndexT newFirstBlockIndex)
{
    n_assert(this->IsValid());
    n_assert(!this->IsLocked());
    this->firstBlockIndex = newFirstBlockIndex;
}

} // namespace Resources
