//------------------------------------------------------------------------------
//  mutablesurfacematerial.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mutablesurfacematerial.h"
#include "mutablesurfaceconstant.h"

namespace Materials
{
__ImplementClass(Materials::MutableSurfaceMaterial, 'MUSM', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
MutableSurfaceMaterial::MutableSurfaceMaterial()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MutableSurfaceMaterial::~MutableSurfaceMaterial()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MutableSurfaceMaterial::SetMaterialTemplate(const Ptr<Materials::Material>& mat)
{
	IndexT i;
	for (i = 0; i < this->constants.Size(); i++)
	{
		// cast to a type which befriends this class, so that we may discard it 
		Ptr<MutableSurfaceConstant> constant = this->constants[i].downcast<MutableSurfaceConstant>();
		constant->Discard();
	}
	this->constants.Clear();
	this->constantsByName.Clear();
	this->materialTemplate = mat;
	this->Setup(mat);
}

} // namespace Materials