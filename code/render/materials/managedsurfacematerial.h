#pragma once
//------------------------------------------------------------------------------
/**
	@class Materials::ManagedSurfaceMaterial
	
	Implements a managed resource container for a surface material resource.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "resources/managedresource.h"
#include "materials/surfacematerial.h"
namespace Materials
{
class ManagedSurfaceMaterial : public Resources::ManagedResource
{
	__DeclareClass(ManagedSurfaceMaterial);
public:
	/// get contained surface material, or the placeholder material if material isn't loaded
    const Ptr<SurfaceMaterial>& GetMaterial() const;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<SurfaceMaterial>&
Materials::ManagedSurfaceMaterial::GetMaterial() const
{
    return this->GetLoadedResource().downcast<SurfaceMaterial>();
}

} // namespace Materials