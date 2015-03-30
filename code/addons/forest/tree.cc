//------------------------------------------------------------------------------
//  treeinstance.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "forest/tree.h"
#include "models/modelserver.h"
#include "resources/resource.h"
#include "models/nodes/transformnode.h"

namespace Forest
{
__ImplementClass(Forest::Tree, 'TREE', Core::RefCounted);

using namespace Util;
using namespace Models;
using namespace Resources;

const float Tree::MaxVisibleDistance = 2000.0f;

//------------------------------------------------------------------------------
/**
*/
Tree::Tree() :
    isValid(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Tree::~Tree()
{
    n_assert(!this->IsValid());
}

//------------------------------------------------------------------------------
/**
*/
void
Tree::Setup(const StringAtom& resId, const StringAtom& collResId)
{
    // setup the graphics resource
    this->resourceId = resId;
    this->collideResId = collResId;

    // load managed model
    if (!this->managedModel.isvalid())
    {
        this->managedModel = ModelServer::Instance()->LoadManagedModel(this->resourceId);
    }
    this->ValidateModelInstance();
}

//------------------------------------------------------------------------------
/**
*/
void
Tree::Discard()
{
    n_assert(this->IsValid());

    // discard our LODs
    IndexT i;
    for (i = 0; i < this->lods.Size(); i++)
    {
        this->lods[i]->Discard();
    }
    this->lods.Clear();

    // discard our graphics resource
    this->managedModel = 0;

    //this->collideMesh = 0;
    this->resourceId.Clear();
    this->isValid = false;
}

//------------------------------------------------------------------------------
/**
    This creates and initializes our ModelInstance if needed. Since Model 
    loading happens asynchronously this may happen at any time after
    the ModelEntity is activated.
*/
void
Tree::ValidateModelInstance()
{
    n_assert(!this->IsValid());
    if (this->managedModel->GetState() == Resource::Loaded)
    {
        this->isValid = true;

        // setup LODs
        this->SetupLevelsOfDetail();

        // setup the collide mesh
        if (this->collideResId.IsValid())
        {
            this->SetupCollideMesh();
        }

        // make sure the billboard LOD is not too complex
        if (this->GetBillboardLOD()->GetNumVertices() > 64)
        {
            n_error("Tree Renderer: last LOD of tree '%s' is too complex! (> 64 vertices)", this->resourceId.Value());
        }
    }
    else if (this->managedModel->GetState() == Resource::Failed)
    {
        n_error("Failed to load model entity '%s'!", this->resourceId.Value());
    }
}


//------------------------------------------------------------------------------
/**
*/
Ptr<TreeInstance>
Tree::CreateInstance(const StringAtom& id, const Math::matrix44& worldTransform)
{
    n_assert(this->IsValid());

    Ptr<TreeInstance> treeInstance = TreeInstance::Create();
    treeInstance->Setup(this, id, worldTransform);
    return treeInstance;
}

//------------------------------------------------------------------------------
/**
*/
const Math::bbox&
Tree::GetBoundingBox() const
{
    n_assert(this->IsValid());
    return this->managedModel->GetModel()->GetBoundingBox();
}

//------------------------------------------------------------------------------
/**
*/
void
Tree::SetupLevelsOfDetail()
{
    // FIXME its a little bit dirty, create a better structure
    // structure of N3 Tree Model hierarchy
    // root node
    //      lodGroup1
    //          lod node level 0
    //          lod node level 1
    //          lod node level 2
    const Ptr<ModelNode> root = this->managedModel->GetModel()->GetRootNode();
	const Ptr<ModelNode>& stem = root->LookupChild("stem");
    n_assert(root.isvalid());
    n_assert(root->IsA(TransformNode::RTTI));
	n_assert(stem->IsA(ShapeNode::RTTI));

    // combine node path for lod objects
    String nodePath;
    nodePath.Format("%s/%s/", root->GetName().Value(), root->GetName().Value());

    // all child nodes of a LOD Group are lod root nodes
    Array<Ptr<ModelNode> > childNodes = root->GetChildren();
	childNodes.EraseIndex(childNodes.FindIndex(stem));
    
    /// FIXME! HACK!! divide min max distnace into x parts til Floh fixed the converter
    uint lodLevels = childNodes.Size();
    float stepSize = (childNodes[0].downcast<TransformNode>()->GetMaxDistance() - childNodes[0].downcast<TransformNode>()->GetMinDistance()) / lodLevels;

    // setup TreeLOD objects
    IndexT i;
    for (i = 0; i < childNodes.Size(); i++)
    {
        n_assert(childNodes[i].isvalid());
        n_assert(childNodes[i]->IsA(TransformNode::RTTI));

        const Ptr<TransformNode>& curNode = childNodes[i].downcast<TransformNode>();
        n_assert(curNode->LodDistancesUsed());

        // note: the last node is assumed to be rendered as billboard
        bool isBillboard = true;
        if (i < (childNodes.Size() - 1))
        {
            isBillboard = false;
        }

        /// FIXME! HACK!! divide min max distnace into x parts til Floh fixed the converter
        float min = i * stepSize;
        float max = min + stepSize;

        String childNodePath = nodePath;
        childNodePath.Append(childNodes[i]->GetName().AsString());

        Ptr<TreeLOD> newTreeLod = TreeLOD::Create();
        newTreeLod->Setup(min, max, childNodes[i], StringAtom(childNodePath), isBillboard, i);
        this->lods.Append(newTreeLod);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Tree::SetupCollideMesh()
{
    /*
    n_assert(this->collideResId.IsValid() && this->collideResId.IsValid());
    nString filename;
    filename.Format("meshes:%s_c_0.nvx2", this->collideResId.Value());
    this->collideMesh = Physics::MeshCache::Instance()->NewMesh(filename);
    n_assert(this->collideMesh.isvalid());
    */
}

} // namespace Forest
