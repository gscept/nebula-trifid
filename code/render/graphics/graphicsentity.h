#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::GraphicsEntity
    
    A graphics entity represents an atomic graphics object which can be
    attached to a Stage. Graphics entities come in three flavours:
    
    - ModelEntity: a visible model instance
    - LightEntity: a light source
    - CameraEntity: a camera 
    
    Visibility queries set graphics entities in relation to each other 
    through bidirectional links. A CameraEntity links to all 
    ModelEntities and LightEntities visible through the camera. Since
    visibility links are bidirectional, ModelEntities and LightEntities
    also know through which cameras they are visible. LightEntities
    have links to all ModelEntities they influence, and ModelEntities
    know by which lights they are lit. 
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "math/bbox.h"
#include "math/clipstatus.h"
#include "timing/time.h"
#include "graphics/graphicsentitytype.h"
#include "messaging/message.h"
#include "messaging/staticmessagehandler.h"

namespace Visibility
{
    class VisibilityChecker;
}

namespace Particles
{
    class ParticleSystemNodeInstance;
}

namespace Lighting
{
	class ShadowServer;
}
//------------------------------------------------------------------------------
namespace Graphics
{
class Stage;
class Cell;
class GraphicsEntityMessage;

class GraphicsEntity : public Core::RefCounted
{
    __DeclareClass(GraphicsEntity);
public:
    /// visibility link types
    enum LinkType
    {
        CameraLink = 0,
        LightLink,
        
        NumLinkTypes,
    };

	/// render priorities
	enum PriorityType
	{
		PriorityHigh = 0,
		PriorityMedium = 1,
		PriorityLow = 2
	};

    /// a unique id for graphics entities
    typedef IndexT Id;

    /// constructor
    GraphicsEntity();
    /// destructor
    virtual ~GraphicsEntity();

    /// return true if entity is currently active (is between OnActivate()/OnDeactivate()
    bool IsActive() const;
    /// return true if entity is current valid (ready for rendering)
    bool IsValid() const;
    /// get the graphics entity's unique id
    Id GetId() const;
    /// get the entity type
    GraphicsEntityType::Code GetType() const;

    /// set the entity's world space transform
    void SetTransform(const Math::matrix44& m);
    /// get the entity's world space transform
    const Math::matrix44& GetTransform() const;
    /// returns true if the transformed changed this frame
    const bool TransformChangedThisFrame() const;
    
    /// set the entity's visibility
    void SetVisible(bool b);
    /// return true if entity is set to visible
    bool IsVisible() const;
	/// sets if the entity should always be visible
	void SetAlwaysVisible(bool b);
	/// returns if the entity is always visible
	bool IsAlwaysVisible() const;
	/// return clipping status of entity
	const Math::ClipStatus::Type& GetClipStatus() const;

	/// set this graphics entity to be static
	void SetStatic(bool b);
	/// get if this graphics entity is a static one
	const bool IsStatic() const;
	/// set if this entity should cast shadows (only viable for shapes and not lights)
	void SetCastsShadows(bool b);
	/// get if this entity should cast shadows (material must support this too, obviously)
	const bool GetCastsShadows() const;

    /// get the stage this entity is attached to
    const Ptr<Stage>& GetStage() const;
    /// return true if entity is attached to stage
    bool IsAttachedToStage() const;

    /// get current entity time
    Timing::Time GetEntityTime() const;
    /// get the local space bounding box
    const Math::bbox& GetLocalBoundingBox();
    /// get bounding box in global space
    const Math::bbox& GetGlobalBoundingBox();

    /// clear all visibility links
    void ClearLinks(LinkType linkType);
    /// add visibility link
    void AddLink(LinkType linkType, const Ptr<GraphicsEntity>& entity);
    /// get visibility links by type
    const Util::Array<Ptr<GraphicsEntity> >& GetLinks(LinkType type) const;

    /// compute clip status against bounding box
    virtual Math::ClipStatus::Type ComputeClipStatus(const Math::bbox& box);

    /// mark the entity for removal from the stage at the next possible time
    void MarkRemove();
    /// return true if this entity has been marked for removal
    bool IsMarkedForRemove() const;

    /// sends a message to this entity
    void Send(const Ptr<Graphics::GraphicsEntityMessage>& msg);
    /// handle a message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);
    /// add a message for deferred handling once the object becomes valid
    void AddDeferredMessage(const Ptr<Messaging::Message>& msg);

protected:
    friend class Stage;
    friend class View;
    friend class GraphicsServer;
    friend class Messaging::StaticMessageHandler;   
    friend class Visibility::VisibilityChecker;
    friend class Particles::ParticleSystemNodeInstance;

    /// set entity type, call in constructor of derived class!
    void SetType(GraphicsEntityType::Code t);
    /// set to valid state (when the entity becomes ready for rendering)
    void SetValid(bool b);
    /// update current clip status
    void UpdateClipStatus(Math::ClipStatus::Type c);
    /// update current time
    void UpdateTime(Timing::Time time, Timing::Time timeFactor);
    
    /// called when entity is created
    virtual void OnActivate();
    /// called before entity is destroyed
    virtual void OnDeactivate();

    /// called when attached to Stage
    virtual void OnAttachToStage(const Ptr<Stage>& stage);
    /// called when removed from Stage
    virtual void OnRemoveFromStage();
    
