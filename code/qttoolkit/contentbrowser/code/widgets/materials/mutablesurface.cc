//------------------------------------------------------------------------------
//  mutablesurfacematerial.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mutablesurface.h"
#include "mutablesurfaceconstant.h"
#include "mutablesurfaceinstance.h"

namespace Materials
{
__ImplementClass(Materials::MutableSurface, 'MUSM', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
MutableSurface::MutableSurface()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
MutableSurface::~MutableSurface()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
MutableSurface::SetMaterialTemplate(const Ptr<Materials::Material>& mat)
{
    // first degregister
    this->materialTemplate->RemoveSurface(this);

	IndexT i;
    for (i = 0; i < this->instances.Size(); i++)
    {
        const Ptr<MutableSurfaceInstance>& instance = this->instances[i].downcast<MutableSurfaceInstance>();
        instance->Cleanup();
    }

    // set material template, and setup surface again
	this->materialTemplate = mat;
	this->Setup(mat);

    // setup instances again, now with the new surface
    for (i = 0; i < this->instances.Size(); i++)
    {
        const Ptr<MutableSurfaceInstance>& instance = this->instances[i].downcast<MutableSurfaceInstance>();
        Ptr<MutableSurface> thisPtr(this);
        instance->Setup(thisPtr.upcast<Surface>());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
MutableSurface::SetValue(const Util::StringAtom& name, const Util::Variant& value)
{
    // change the surface
    SurfaceValueBinding obj;
    obj.value = value;
    obj.system = false;             // should be true if we modify this material from the tools
    this->staticValues[name] = obj;

    IndexT i;
    for (i = 0; i < this->instances.Size(); i++)
    {
        this->instances[i]->SetValue(name, value);
    }
}

//------------------------------------------------------------------------------
/**
*/
const Util::Variant&
MutableSurface::GetValue(const Util::StringAtom& name)
{
    return this->staticValues[name].value;
}

} // namespace Materials