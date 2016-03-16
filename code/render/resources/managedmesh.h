#pragma once
//------------------------------------------------------------------------------
/**
    @class Resources::ManagedMesh
  
    Specialized managed resource for meshes.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "resources/managedresource.h"
#include "coregraphics/mesh.h"

//------------------------------------------------------------------------------
namespace Resources
{
class ManagedMesh : public ManagedResource
{
    __DeclareClass(ManagedMesh);
public:
    /// get contained mesh resource
    const Ptr<CoreGraphics::Mesh>& GetMesh() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Mesh>&
ManagedMesh::GetMesh() const
{
    return this->GetLoadedResource().downcast<CoreGraphics::Mesh>();
}

} // namespace Resources
//------------------------------------------------------------------------------
