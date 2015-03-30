//------------------------------------------------------------------------------
//  resourceallocator.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "resources/resourceallocator.h"
#include "resources/resource.h"

namespace Resources
{
__ImplementClass(Resources::ResourceAllocator, 'RSAC', Core::RefCounted);

using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ResourceAllocator::ResourceAllocator() :
    arenaBlockSize(0),
    arenaNumBlocks(0),
    arenaStartAddress(0),
    isValid(false)
{
    this->lumps.Reserve(512);
}

//------------------------------------------------------------------------------
/**
*/
ResourceAllocator::~ResourceAllocator()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceAllocator::Setup(SizeT blockSize, SizeT numBlocks)
{
    n_assert(!this->IsValid());
    n_assert(blockSize > 0);
    n_assert(numBlocks > 0);

    this->isValid = true;
    this->arenaBlockSize = blockSize;
    this->arenaNumBlocks = numBlocks;
    this->arenaStartAddress = (uchar*) this->AllocateArena(this->arenaBlockSize * this->arenaNumBlocks);

    // setup one free lump which spans the entire memory arena
    this->lumps.Append(this->CreateLump(0, this->arenaNumBlocks));
}

//------------------------------------------------------------------------------
/**
*/
void
ResourceAllocator::Discard()
{
    n_assert(this->IsValid());

    // release free lumps
    IndexT i;
    for (i = 0; i < this->lumps.Size(); i++)
    {
        n_assert(!this->lumps[i]->IsBound());
        this->lumps[i]->Discard();
    }
    this->lumps.Clear();
    this->arenaBlockSize = 0;
    this->arenaNumBlocks = 0;
    this->FreeArena(this->arenaStartAddress);
    this->arenaStartAddress = 0;

    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
    This methods searches the freeLumps array for the lump which would waste
    the least memory for the requested block size. If absolutely no lump
    is found, the method returns InvalidIndex.
*/
IndexT
ResourceAllocator::FindBestFreeLumpIndex(SizeT requiredNumBlocks) const
{    
    // first find the first lump which can satisfy the request...
    IndexT bestLumpIndex = InvalidIndex;
    IndexT lumpIndex;
    SizeT numLumps = this->lumps.Size();
    for (lumpIndex = 0; lumpIndex < numLumps; lumpIndex++)
    {
        const Ptr<ResourceLump>& curLump = this->lumps[lumpIndex];
        if (!curLump->IsBound())
        {
            if (curLump->GetNumBlocks() >= requiredNumBlocks)
            {
                bestLumpIndex = lumpIndex;
                break;
            }
        }
    }
    if (bestLumpIndex != InvalidIndex)
    {
        // see if we can find a better lump
        for (; lumpIndex < numLumps; lumpIndex++)
        {
            if (!this->lumps[lumpIndex]->IsBound())
            {
                SizeT curNumBlocks = this->lumps[lumpIndex]->GetNumBlocks();
                SizeT bestNumBlocks = this->lumps[bestLumpIndex]->GetNumBlocks();
                if ((curNumBlocks >= requiredNumBlocks) && (curNumBlocks < bestNumBlocks))
                {
                    bestLumpIndex = lumpIndex;
                }
            }
        }
    }
    return bestLumpIndex;
}

//------------------------------------------------------------------------------
/**
    This method tries to allocate a lump from the resource arena and
    bind it to a resource. If the resource arena is too fragmented to
    satisfy the request it will be compacted and the method will retry. If
    this fails again, the method will return an invalid ptr.
*/
Ptr<ResourceLump>
ResourceAllocator::AllocateLump(const Ptr<Resource>& res, SizeT numBlocks)
{
    n_assert(this->IsValid());

    // find the best free lump which is big enough
    IndexT bestFreeLump = this->FindBestFreeLumpIndex(numBlocks);
    if (InvalidIndex == bestFreeLump)
    {
        // hmm, the arena may be full, or fragmented, so let's defragment
        // the arena and try again...
        this->Compact();
        bestFreeLump = this->FindBestFreeLumpIndex(numBlocks);
        if (InvalidIndex == bestFreeLump)
        {
            // nope, we definitely can't satisfy the request
            return Ptr<ResourceLump>();
        }
    }

    // this is definitely the lump we will settle on, but we will very likely
    // have to split the lump since it is to big
    Ptr<ResourceLump> lump = this->lumps[bestFreeLump];
    n_assert(!lump->IsBound());

    // if the best lump doesn't exactly satisfy the request, split it into two lumps 
    IndexT lumpFirstBlockIndex = lump->GetFirstBlockIndex();
    SizeT lumpNumBlocks = lump->GetNumBlocks();
    n_assert(lumpNumBlocks >= numBlocks);
    if (lumpNumBlocks > numBlocks)
    {
        // resize the existing lump to exactly the right size,
        // and create a new free lump from the remaining blocks
        lump->Resize(numBlocks);
        SizeT diffNumBlocks = lumpNumBlocks - numBlocks;
        
        Ptr<ResourceLump> newLump = this->CreateLump(lumpFirstBlockIndex + numBlocks, diffNumBlocks);
        this->lumps.Append(newLump);
    }
    
    // lump points to the right blocks and has exactly the right size
    lump->Bind(res);
    return lump;
}

//------------------------------------------------------------------------------
/**
    Compact the resource arena to eliminate defragmentation. This will
    remove any existing free lumps and move the bound lumps around to
    fill the gaps. At the end of the arena, a single free lump will be
    created which spans all the available space in the resource arena.
    The method will return the number of continuous free blocks after
    the compact operation.
    NOTE: the method will not move locked lumps around, this may
    result in non-optimal compacting.
*/
SizeT
ResourceAllocator::Compact()
{
    n_assert(this->IsValid());

    // create a sorted-by-first-block array of all free lumps
    SizeT numLumps = this->lumps.Size();
    Array<KeyValuePair<IndexT,IndexT> > sortedLumpIndices;
    sortedLumpIndices.Reserve(numLumps);
    IndexT i;
    for (i = 0; i < numLumps; i++)
    {
        sortedLumpIndices.Append(KeyValuePair<IndexT,IndexT>(this->lumps[i]->GetFirstBlockIndex(), i));
    }
    sortedLumpIndices.Sort();

    // we need to keep an array of the free areas that should be created
    // after compacting
    Array<KeyValuePair<IndexT,SizeT> > createFreeLumps;

    // now go through the lumps in sorted order and remove the gaps
    IndexT firstFreeBlockIndex = InvalidIndex;
    SizeT numFreeBlocks = 0;
    for (i = 0; i < sortedLumpIndices.Size(); i++)
    {
        IndexT lumpIndex = sortedLumpIndices[i].Value();
        if (!this->lumps[lumpIndex]->IsBound())
        {
            // we found a free lump, simply increment the number of blocks
            // to move and leave the lump alone for now (free lumps will
            // be removed in a second pass after compaction)
            numFreeBlocks += this->lumps[lumpIndex]->GetNumBlocks();
        }
        else
        {
            // a bound lump will be moved backward to fill the free space,
            // but only if it isn't locked, if we encounter a locked
            // lump, the move block counter must be reset to zero
            if (this->lumps[lumpIndex]->IsLocked())
            {
                // record the current progress in free lumps array
                if ((InvalidIndex != firstFreeBlockIndex) && (numFreeBlocks > 0))
                {
                    createFreeLumps.Append(KeyValuePair<IndexT,SizeT>(firstFreeBlockIndex, numFreeBlocks));
                }
                numFreeBlocks = 0;
            }
            else if (numFreeBlocks > 0)
            {
                IndexT fromBlockIndex = this->lumps[lumpIndex]->GetFirstBlockIndex();
                n_assert(fromBlockIndex >= numFreeBlocks);
                IndexT toBlockIndex = fromBlockIndex - numFreeBlocks;
                SizeT numBlocks = this->lumps[lumpIndex]->GetNumBlocks();
                this->MoveArenaBlocks(fromBlockIndex, toBlockIndex, numBlocks);
                this->lumps[lumpIndex]->OnMoved(toBlockIndex);
                firstFreeBlockIndex = toBlockIndex + numBlocks;
            }
        }
    }
    // add the final new free lump at the end of the create-array
    if ((InvalidIndex != firstFreeBlockIndex) && (numFreeBlocks > 0))
    {
        createFreeLumps.Append(KeyValuePair<IndexT,SizeT>(firstFreeBlockIndex, numFreeBlocks));
    }

    // now delete all previous free lump objects (they no longer contain valid values)
    for (i = this->lumps.Size() - 1; i != InvalidIndex; i--)
    {
        if (!this->lumps[i]->IsBound())
        {
            this->lumps[i]->Discard();
            this->lumps.EraseIndexSwap(i);
        }
    }

    // finally create a new set of free lumps, which point to the new free
    // areas, if there were no locked lumps, then this should be a single
    // lump at the end of the arena, otherwise there may be a free lump
    // right in front of each locked lump
    // while we're at it, find the biggest free lump
    SizeT biggestFreeAreaBlocks = 0;
    for (i = 0; i < createFreeLumps.Size(); i++)
    {
        IndexT firstBlockIndex = createFreeLumps[i].Key();
        SizeT numBlocks = createFreeLumps[i].Value();
        Ptr<ResourceLump> newFreeLump = this->CreateLump(firstBlockIndex, numBlocks);
        this->lumps.Append(newFreeLump);

        // keep track of biggest free area
        if (numBlocks > biggestFreeAreaBlocks)
        {
            biggestFreeAreaBlocks = numBlocks;
        }        
    }
    return biggestFreeAreaBlocks;
}

//------------------------------------------------------------------------------
/**
    Free a resource lump, this will just unbind the lump from its resource
    which marks it as "free".
*/
void
ResourceAllocator::FreeLump(const Ptr<ResourceLump>& lump)
{
    n_assert(lump.isvalid() && lump->IsBound());
    lump->Unbind();
}

//------------------------------------------------------------------------------
/**
    Create and setup a new resource lump object. Override this method
    in a subclass to create a specific subclass of ResourceLump.
*/
Ptr<ResourceLump>
ResourceAllocator::CreateLump(IndexT firstBlock, SizeT numBlocks) const
{
    n_assert((firstBlock + numBlocks) <= this->arenaNumBlocks);
    Ptr<ResourceLump> lump = ResourceLump::Create();
    lump->Setup(this->arenaStartAddress, firstBlock, numBlocks, this->arenaBlockSize);
    return lump;
}

//------------------------------------------------------------------------------
/**
    Allocate the resource arena. Subclasses may have to override this
    method to perform allocation from special resource memory.
*/
void*
ResourceAllocator::AllocateArena(SizeT numBytes)
{
    return Memory::Alloc(Memory::ResourceHeap, numBytes);
}

//------------------------------------------------------------------------------
/**
    Free the resource arena, subclasses may have to override this
    method.
*/
void
ResourceAllocator::FreeArena(void* ptr)
{
    Memory::Free(Memory::ResourceHeap, ptr);
}

//------------------------------------------------------------------------------
/**
    Memory-copy wrapper for resource arena memory. Overwrite in subclass 
    if needed. The pointers are guaranteed to be block-aligned, and the
    size is guaranteed to be a multiple of the block size. The 2 memory
    regions are guaranteed to not overlap.
*/
void
ResourceAllocator::CopyArenaMemory(const void* from, void* to, SizeT numBytes)
{
    Memory::Copy(from, to, numBytes);
}

//------------------------------------------------------------------------------
/**
    Move a region of memory within the resource arena. Note that the memory
    may overlap! Subclasses must override this method if they need a
    special memory copy function.
*/
void
ResourceAllocator::MoveArenaBlocks(IndexT fromBlockIndex, IndexT toBlockIndex, SizeT numBlocksToMove)
{
    n_assert(this->IsValid());
    n_assert(fromBlockIndex != toBlockIndex);
    n_assert((fromBlockIndex + numBlocksToMove) < this->arenaNumBlocks);
    n_assert((toBlockIndex + numBlocksToMove) < this->arenaNumBlocks);

    const uchar* src = this->arenaStartAddress + fromBlockIndex * this->arenaBlockSize;
    uchar* dst = this->arenaStartAddress + toBlockIndex * this->arenaBlockSize;    
    SizeT size = this->arenaBlockSize * numBlocksToMove;

    if (fromBlockIndex != toBlockIndex)
    {
        if (Memory::IsOverlapping(src, size, dst, size))
        {
            // memory is overlapping, perform a block-wise copy
            IndexT i;
            for (i = 0; i < numBlocksToMove; i++)
            {
                this->CopyArenaMemory(src + i * this->arenaBlockSize, dst + i * this->arenaBlockSize, this->arenaBlockSize);
            }
        }
        else
        {
            // memory not overlapping, copy in one go
            this->CopyArenaMemory(src, dst, size);
        }
    }
}

} // namespace Resources
