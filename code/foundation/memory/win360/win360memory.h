#pragma once
//------------------------------------------------------------------------------
/**
    @file memory/win360/win360memory.h
    
    Memory subsystem features which are identical on Win32 and Xbox360.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/config.h"
#include "core/debug.h"
#include "threading/interlocked.h"
#include "memory/win360/win360memoryconfig.h"
#if __WIN32__
#include "memory/win32/win32memory.h"
#elif __XBOX360__
#include "memory/xbox360/xbox360memory.h"
#endif

namespace Memory
{
extern long volatile TotalAllocCount;
extern long volatile TotalAllocSize;
extern long volatile HeapTypeAllocCount[NumHeapTypes];
extern long volatile HeapTypeAllocSize[NumHeapTypes];
extern unsigned int volatile MemoryLoggingThreshold;
extern HeapType volatile MemoryLoggingHeapType;

//------------------------------------------------------------------------------
/**
    Global memory functions.
*/
/// allocate a chunk of memory
extern void* Alloc(HeapType heapType, size_t size);
/// re-allocate a chunk of memory
extern void* Realloc(HeapType heapType, void* ptr, size_t size);
/// free a chunk of memory
extern void Free(HeapType heapType, void* ptr);
/// duplicate a C-string (obsolete)
extern char* DuplicateCString(const char* from);
/// check if 2 memory regions are overlapping
extern bool IsOverlapping(const unsigned char* srcPtr, size_t srcSize, const unsigned char* dstPtr, size_t dstSize);

//------------------------------------------------------------------------------
/**
    Get the system's total current memory, this does not only include
    Nebula3's memory allocations but the memory usage of the entire system.
*/
struct TotalMemoryStatus
{
    unsigned int totalPhysical;
    unsigned int availPhysical;
    unsigned int totalVirtual;
    unsigned int availVirtual;
};

extern TotalMemoryStatus GetTotalMemoryStatus();
extern void DumpTotalMemoryStatus();

//------------------------------------------------------------------------------
/**
    Debug function which validates the process heap. This will NOT check
    local heaps (call Heap::ValidateAllHeaps() for this). 
    Stops the program if something is wrong. 
*/
#if NEBULA3_MEMORY_ADVANCED_DEBUGGING
/// check memory lists for consistency
extern bool ValidateMemory();
/// dump current memory status to log file
extern void Checkpoint(const char* msg);
/// enable memory logging
void EnableMemoryLogging(unsigned int threshold, HeapType heapType = InvalidHeapType);
/// disable memory logging
void DisableMemoryLogging();
/// toggle memory logging
void ToggleMemoryLogging(unsigned int threshold, HeapType heapType = InvalidHeapType);
/// dump memory leaks
void DumpMemoryLeaks();
#endif

#if NEBULA3_MEMORY_ADVANCED_DEBUGGING
#define __MEMORY_CHECKPOINT(s) Memory::Checkpoint(##s)
#else
#define __MEMORY_CHECKPOINT(s)
#endif

// FIXME: Memory-Validation disabled for now
#define __MEMORY_VALIDATE(s)
} // namespace Memory

#ifdef new
#undef new
#endif

#ifdef delete
#undef delete
#endif

#define n_new(type) new type
#define n_new_array(type, size) new type[size]
#define n_delete(ptr) delete ptr
#define n_delete_array(ptr) delete[] ptr
//------------------------------------------------------------------------------

