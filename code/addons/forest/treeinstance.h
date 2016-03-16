#pragma once
//------------------------------------------------------------------------------
/**
    @class Forest::TreeInstance

    An actual tree instances. Tree instances are created from Tree objects
    and are grouped into TreeClusters for efficient culling and rendering.

    (C) 2008 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "math/bbox.h"
#include "math/matrix44.h"
#include "models/modelinstance.h"
#include "timing/time.h"

//------------------------------------------------------------------------------
namespace Forest
{
class Tree;

class TreeInstance : public Core::RefCounted
{
    __DeclareClass(TreeInstance);
public:
    /// constructor
    TreeInstance();
    /// destructor
    virtual ~TreeInstance();

    /// setup the tree instance
    void Setup(const Ptr<Tree>& tree, const Util::StringAtom& id, const Math::matrix44& tform);
    /// discard the tree instance
    void Discard();
    /// return true if the tree instance is valid
    bool IsValid() const;

    /// return pointer to Tree which created the instance
    const Ptr<Tree>& GetTree() const;
    /// get the tree's id
    const Util::StringAtom& GetId() const;
    /// get current transform matrix
    const Math::matrix44& GetTransform() const;
    /// get the inverse transform matrix
    const Math::matrix44& GetInvTransform() const;
    /// get the global bounding box of the tree instance
    const Math::bbox& GetGlobalBoundingBox() const;
    /// get data necessary for LOD rendering, lodIndex is InvalidIndex if no billboard LOD is visible!
    void GetBillboardData(IndexT& lodIndex, float& outFadeAlpha, float& outScale) const;
    /// get a pointer to the nebula3 model instance of the tree
    const Ptr<Models::ModelInstance>& GetModelInstance() const;

    /// start an LOD transition
    void StartTransition(IndexT lodIndex, bool doSmoothTransition);
    /// continue an LOD transition
    void ContinueTransition();
    /// return true if a transition is currently active
    bool IsTransitionActive() const;

    /// render the tree instance
    void OnRenderBefore(const Math::vector& viewerPos, IndexT frameIndex);

private:
    Ptr<Tree> tree;
    Util::Array<Ptr<Models::ModelInstance> > lodModelInstances;
    Ptr<Models::ModelInstance> lodModelInstance;
    Util::StringAtom id;
    Math::matrix44 transform;
    Math::matrix44 invTransform;
    Math::bbox globalBox;

    static const float LODTransitionDuration;

    Timing::Time transStartTime;    // start time of current transition
    IndexT curLodIndex;             // current LOD index, InvalidIndex if no current LOD is set
    IndexT dstLodIndex;             // next LOD index if during a LOD transition, otherwise InvalidIndex
    float curAlpha;                 // usually 1.0, except if a transition is running
    float dstAlpha;                 // usually 0.0, except if a transition is running
    uint lastVisibleFrameId;
    float billboardScale;
};

//------------------------------------------------------------------------------
/**
*/
inline bool
TreeInstance::IsTransitionActive() const
{
    return (InvalidIndex != this->dstLodIndex);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
TreeInstance::IsValid() const
{
    return this->tree.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
TreeInstance::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
TreeInstance::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
TreeInstance::GetInvTransform() const
{
    return this->invTransform;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::bbox&
TreeInstance::GetGlobalBoundingBox() const
{
    return this->globalBox;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Models::ModelInstance>&
TreeInstance::GetModelInstance() const
{
    return this->lodModelInstance;
}

} // namespace Forest
//------------------------------------------------------------------------------
