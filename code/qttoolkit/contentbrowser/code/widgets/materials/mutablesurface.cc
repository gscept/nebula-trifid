//------------------------------------------------------------------------------
//  mutablesurfacematerial.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "mutablesurface.h"
#include "mutablesurfaceconstant.h"
#include "mutablesurfaceinstance.h"

namespace Materials
{
__ImplementClass(Materials::MutableSurface, 'MUSM', Materials::Surface);

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
Ptr<MutableSurfaceInstance>
MutableSurface::CreateInstance()
{
	Ptr<MutableSurfaceInstance> newInst = MutableSurfaceInstance::Create();
	Ptr<MutableSurface> thisPtr(this);
	newInst->Setup(thisPtr.downcast<Surface>());
	this->instances.Append(newInst.upcast<SurfaceInstance>());
	return newInst;
}

//------------------------------------------------------------------------------
/**
*/
void
MutableSurface::SetMaterialTemplate(const Ptr<Materials::Material>& mat)
{
    // first deregister
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