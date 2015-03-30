//------------------------------------------------------------------------------
//  grasslibrary.cc
//  (C) 2008 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vegetation/grasslibrary.h"
#include "vegetation/grassrenderer.h"
#include "vegetation/grasspatch.h"
#include "vegetation/grassplantmesh.h"
#include "models/managedmodel.h"
#include "resources/managedmesh.h"
#include "models/nodes/transformnode.h"
#include "models/nodes/shapenode.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "models/modelserver.h"
#include "coregraphics/streammeshloader.h"
#include "models/streammodelloader.h"
#include "coregraphics/base/resourcebase.h"
#include "resources/resourcemanager.h"

namespace Vegetation
{
__ImplementClass(Vegetation::GrassLibrary, 'BGLI', Core::RefCounted);

using namespace Base;
using namespace CoreGraphics;
using namespace Resources;
using namespace Models;
using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
GrassLibrary::GrassLibrary() :
    numGrassPatches(0),
    valid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
GrassLibrary::~GrassLibrary()
{    
    // discard our managed model
    ModelServer::Instance()->DiscardManagedModel(this->managedModel);
    this->managedModel = 0;
    this->patches.Clear();
    this->plantMeshes.Clear();
}

//------------------------------------------------------------------------------
/**
*/
const Resources::ResourceId&
GrassLibrary::GetResourceId() const
{
    return this->managedModel->GetResourceId();
}

//------------------------------------------------------------------------------
/**
*/
void
GrassLibrary::Setup(const GrassLayerInfo& inf)
{
    n_assert(!this->managedModel.isvalid());
    // extra mesh loader for shapenodes in model
    Ptr<StreamMeshLoader> meshLoader = StreamMeshLoader::Create();
    meshLoader->SetAccess(ResourceBase::AccessRead);
    meshLoader->SetUsage(ResourceBase::UsageCpu);
    Ptr<StreamModelLoader> modelLoader = StreamModelLoader::Create();
    modelLoader->SetStreamMeshLoader(meshLoader);
    // setup a graphics resource for the library
    ResourceId pathToModel(String("mdl:" + inf.GetGrassLibraryName() + ".n3"));
    this->managedModel = ResourceManager::Instance()->CreateManagedResource(Model::RTTI, pathToModel, modelLoader.cast<ResourceLoader>()).downcast<ManagedModel>();   
}

//------------------------------------------------------------------------------
/**
    This parses the Nebula2 hierarchy of the library graphics resource and
    creates GrassPlantMesh objects from the geometry.
*/
void
GrassLibrary::SetupPlantMeshes()
{
    const Ptr<Model>& model = this->managedModel->GetModel();
    n_assert(0 != model);
    const Array<Ptr<ModelNode>> nodes = model->GetNodes();

    // collect meshes from shapenodes
    if (nodes.Size() > 0)
    {
        IndexT nodeIdx;
        for (nodeIdx = 0; nodeIdx < nodes.Size(); nodeIdx++)
        {
            this->CollectMeshes(nodes[nodeIdx]);
        }
    }

    // throw an error if there are no plant meshes
    if (this->plantMeshes.Size() == 0)
    {
        n_error("GrassLibrary: no plant hierarchy nodes found in '%s'!", this->managedModel->GetResourceId().Value());
    }
}

//------------------------------------------------------------------------------
/**
    Collect recursivly all meshes from shapenodes.
*/
void
GrassLibrary::CollectMeshes(const Ptr<ModelNode>& node)
{
    if (node->IsA(ShapeNode::RTTI))
    {
        const Ptr<ShapeNode>& shapeNode = node.cast<ShapeNode>();
        n_assert(shapeNode->GetParent().isvalid() && shapeNode->GetParent()->IsA(TransformNode::RTTI));
        const Ptr<TransformNode>& parentTransformNode = shapeNode->GetParent().cast<TransformNode>();
        // get the shape node's local transform and reset the x/z position to 0
        matrix44 localTransform = matrix44::transformation(point::origin(), 
                                                           quaternion::identity(), 
                                                           parentTransformNode->GetScale(),
                                                           point::origin(), 
                                                           parentTransformNode->GetRotation(),
                                                           parentTransformNode->GetPosition());
                    
        // create a new plant mesh object
        int primitiveGroupIndex = shapeNode->GetPrimitiveGroupIndex();
        const Ptr<ManagedMesh>& mesh = shapeNode->GetManagedMesh();
        Ptr<GrassPlantMesh> newPlantMesh = GrassPlantMesh::Create();
        newPlantMesh->Setup(shapeNode, mesh, primitiveGroupIndex, localTransform);
        this->plantMeshes.Append(newPlantMesh);
    }
    // iterate thru children and call this function recursivly
    const Array<Ptr<ModelNode>>& children = node->GetChildren();
    if (children.Size() > 0)
    {
        IndexT childIdx;
        for (childIdx = 0; childIdx < children.Size(); childIdx++)
        {
            this->CollectMeshes(children[childIdx]);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Set grass patch visibility. This is mainly for debugging.
*/
void
GrassLibrary::SetGrassPatchVisibility(bool b)
{
    int i;
    for (i = 0; i < this->patches.Size(); i++)
    {
        this->patches[i]->SetVisible(b);
    }
}

//------------------------------------------------------------------------------
/**
    This goes through the grass patch array and returns the first available
    inactive grass patch. Returns 0 if no free grass patch exists at the
    moment.
*/
Ptr<GrassPatch>
GrassLibrary::GetFirstFreeGrassPatch()
{
    int i;
    for (i = 0; i < this->patches.Size(); i++)
    {
        if (!this->patches[i]->IsActive())
        {
            return this->patches[i];
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
bool 
GrassLibrary::IsModelLoaded() const
{
    bool loaded = (this->managedModel->GetState() == Resource::Loaded);
    if (loaded)
    {
        loaded = this->managedModel->GetModel()->CheckPendingResources();
    }    
    return loaded;
}

//------------------------------------------------------------------------------
/**
*/
void 
GrassLibrary::ValidateMesh()
{
    n_assert(!this->IsValid());
    if (this->IsModelLoaded())
    {
        // setup the plant meshes
        this->SetupPlantMeshes();

        Ptr<GrassRenderer> renderer = GrassRenderer::Instance();
        // setup the grass patches
        this->numGrassPatches = renderer->GetPatchArraySize() * renderer->GetPatchArraySize();
        int i;
        for (i = 0; i < this->numGrassPatches; i++)
        {
            Ptr<GrassPatch> newGrassPatch = GrassPatch::Create();
            newGrassPatch->Setup(this, i);
            this->patches.Append(newGrassPatch);
        }
        this->valid = true;
    }
}

//------------------------------------------------------------------------------
/**
*/
bool 
GrassLibrary::IsValid() const
{
    return this->valid;
}
} // namespace Vegetation
