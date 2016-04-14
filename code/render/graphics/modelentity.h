#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::ModelEntity
    
    Represents a visible graphics object.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "graphics/graphicsentity.h"
#include "resources/resourceid.h"
#include "models/managedmodel.h"
#include "models/modelinstance.h"
#include "characters/characterinstance.h"
#include "graphics/graphicsprotocol.h"
#include "characters/charjointinfo.h"
#include "lighting/environmentprobe.h"
#include "materials/surfaceconstantinstance.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class ModelEntity : public GraphicsEntity
{
    __DeclareClass(ModelEntity);
public:
    /// constructor
    ModelEntity();
    /// destructor
    virtual ~ModelEntity();
    
    /// set the model's resource id
    void SetResourceId(const Resources::ResourceId& resId);
    /// get the model's resource id
    const Resources::ResourceId& GetResourceId() const;
    /// set if the resource should load synced
    void SetLoadSynced(bool b);
    /// get if resources should load synced
    bool GetLoadSynced() const;
    /// optional path to root node (allows to instantiate a ModelEntity from part of a ModelNode hierarchy)
    void SetRootNodePath(const Util::StringAtom& rootNodePath);
    /// get root node path, return invalid string atom if root node
    const Util::StringAtom& GetRootNodePath() const;
    /// set optional root node offset matrix
    void SetRootNodeOffsetMatrix(const Math::matrix44& offsetMatrix);
    /// get optional root node offset matrix
    const Math::matrix44& GetRootNodeOffsetMatrix() const;
	/// sets whether or not the model should be rendered instanced
	void SetInstanced(bool b);
	/// gets whether or not the model is rendered instanced
	bool IsInstanced() const;
	/// sets the instance code used when performing instanced rendering
	void SetInstanceCode(const IndexT& i);
	/// get the instance code used when performing instanced rendering
	const IndexT& GetInstanceCode();
	/// set environment probe
	void SetEnvironmentProbe(const Ptr<Lighting::EnvironmentProbe>& probe);
	/// get environment probe
	const Ptr<Lighting::EnvironmentProbe>& GetEnvironmentProbe() const;

	/// sets picking id
	void SetPickingId(const IndexT& i);
	/// gets picking id
	const IndexT& GetPickingId() const;

    /// enable anim driven motion tracking
    void ConfigureAnimDrivenMotionTracking(bool enabled, const Util::StringAtom& jointName);
    /// enable anim event tracking
    void ConfigureAnimEventTracking(bool enabled, bool onlyDominatingClip);
    /// configure joint tracking
    void ConfigureCharJointTracking(bool enabled, const Util::Array<Util::StringAtom>& jointNames);

    /// get the state of the contained managed model resource
    Resources::Resource::State GetModelResourceState() const;
    /// get pointer to model instance (only valid if already loaded)
    const Ptr<Models::ModelInstance>& GetModelInstance() const;

    /// return true if this is a character
    bool HasCharacter() const;
    /// get pointer to character instance
    const Ptr<Characters::CharacterInstance>& GetCharacterInstance() const;
    /// get pointer to character
    const Ptr<Characters::Character>& GetCharacter() const;
	/// set bool to render skeleton
	void SetRenderSkeleton(bool b);
    /// validates character from model resource, sorta haxxy
    void ValidateCharacter();

    /// return true if anim driven motion tracking is enabled
    bool IsAnimDrivenMotionTrackingEnabled() const;
    /// get anim driven motion joint name
    const Util::StringAtom& GetAnimDrivenMotionJointName() const;
    /// get the current anim-driven-motion tracking vector
    const Math::vector& GetAnimDrivenMotionVector() const;

    /// return true if anim event tracking is enabled
    bool IsAnimEventTrackingEnabled() const;
    /// return the array of anim events of the current frame
    const Util::Array<Animation::AnimEventInfo>& GetAnimEvents() const;

    /// return true if joint tracking is enabled
    bool IsCharJointTrackingEnabled() const;
    /// dynamically add a tracked joint, note: you cannot remove tracked joints once added!
    void AddTrackedCharJoint(const Util::StringAtom& jointName);
    /// get tracked character joint
    const Characters::CharJointInfo* GetTrackedCharJointInfo(const Util::StringAtom& jointName) const;
    /// return true if current char joint data is valid in this frame (may change by visibility)
    bool IsCharJointDataValid() const;
    /// get char joint information
    const Util::Array<Characters::CharJointInfo>& GetCharJointInfos() const;

    /// modifies a material variable for a given model node
    void SetSurfaceConstant(const Util::String& nodeName, const Util::String& varName, const Util::Variant& value);
	/// sets material variable as texture
	void SetSurfaceConstant(const Util::String& nodeName, const Util::String& varName, const Ptr<CoreGraphics::Texture>& tex);
	/// sets material variable if it exists on node and direct children
	void SetMaterialVariableByName(const Util::String& varName, const Util::Variant& value);

    /// handle a message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

protected:
    friend class View;
    friend class Visibility::VisibilityChecker;

    /// called when entity is created
    virtual void OnActivate();
    /// called before entity is destroyed
    virtual void OnDeactivate();

    /// called per frame to
    virtual void OnReset();

    /// called from view
	virtual void OnResolveVisibility(IndexT frameIndex, bool updateLod = false);
    /// called when transform matrix changed
    virtual void OnTransformChanged();
    /// called before culling on each(!) graphics entity (visible or not!)
    virtual void OnCullBefore(Timing::Time time, Timing::Time globalTimeFactor, IndexT frameIndex);
    /// called when the entity has been found visible, may be called several times per frame!
    virtual void OnNotifyCullingVisible(const Ptr<GraphicsEntity>& observer, IndexT frameIndex);
    /// called right before rendering
    virtual void OnRenderBefore(IndexT frameIndex);
    /// called to render a debug visualization of the entity
    virtual void OnRenderDebug();
    /// validate the ModelInstance
    void ValidateModelInstance();    
    /// called when the entity becomes visible
    virtual void OnShow();
    /// called when the entity becomes invisible
    virtual void OnHide();
    /// handle per-frame anim events
    void HandleCharacterAnimEvents(Timing::Time time);
    /// handle per-frame anim-driven-movement stuff
    void HandleCharacterAnimDrivenMotion();
    /// handle tracked character joints
    void HandleTrackedJoints();

    Resources::ResourceId resId;
    bool loadSynced;
    Util::StringAtom rootNodePath;
    Math::matrix44 rootNodeOffsetMatrix;
    Ptr<Models::ManagedModel> managedModel;
    Ptr<Models::ModelInstance> modelInstance;
    Math::matrix44 rot180Transform;

	// instancing
	bool instanced;
	IndexT instanceCode;

	// picking
	IndexT pickingId;

	// lighting
	Ptr<Lighting::EnvironmentProbe> environmentProbe;

    // character stuff
    Ptr<Characters::CharacterInstance> charInst;
    Ptr<Characters::Character> character;
    bool nebula2CharacterRotationHack;
	bool renderSkeleton;

    // anim driven motion
    bool animDrivenMotionTrackingEnabled;
    Util::StringAtom animDrivenMotionJointName;
    Math::vector animDrivenMotionVector;

    // anim events
    bool animEventTrackingEnabled;
    bool animEventOnlyDominatingClip;
    Timing::Tick animEventLastTick;
    Util::Array<Animation::AnimEventInfo> animEventInfos;

    // character joint tracking
    bool jointTrackingEnabled;
    bool trackedJointInfosValid;
    Util::Array<Characters::CharJointInfo> trackedJointInfos;
    Util::Dictionary<Util::StringAtom,IndexT> trackedJoints;    // jointName -> key, jointIndex -> value
};

