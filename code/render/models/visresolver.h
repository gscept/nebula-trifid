#pragma once
//------------------------------------------------------------------------------
/**
    @class Models::VisResolver
  
    The VisResolver accepts visible ModelInstances and resolves
    them into their ModelNodeInstances, organized into node type
    and sorted for optimal rendering (instances of the same ModelNode
    should be rendered together to reduce state switch overhead).
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "core/singleton.h"
#include "models/visresolvecontainer.h"

//------------------------------------------------------------------------------
namespace Models
{
class ModelInstance;
class Model;
class ModelNode;
class ModelNodeInstance;

class VisResolver : public Core::RefCounted
{
    __DeclareClass(VisResolver);
    __DeclareSingleton(VisResolver);
public:
    /// constructor
    VisResolver();
    /// destructor
    virtual ~VisResolver();

    /// open the visibility resolver
    void Open();
    /// close the visibility resolver
    void Close();
    /// return true if currently open
    bool IsOpen() const;

    /// begin resolving 
    void BeginResolve(const Math::matrix44& cameraTransform);
    /// attach a visible ModelInstance
	void AttachVisibleModelInstance(IndexT frameIndex, const Ptr<ModelInstance>& inst, bool updateLod);
    /// attach a visible ModelInstance
	void AttachVisibleModelInstancePlayerCamera(IndexT frameIndex, const Ptr<ModelInstance>& inst, bool updateLod);
    /// end resolve
    void EndResolve();

    /// post-resolve: get Models with visible ModelNodeInstances by node type
    const Util::Array<Ptr<Model>>& GetVisibleModels(const Materials::MaterialType::Code& materialType) const;
    /// post-resolve: get visible ModelNodes of a Model by node type
    const Util::Array<Ptr<ModelNode>>& GetVisibleModelNodes(const Materials::MaterialType::Code& materialType, const Ptr<Model>& model) const;
    /// post-resolve: get visible ModelNodeInstance of a ModelNode by node type
    const Util::Array<Ptr<ModelNodeInstance>>& GetVisibleModelNodeInstances(const Materials::MaterialType::Code& materialType, const Ptr<ModelNode>& modelNode) const;

private:
    friend class Model;

    /// add a visible model by node type
    void AddVisibleModel(IndexT frameIndex, const Materials::MaterialType::Code& materialType, const Ptr<Model>& model);

    VisResolveContainer<Model> visibleModels;
    Math::matrix44 cameraTransform;
    IndexT resolveCount;
    bool isOpen;
    bool inResolve;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
VisResolver::IsOpen() const
{
    return this->isOpen;
}

//------------------------------------------------------------------------------
/**
*/
inline void
VisResolver::AddVisibleModel(IndexT frameIndex, const Materials::MaterialType::Code& materialType, const Ptr<Model>& model)
{
    n_assert(this->inResolve);
    this->visibleModels.Add(frameIndex, materialType, model);
}

} // namespace Models
//------------------------------------------------------------------------------
