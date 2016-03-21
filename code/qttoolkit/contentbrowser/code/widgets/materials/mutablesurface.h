#pragma once
//------------------------------------------------------------------------------
/**
	@class Materials::MutableSurface
	
	Inherits the SurfaceMaterial class to expose editor-specific functionalities.
	
	(C) 2015-2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "materials/surface.h"
namespace Materials
{
class MutableSurfaceInstance;
class MutableSurface : public Surface
{
	__DeclareClass(MutableSurface);
public:
	/// constructor
	MutableSurface();
	/// destructor
	virtual ~MutableSurface();

	/// create an instance of a surface
	Ptr<MutableSurfaceInstance> CreateInstance();

	/// set material template
	void SetMaterialTemplate(const Ptr<Materials::Material>& mat);

    /// set material variable on all instances
    void SetValue(const Util::StringAtom& name, const Util::Variant& value);
    /// get value from surface static values table
    const Util::Variant& GetValue(const Util::StringAtom& name);
};
} // namespace Materials