    /// called when the entity becomes visible
    virtual void OnShow();
    /// called when the entity becomes invisible
    virtual void OnHide();

    /// called per frame to
    virtual void OnReset();

    /// called from Render method in view
    virtual void OnResolveVisibility(IndexT frameIndex, bool updateLod = false);
    /// called when transform matrix changed
    virtual void OnTransformChanged();
    /// called before culling on each(!) graphics entity (visible or not!)
    virtual void OnCullBefore(Timing::Time time, Timing::Time globalTimeFactor, IndexT frameIndex);
    /// called when the entity has been found visible during culling, may be called several times per frame!
    virtual void OnNotifyCullingVisible(const Ptr<GraphicsEntity>& observer, IndexT frameIndex);
    /// called right before rendering
    virtual void OnRenderBefore(IndexT frameIndex);
    /// called to render a debug visualization of the entity
    virtual void OnRenderDebug();
    
    /// set the local space bounding box
    void SetLocalBoundingBox(const Math::bbox& b);
	/// extend the local bounding box (use with dynamic bounding boxes)
	void ExtendLocalBoundingBox(const Math::bbox& b);
    /// update the global bounding box from the transform and local box
    void UpdateGlobalBoundingBox();
    /// handle deferred messages (called by subclasses once resources are loaded)
    void HandleDeferredMessages();

    static Id UniqueIdCounter;
    Id id;
    Math::matrix44 transform;

    Math::bbox localBox;
    Math::bbox globalBox;
    bool globalBoxDirty;
	bool localBoxModified;

    Ptr<Stage> stage;
    Util::FixedArray<Util::Array<Ptr<GraphicsEntity> > > links;
    GraphicsEntityType::Code type;
	bool isStatic;
	bool castsShadows;
    bool isActive;
    bool isValid;
    bool isVisible;
	bool isAlwaysVisible;
    bool transformChanged;
    bool removeFlag;
    IndexT renderBeforeFrameIndex;
    IndexT updateFrameIndex;
    IndexT notifyCullingVisibleFrameIndex;
    Timing::Time entityTime;
    float timeFactor;
    Math::ClipStatus::Type clipStatus;
    Util::Array<Ptr<Messaging::Message> > deferredMessages;
};


//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::MarkRemove()
{
    this->removeFlag = true;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
GraphicsEntity::IsMarkedForRemove() const
{
    return this->removeFlag;
}

//------------------------------------------------------------------------------
/**
*/
inline GraphicsEntity::Id
GraphicsEntity::GetId() const
{
    return this->id;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
GraphicsEntity::IsActive() const
{
    return this->isActive;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::SetValid(bool b)
{
    this->isValid = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
GraphicsEntity::IsValid() const
{
    return this->isValid;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::SetType(GraphicsEntityType::Code t)
{
    this->type = t;
}

//------------------------------------------------------------------------------
/**
*/
inline GraphicsEntityType::Code
GraphicsEntity::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44&
GraphicsEntity::GetTransform() const
{
    return this->transform;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
GraphicsEntity::TransformChangedThisFrame() const
{
    return this->transformChanged;
}

//------------------------------------------------------------------------------
/**
*/
inline bool
GraphicsEntity::IsVisible() const
{
    return this->isVisible;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
GraphicsEntity::IsAlwaysVisible() const
{
	return this->isAlwaysVisible;
}
//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::SetStatic(bool b)
{
	this->isStatic = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
GraphicsEntity::IsStatic() const
{
	return this->isStatic;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::SetCastsShadows(bool b)
{
	this->castsShadows = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool
GraphicsEntity::GetCastsShadows() const
{
	return this->castsShadows;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::ClipStatus::Type& 
GraphicsEntity::GetClipStatus() const
{
	return this->clipStatus;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::SetLocalBoundingBox(const Math::bbox& b)
{
    this->localBox = b;
    this->globalBoxDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::ExtendLocalBoundingBox(const Math::bbox& b)
{
	this->localBox.extend(b);
	this->localBoxModified = true;
	this->globalBoxDirty = true;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::bbox&
GraphicsEntity::GetLocalBoundingBox()
{
    return this->localBox;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::bbox&
GraphicsEntity::GetGlobalBoundingBox()
{
    // and now if something global happened
    if (this->globalBoxDirty)
    {
        this->UpdateGlobalBoundingBox();
    }
    return this->globalBox;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::AddLink(LinkType linkType, const Ptr<GraphicsEntity>& entity)
{
    this->links[linkType].Append(entity);
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<GraphicsEntity> >&
GraphicsEntity::GetLinks(LinkType linkType) const
{
    return this->links[linkType];
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::UpdateClipStatus(Math::ClipStatus::Type c)
{
    this->clipStatus = c;
}

//------------------------------------------------------------------------------
/**
*/
inline void
GraphicsEntity::UpdateTime(Timing::Time t, Timing::Time f)
{
    this->entityTime = t;
    this->timeFactor = float(f);
}

//------------------------------------------------------------------------------
/**
*/
inline Timing::Time
GraphicsEntity::GetEntityTime() const
{
    return this->entityTime;
}

} // namespace Graphics
//------------------------------------------------------------------------------
    