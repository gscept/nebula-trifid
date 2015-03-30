//------------------------------------------------------------------------------
//  grasslayer.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vegetation/grasslayer.h"
#include "vegetation/grassrenderer.h"

namespace Vegetation
{
__ImplementClass(Vegetation::GrassLayer, 'VGRL', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
GrassLayer::GrassLayer()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
GrassLayer::Setup(const GrassLayerInfo& inf)
{
    n_assert(!this->library.isvalid());
    GrassRenderer* renderer = GrassRenderer::Instance();

    this->layerInfo = inf;

    // find or setup the grass library associated with this layer
    if (renderer->Libraries().Contains(inf.GetGrassLibraryName()))
    {
        // use existing library
        this->library = renderer->Libraries()[inf.GetGrassLibraryName()];
    }
    else
    {
        // create new library and add to shared pool
        this->library = GrassLibrary::Create();
        this->library->Setup(inf);
        renderer->Libraries().Add(inf.GetGrassLibraryName(), this->library);
    }
}

//------------------------------------------------------------------------------
/**
*/
const GrassLayerInfo&
GrassLayer::GetLayerInfo() const
{
    return this->layerInfo;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<GrassLibrary>
GrassLayer::GetGrassLibrary() const
{
    n_assert(this->library.isvalid());
    return this->library;
}

} // namespace Vegetation
