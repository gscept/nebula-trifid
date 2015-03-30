#pragma once
//------------------------------------------------------------------------------
/**
    @class Base::ResourceLump
    
    A resource lump points to a piece of memory from a ResourceAllocator heap.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"

//------------------------------------------------------------------------------
namespace Resources
{
class Resource;

class ResourceLump : public Core::RefCounted
{
    __DeclareClass(ResourceLump);
public:
    /// constructor
    ResourceLump();
    /// destructor
    virtual ~ResourceLump();

    /// setup the lump for a specific memory range
    void Setup(void* arenaStartAddress, IndexT firstBlock, SizeT numBlocks, SizeT blockSize);
    /// resize the lump
    void Resize(SizeT newNumBlocks);
    /// lock the resource lump in place (for instance while waiting for IO)
    void Lock();
    /// unlock the resource lump
    void Unlock();
    /// discard the lump
    void Discard();
    /// return true if the lump has been setup
    bool IsValid() const;
    /// return true if the lump is currently locked
    bool IsLocked() const;

    /// bind the lump to a resource
    virtual void Bind(const Ptr<Resource>& res);
    /// unbind the lump from its resource
    virtual void Unbind();
    /// move the lump to a new memory area
    virtual void OnMoved(IndexT dstBlockIndex);
    /// return true if the lump is bound to a resource
    bool IsBound() const;

    /// get index of first block
    IndexT GetFirstBlockIndex() const;
    /// get number of block
    SizeT GetNumBlocks() const;
    /// get block size
    SizeT GetBlockSize() const;
    /// get the bound resource id
    const Ptr<Resource>& GetBoundResource() const;
    /// get start address of the lump
    void* GetStartAddress() const;
    /// get byte size of the lump
    SizeT GetByteSize() const;

private:
    /// compute address of a specific block
    uchar* ComputeBlockAddress(IndexT blockIndex) const;

    uchar* arenaStartAddress;
    IndexT firstBlockIndex;
    SizeT numBlocks;
    SizeT blockSize;
    Ptr<Resource> resource;
    bool isLocked;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourceLump::IsValid() const
{
    return (InvalidIndex != this->firstBlockIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ResourceLump::IsLocked() const
{
    return this->isLocked;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ResourceLump::GetFirstBlockIndex() const
{
    n_assert(this->IsValid());
    return this->firstBlockIndex;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ResourceLump::GetNumBlocks() const
{
    n_assert(this->IsValid());
    return this->numBlocks;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ResourceLump::GetBlockSize() const
{
    n_assert(this->IsValid());
    return this->blockSize;
}

//------------------------------------------------------------------------------
/**
*/
inline void*
ResourceLump::GetStartAddress() const
{
    n_assert(this->IsValid());
    return this->arenaStartAddress + this->firstBlockIndex * this->blockSize;
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
ResourceLump::GetByteSize() const
{
    n_assert(this->IsValid());
    return this->numBlocks * this->blockSize;
}

} // namespace Resources
//------------------------------------------------------------------------------
