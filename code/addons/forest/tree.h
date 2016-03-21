#pragma once
//------------------------------------------------------------------------------
/**
    @class Forest::Tree

    Represents the shared data for a tree.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "forest/treeinstance.h"
#include "forest/treelod.h"
#include "models/managedmodel.h"

//------------------------------------------------------------------------------
namespace Forest
{
class Tree : public Core::RefCounted
{
    __DeclareClass(Tree);
public:
    /// constructor
    Tree();
    /// destructor
    virtual ~Tree();

    /// setup the tree from a resource id
    void Setup(const Util::StringAtom& resId, const Util::StringAtom& collideResId);
    /// discard the tree
    void Discard();
    /// return true if Tree object is valid
    bool IsValid() const;
    /// validate the ModelInstance
    void ValidateModelInstance();    

    /// create a TreeInstance
    Ptr<TreeInstance> CreateInstance(const Util::StringAtom& id, const Math::matrix44& worldTransform);

    /// get the resource id of the tree
    const Util::StringAtom& GetResourceId() const;
    /// get pointer to graphics resource
    const Ptr<Models::Model>& GetModel() const;
    /// read access to LODs
    const Util::Array<Ptr<TreeLOD> >& GetLODs() const;
    /// get the overall local bounding box of the tree
    const Math::bbox& GetBoundingBox() const;
    /// get a pointer to the billboard LOD (always the last one)
    const Ptr<TreeLOD>& GetBillboardLOD() const;
    /// get a pointer to the optional collide mesh
    //const Ptr<Physics::Mesh>& GetCollideMesh() const;

private:
    // setup the LODs
    void SetupLevelsOfDetail();
    /// setup the optional collide mesh
    void SetupCollideMesh();

    static const float MaxVisibleDistance;
    Util::StringAtom resourceId;
    Util::StringAtom collideResId;
    Ptr<Models::ManagedModel> managedModel;
    Util::Array<Ptr<TreeLOD> > lods;
    //Ptr<Physics::Mesh> collideMesh;
    bool isValid;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
Tree::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
Tree::GetResourceId() const
{
    return this->resourceId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Models::Model>&
Tree::GetModel() const
{
    return this->managedModel->GetModel();
}
//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<TreeLOD>>&
Tree::GetLODs() const
{
    return this->lods;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<TreeLOD>&
Tree::GetBillboardLOD() const
{
    return this->lods.Back();
}

//------------------------------------------------------------------------------
/**
*/
/*
inline const Ptr<Physics::Mesh>&
Tree::GetCollideMesh() const
{
    return this->collideMesh;
}
*/
} // namespace Forest2
//------------------------------------------------------------------------------
