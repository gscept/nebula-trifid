#pragma once
//------------------------------------------------------------------------------
/**
    @class Characters::CharacterNode
  
    The CharacterNode class wraps a Character object into a ModelNode
    for rendering.
    
    (C) 2008 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/  
#include "models/nodes/transformnode.h"
#include "characters/character.h"
#include "models/model.h"
#include "models/modelnodeinstance.h"
#include "coreanimation/animresource.h"
#include "coreanimation/managedanimresource.h"

//------------------------------------------------------------------------------
namespace Characters
{
class CharacterNode : public Models::TransformNode
{
    __DeclareClass(CharacterNode);
public:
    /// constructor
    CharacterNode();
    /// destructor
    virtual ~CharacterNode();

    /// called when attached to model node
    virtual void OnAttachToModel(const Ptr<Models::Model>& model);
    /// called when removed from model node
    virtual void OnRemoveFromModel();
    /// called when resources should be loaded
    virtual void LoadResources(bool sync);
    /// called when resources should be unloaded
    virtual void UnloadResources();
    /// get overall state of contained resources (Initial, Loaded, Pending, Failed, Cancelled)
    virtual Resources::Resource::State GetResourceState() const;
    /// called once when all pending resource have been loaded
    virtual void OnResourcesLoaded();
    /// parse data tag (called by loader code)
    virtual bool ParseDataTag(const Util::FourCC& fourCC, const Ptr<IO::BinaryReader>& reader);

    /// create a model node instance
    virtual Ptr<Models::ModelNodeInstance> CreateNodeInstance() const;
    /// get the owned character object
    const Ptr<Character>& GetCharacter() const;
    /// get the character's managed animation resource
    const Ptr<CoreAnimation::ManagedAnimResource>& GetManagedAnimResource() const;

    /// set the character's animation resource
    void SetAnimationResourceId(const Resources::ResourceId& resId);
    /// get the character's animation resource
    const Resources::ResourceId& GetAnimationResourceId() const;

private:
    /// recursively create model node instance and child model node instances
	/// deprecated? If we don't generate our 
    //virtual Ptr<Models::ModelNodeInstance> RecurseCreateNodeInstanceHierarchy(const Ptr<Models::ModelInstance>& modelInst, const Ptr<Models::ModelNodeInstance>& parentNodeInst=0);

    Resources::ResourceId animResId;
    Resources::ResourceId variationResId;
    Ptr<Character> character;
    Ptr<CoreAnimation::ManagedAnimResource> managedAnimResource;
    Ptr<CoreAnimation::ManagedAnimResource> managedVariationResource;
};

//------------------------------------------------------------------------------
/**
*/
inline void
CharacterNode::SetAnimationResourceId(const Resources::ResourceId& resId)
{
    this->animResId = resId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
CharacterNode::GetAnimationResourceId() const
{
    return this->animResId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Character>&
CharacterNode::GetCharacter() const
{
    return this->character;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreAnimation::ManagedAnimResource>& 
CharacterNode::GetManagedAnimResource() const
{
    return this->managedAnimResource;
}

} // namespace Characters
//------------------------------------------------------------------------------
  