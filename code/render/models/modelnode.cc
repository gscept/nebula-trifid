//------------------------------------------------------------------------------
//  modelnode.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "models/modelnode.h"
#include "models/model.h"
#include "models/modelinstance.h"
#include "models/modelnodeinstance.h"

namespace Models
{
__ImplementClass(Models::ModelNode, 'MDND', Core::RefCounted);

using namespace Math;
using namespace Resources;
using namespace IO;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ModelNode::ModelNode() :
    inLoadResources(false),
    resourceStreamingLevelOfDetail(0.0f)
{
    this->type = ModelNodeType::FromName(ModelNodeType::Name("Solid"));
}

//------------------------------------------------------------------------------
/**
*/
ModelNode::~ModelNode()
{
    n_assert(!this->IsAttachedToModel());
    n_assert(!this->inLoadResources);
}

//------------------------------------------------------------------------------
/**
*/
bool
ModelNode::IsAttachedToModel() const
{
    return this->model.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Model>&
ModelNode::GetModel() const
{
    return this->model;
}

//------------------------------------------------------------------------------
/**
*/
Ptr<ModelNodeInstance>
ModelNode::CreateNodeInstance() const
{
    n_error("ModelNode::CreateNodeInstance() called!");
    return Ptr<ModelNodeInstance>();
}

//------------------------------------------------------------------------------
/**
    Create the node instance hierarchy.
*/
Ptr<ModelNodeInstance>
ModelNode::CreateNodeInstanceHierarchy(const Ptr<ModelInstance>& modelInst)
{
    return this->RecurseCreateNodeInstanceHierarchy(modelInst, 0);
}

//------------------------------------------------------------------------------
/**
    Recursively create node instances and attach them to the provided
    model instance. Returns a pointer to the root node instance.
*/
Ptr<ModelNodeInstance>
ModelNode::RecurseCreateNodeInstanceHierarchy(const Ptr<ModelInstance>& modelInst, const Ptr<ModelNodeInstance>& parentNodeInst)
{
    // create a ModelNodeInstance of myself
    Ptr<ModelNodeInstance> myNodeInst = this->CreateNodeInstance();
    myNodeInst->Setup(modelInst, this, parentNodeInst);

    // recurse into children
    IndexT i;
    for (i = 0; i < this->children.Size(); i++)
    {
        this->children[i]->RecurseCreateNodeInstanceHierarchy(modelInst, myNodeInst);
    }
    return myNodeInst;
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNode::OnAttachToModel(const Ptr<Model>& m)
{
    n_assert(!this->IsAttachedToModel());
    this->model = m;
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNode::OnRemoveFromModel()
{
    n_assert(this->IsAttachedToModel());
    if (this->inLoadResources)
    {
        this->UnloadResources();
    }
    this->model = 0;
    this->parent = 0;
    this->children.Clear();
    this->childIndexMap.Clear();
    this->visibleModelNodeInstances.Reset();
}

//------------------------------------------------------------------------------
/**
    This method is called when required resources should be loaded.
*/
void
ModelNode::LoadResources(bool sync)
{
    n_assert(!this->inLoadResources);
    this->inLoadResources = true;
}

//------------------------------------------------------------------------------
/**
    This method is called when required resources should be unloaded.
*/
void
ModelNode::UnloadResources()
{
    n_assert(this->inLoadResources);
    this->inLoadResources = false;
}


//------------------------------------------------------------------------------
/**
	This method is called whenever a model node resource needs reloading
*/
void
ModelNode::ReloadResources()
{
	n_assert(this->inLoadResources);
	this->inLoadResources = false;
}

//------------------------------------------------------------------------------
/**
    Returns the overall resource state (Initial, Loaded, Pending,
    Failed, Cancelled). Higher states override lower state (if some resources
    are already Loaded, and some are Pending, then Pending will be returned,
    if some resources failed to load, then Failed will be returned, etc...).
    Subclasses which load resource must override this method, and modify
    the return value of the parent class as needed).
*/
Resource::State
ModelNode::GetResourceState() const
{
    return Resource::Initial;
}

//------------------------------------------------------------------------------
/**
    This method is called once by Model::OnResourcesLoaded() when all
    pending resources of a model have been loaded.
*/
void
ModelNode::OnResourcesLoaded()
{
    // empty, override in subclass!
}

//------------------------------------------------------------------------------
/**
    This method is called once before rendering the ModelNode's visible 
    instance nodes through the ModelNodeInstance::ApplyState() and 
    ModelNodeInstance::Render() methods. The method must apply the 
    shader state that is shared across all instances. Since this state is
    constant across all instance nodes, this only happens once before
    rendering an instance set.
*/
void
ModelNode::ApplySharedState(IndexT frameIndex)
{
    // n_printf("ModelNode::ApplySharedState() called on '%s'!\n", this->GetName().Value());
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNode::AddVisibleNodeInstance(IndexT frameIndex, const Ptr<ModelNodeInstance>& nodeInst)
{
    this->visibleModelNodeInstances.Add(frameIndex, this->type, nodeInst);
    if (!this->visibleModelNodeInstances.IsResolved(this->type))
    {
        this->visibleModelNodeInstances.SetResolved(this->type, true);
        this->model->AddVisibleModelNode(frameIndex, this->type, this);
    }
}

//------------------------------------------------------------------------------
/**
    Begin parsing data tags. This method is called by StreamModelLoader
    before ParseDataTag() is called for the first time.
*/
void
ModelNode::BeginParseDataTags()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Parse a single data tag. If a subclass doesn't accept the data tag,
    the parent class method must be called!
*/
bool
ModelNode::ParseDataTag(const FourCC& fourCC, const Ptr<BinaryReader>& reader)
{
    if (FourCC('LBOX') == fourCC)
    {
        // bounding box
        point center = reader->ReadFloat4();
        vector extents = reader->ReadFloat4();
        this->SetBoundingBox(bbox(center, extents));
    }
    else if (FourCC('MNTP') == fourCC)
    {
        // model node type
        this->SetType(ModelNodeType::FromName(reader->ReadString()));
    }
    
    else if (FourCC('SSTA') == fourCC)
    {
        // string attribute
        StringAtom key = reader->ReadString();
        String value = reader->ReadString();
        this->SetStringAttr(key, value);
    }
    else
    {
        // throw error on unknown tag (we can't skip unknown tags)
        n_error("ModelNode::ParseDataTag: unknown data tag '%s' in '%s'!", 
            fourCC.AsString().AsCharPtr(),
            reader->GetStream()->GetURI().AsString().AsCharPtr());
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
/**
    End parsing data tags. This method is called by StreamModelLoader
    after the last ParseDataTag() is called.
*/
void
ModelNode::EndParseDataTags()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNode::AddChild(const Ptr<ModelNode>& c)
{
    n_assert(!this->childIndexMap.Contains(c->GetName()));
    this->children.Append(c);
    this->childIndexMap.Add(c->GetName(), this->children.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
bool
ModelNode::HasChild(const StringAtom& name) const
{
    return this->childIndexMap.Contains(name);
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<ModelNode>&
ModelNode::LookupChild(const StringAtom& name) const
{
    return this->children[this->childIndexMap[name]];
}

//------------------------------------------------------------------------------
/**
	Finds child recursively
*/
const Ptr<ModelNode> 
ModelNode::FindChild( const Util::StringAtom& name ) const
{
	if (this->childIndexMap.Contains(name))
	{
		return this->children[this->childIndexMap[name]];
	}
	else
	{
		IndexT i;
		for (i = 0; i < this->children.Size(); i++)
		{
			Ptr<ModelNode> node = this->children[i]->FindChild(name);
			if (node.isvalid())
			{
				return node;
			}
		}
	}

	// fall-through
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNode::RemoveChild(const Ptr<ModelNode> & c)
{
	n_assert2(this->childIndexMap.Contains(c->GetName()),"Trying to remove non existing child node");
	IndexT idx = this->childIndexMap[c->GetName()];
	this->children.EraseIndex(idx);
	this->childIndexMap.Erase(c->GetName());

	// reorder index map
	Util::Array<Ptr<ModelNode> > temp = this->children;
	
	this->childIndexMap.Clear();
	this->children.Clear();

	for(int i = 0; i< temp.Size();i++)
	{
		this->children.Append(temp[i]);
		this->childIndexMap.Add(temp[i]->GetName(), this->children.Size() - 1);
	}

}

} // namespace Models