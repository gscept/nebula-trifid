//------------------------------------------------------------------------------
//  meshcache.cc
//  (C) 2006 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/meshcache.h"

namespace Physics
{
__ImplementClass(Physics::MeshCache, 'MSHC', Core::RefCounted);
__ImplementSingleton(Physics::MeshCache);

//------------------------------------------------------------------------------
/**
*/
MeshCache::MeshCache() :
    isOpen(false)
{
    n_assert(0 == Singleton);
    Singleton = this;
}

//------------------------------------------------------------------------------
/**
*/
MeshCache::~MeshCache()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    n_assert(Singleton);
    Singleton = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
MeshCache::Open()
{
    if (this->IsOpen())
    {
        this->Close();
    }
    this->isOpen = true;
}

//------------------------------------------------------------------------------
/**
*/
void
MeshCache::Close()
{
    n_assert(this->IsOpen());
    this->meshes.Clear();
    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<PhysicsMesh>
MeshCache::NewMesh(const Util::String& filename)
{
    Ptr<PhysicsMesh> mesh;
    if (this->meshes.Contains(filename))
    {
        // return existing mesh
        mesh = this->meshes[filename];
    }
    else
    {
        // create new cached mesh
        mesh = PhysicsMesh::Create();
        mesh->SetFilename(filename);
        if (mesh->Load())
        {
            this->meshes.Add(filename, mesh);
        }
        else
        {
            n_error("Physics::MeshCache: failed to load mesh file '%s'!", filename.AsCharPtr());
        }
    }
    return mesh;
}

} // namespace MeshCache