//------------------------------------------------------------------------------
/**
*/
inline void
ModelEntity::SetResourceId(const Resources::ResourceId& id)
{
    n_assert(!this->IsActive());
    this->resId = id;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
ModelEntity::GetResourceId() const
{
    return this->resId;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelEntity::SetLoadSynced( bool b )
{
    this->loadSynced = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
ModelEntity::GetLoadSynced() const
{
    return this->loadSynced;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ModelEntity::SetRootNodePath(const Util::StringAtom& p)
{
    this->rootNodePath = p;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
ModelEntity::GetRootNodePath() const
{
    return this->rootNodePath;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ModelEntity::SetRootNodeOffsetMatrix(const Math::matrix44& m)
{
    this->rootNodeOffsetMatrix = m;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
ModelEntity::GetRootNodeOffsetMatrix() const
{
    return this->rootNodeOffsetMatrix;
}

//------------------------------------------------------------------------------
/**
*/
inline Resources::Resource::State
ModelEntity::GetModelResourceState() const
{
    return this->managedModel->GetState();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Models::ModelInstance>&
ModelEntity::GetModelInstance() const
{
    return this->modelInstance;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
ModelEntity::IsInstanced() const
{
	return this->instanced;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelEntity::SetInstanced( bool b )
{
	this->instanced = b;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelEntity::SetInstanceCode( const IndexT& i )
{
	this->instanceCode = i;
}

//------------------------------------------------------------------------------
/**
*/
inline const IndexT& 
ModelEntity::GetInstanceCode()
{
	return this->instanceCode;
}

//------------------------------------------------------------------------------
/**
*/
inline void
ModelEntity::SetEnvironmentProbe(const Ptr<Lighting::EnvironmentProbe>& probe)
{
	this->environmentProbe = probe;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Lighting::EnvironmentProbe>&
ModelEntity::GetEnvironmentProbe() const
{
	return this->environmentProbe;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelEntity::SetPickingId( const IndexT& i )
{
	this->pickingId = i;
}

//------------------------------------------------------------------------------
/**
*/
inline const IndexT& 
ModelEntity::GetPickingId() const
{
	return this->pickingId;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ModelEntity::HasCharacter() const
{
    return this->charInst.isvalid();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Characters::CharacterInstance>&
ModelEntity::GetCharacterInstance() const
{
    return this->charInst;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Characters::Character>&
ModelEntity::GetCharacter() const
{
    return this->character;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
ModelEntity::SetRenderSkeleton( bool b )
{
	this->renderSkeleton = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ModelEntity::IsAnimDrivenMotionTrackingEnabled() const 
{
    return this->animDrivenMotionTrackingEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
ModelEntity::GetAnimDrivenMotionJointName() const
{
    return this->animDrivenMotionJointName;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::vector&
ModelEntity::GetAnimDrivenMotionVector() const
{
    return this->animDrivenMotionVector;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ModelEntity::IsAnimEventTrackingEnabled() const
{
    return this->animEventTrackingEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Animation::AnimEventInfo>&
ModelEntity::GetAnimEvents() const
{
    return this->animEventInfos;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ModelEntity::IsCharJointTrackingEnabled() const
{
    return this->jointTrackingEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
ModelEntity::IsCharJointDataValid() const
{
    return this->trackedJointInfosValid;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Characters::CharJointInfo>&
ModelEntity::GetCharJointInfos() const
{
    return this->trackedJointInfos;
}

} // namespace Graphics
//------------------------------------------------------------------------------    