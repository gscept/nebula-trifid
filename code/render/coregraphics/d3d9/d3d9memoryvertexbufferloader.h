#pragma once
//------------------------------------------------------------------------------
/**
    @class Win360::D3D9MemoryVertexBufferLoader
    
    Initialize a D3D9VertexBuffer from data in memory on the Win32/Xbox360
    platform. This resource loader only creates static VertexBuffers which are 
    initialized once and are not accessible by the CPU.
    
    (C) 2007 Radon Labs GmbH
*/
#include "coregraphics/base/memoryvertexbufferloaderbase.h"

//------------------------------------------------------------------------------
namespace Direct3D9
{
class D3D9MemoryVertexBufferLoader : public Base::MemoryVertexBufferLoaderBase
{
    __DeclareClass(D3D9MemoryVertexBufferLoader);
public:
    /// called by resource when a load is requested
    virtual bool OnLoadRequested();
};

} // namespace Win360
//------------------------------------------------------------------------------
