#pragma once
//------------------------------------------------------------------------------
/**
@class Physics::PhysicsStreamMeshLoader
    
    Setup a physics mesh object from a stream. Supports the following file formats:
        
    - nvx3 (Nebula3 binary mesh file format)
    
    
    (C) 2012 Johannes Hirche
*/
//------------------------------------------------------------------------------
#include "resources/streamresourceloader.h"
#include "coregraphics/base/resourcebase.h"

namespace Physics
{
class PhysicsStreamMeshLoader : public Resources::StreamResourceLoader
{
    __DeclareClass(PhysicsStreamMeshLoader);
public:
    /// constructor
    PhysicsStreamMeshLoader();    

	virtual bool SetupResourceFromStream(const Ptr<IO::Stream>& stream);
private:
    
   
protected:

};

} // namespace Physics
//------------------------------------------------------------------------------
