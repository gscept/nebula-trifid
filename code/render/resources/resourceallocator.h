#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::ResourceAllocator
    
    Base class of platform-specific resource allocators. A resource allocator
    provides custom memory allocation from a fixed area of platform specific
    resource memory. This is the base of a streaming resource management
    system where resource are streamed in and out of a fixed memory area.
    The resource manager is free to move the allocated memory blocks around
    (the addresses in the lumps will be adjusted accordingly and the lump
    will be notified).

    NOTE: The ResourceAllocator system performs some operations
    per-block, thus it's good practice to choose fewer big blocks,
    instead of many small blocks!
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file 
*/
#include "core/refcounted.h"
#include "resources/resourcelump.h"

//------------------------------------------------------------------------------
namespace Resources
{
class Resource;

class ResourceAllocator : public Core::RefCounted
{
    __DeclareClass(ResourceAllocator);
public:
    /// constructor
    ResourceAllocator();
    /// destructor
    virtual ~ResourceAllocator();
    
    /// setup the resource allocator
    virtual void Setup(SizeT blockSize, SizeT numBlocks);
    /// discard the resource allocator
    virtual void Discard();
    /// return true if the allocator has been setup
    bool IsValid() const;

    /// get the atomic block size
    SizeT GetBlockSize() const;
    /// get number of block in the resource heap
    SizeT GetNumBlocks() const;

    /// allocate a lump from the resource arena and bind it to the provided resource
    virtual Ptr<ResourceLump> AllocateLump(const Ptr<Resource>& resId, SizeT numBlocks);
    /// free a lump
    virtual void FreeLump(const Ptr<ResourceLump>& lump);
    /// compact the resource arena, returns number of continuousfree blocks
    virtual SizeT Compact();

protected:
    /// create and setup a new memory lump object
    virtual Ptr<ResourceLump> CreateLump(IndexT firstBlock, SizeT numBlocks) const;
    /// allocate the resource memory arena
    virtual void* AllocateArena(SizeT numBytes);
    /// free the resource memory arena
    virtual void FreeArena(void* ptr);
    /// memory copy method for arena memory
    virtual void CopyArenaMemory(const void* from, void* to, SizeT numBytes);

private:
    /// find the index of the "best" free lump which satisfies the requested number blocks
    IndexT FindBestFreeLumpIndex(SizeT requiredNumBlocks) const;
    /// move memory in the arena, memory may overlap!
    void MoveArenaBlocks(IndexT fromBlockIndex, IndexT toBlockIndex, SizeT numBlocksToMove);
   
    SizeT arenaBlockSize;
    SizeT arenaNumBlocks;
    uchar* arenaStartAddress;
    Util::Array<Ptr<ResourceLump> > lumps;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ResourceAllocator::GetBlockSize() const
{
    return this->arenaBlockSize;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ResourceAllocator::GetNumBlocks() const
{
    return this->arenaNumBlocks;
}

} // namespace ResourceAllocatorBas
//------------------------------------------------------------------------------
    