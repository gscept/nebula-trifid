#pragma once
//------------------------------------------------------------------------------
/**
    @class Win360::D3D9MemoryIndexBufferLoader
    
    Initialize a D3D9IndexBuffer from data in memory for the Win32/Xbox360
    platform. This resource loader only creates static IndexBuffers which are 
    initialized once and are not accessible by the CPU.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/memoryindexbufferloaderbase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9MemoryIndexBufferLoader : public Base::MemoryIndexBufferLoaderBase
{
    __DeclareClass(D3D9MemoryIndexBufferLoader);
public:
    /// called by resource when a load is requested
    virtual bool OnLoadRequested();
};

} // namespace Win360
//------------------------------------------------------------------------------
