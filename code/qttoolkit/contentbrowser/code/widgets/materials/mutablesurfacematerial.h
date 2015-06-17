#pragma once
//------------------------------------------------------------------------------
/**
	@class Materials::MutableSurfaceMaterial
	
	Inherits the SurfaceMaterial class to expose editor-specific functionalities.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "materials/surfacematerial.h"
namespace Materials
{
class MutableSurfaceMaterial : public SurfaceMaterial
{
	__DeclareClass(MutableSurfaceMaterial);
public:
	/// constructor
	MutableSurfaceMaterial();
	/// destructor
	virtual ~MutableSurfaceMaterial();

	/// set material template
	void SetMaterialTemplate(const Ptr<Materials::Material>& mat);
};
} // namespace Materials