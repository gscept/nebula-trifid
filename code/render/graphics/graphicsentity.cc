//------------------------------------------------------------------------------
//  graphicsentity.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsentity.h"
#include "graphics/stage.h"
#include "graphics/graphicsserver.h"
#include "graphics/graphicsprotocol.h"

namespace Graphics
{
__ImplementClass(Graphics::GraphicsEntity, 'GREN', Core::RefCounted);

using namespace Util;
using namespace Math;
using namespace Messaging;

GraphicsEntity::Id GraphicsEntity::UniqueIdCounter = 0;

//------------------------------------------------------------------------------
/**
*/
GraphicsEntity::GraphicsEntity() :
    transform(matrix44::identity()),
    globalBoxDirty(true),
    links(NumLinkTypes),
    type(GraphicsEntityType::InvalidType),
	isStatic(false),
    isActive(false),
    isValid(false),
	isAlwaysVisible(false),
    isVisible(true),
	castsShadows(true),
    transformChanged(true),
    removeFlag(false),
    renderBeforeFrameIndex(InvalidIndex),
    updateFrameIndex(InvalidIndex),
    notifyCullingVisibleFrameIndex(InvalidIndex),
    entityTime(0.0),
    timeFactor(1.0),
    clipStatus(ClipStatus::Invalid)
{
    this->id = ++UniqueIdCounter;
}

//------------------------------------------------------------------------------
/**
*/
GraphicsEntity::~GraphicsEntity()
{
    n_assert(!this->IsActive());
}

//------------------------------------------------------------------------------
/**
    Activate the entity. This method is called when the entity
    is created an attached to the graphics server. During OnActivate()
    the entity should perform any one-time initializations.
*/
void
GraphicsEntity::OnActivate()
{
    n_assert(!this->IsActive());
    this->isActive = true;
    this->isValid = true;
    this->removeFlag = false;
    GraphicsServer::Instance()->RegisterEntity(this);
}

//------------------------------------------------------------------------------
/**
    Deactivate the entity, this method is called when the entity
    is removed from the graphics server. Any initialization done
    in OnActivate() should be undone here.
*/
void
GraphicsEntity::OnDeactivate()
{
    n_assert(this->IsActive());
    n_assert(this->deferredMessages.IsEmpty());
    if (this->IsAttachedToStage())
    {           
        this->stage->RemoveEntity(this);
    }
    n_assert(!this->IsAttachedToStage());
    IndexT i;
    for (i = 0; i < NumLinkTypes; i++)
    {
        n_assert(this->links[i].IsEmpty());
    }
    GraphicsServer::Instance()->UnregisterEntity(this);
    this->isActive = false;
}

//------------------------------------------------------------------------------
/**
    This method is called when the graphics entity is attached to a stage. An
    entity may only be attached to one stage at any time, but can be
    attached to different stages during its lifetime. Attachging an entity
    to a stage may be relatively slow because the entity must be inserted
    into the cell hierarchy.
*/
void
GraphicsEntity::OnAttachToStage(const Ptr<Stage>& s)
{
    n_assert(this->IsActive());
    n_assert(!this->IsAttachedToStage());
    IndexT i;
    for (i = 0; i < NumLinkTypes; i++)
    {
        n_assert(this->links[i].IsEmpty());
    }
    this->stage = s;
}

//------------------------------------------------------------------------------
/**
    This method is called when the graphics entity is removed from a stage.
*/
void
GraphicsEntity::OnRemoveFromStage()
{
    n_assert(this->IsActive());
    n_assert(this->IsAttachedToStage());
    n_assert(this->deferredMessages.IsEmpty());
    this->ClearLinks(CameraLink);
    this->ClearLinks(LightLink);
    this->stage = 0;
}

//------------------------------------------------------------------------------
/**
    This method is called from the SetVisible() method when the entity 
    changes from invisible to visible state.
*/
void
GraphicsEntity::OnShow()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called from the SetVisible() method when the entity
    changes from visible to invisible state.
*/
void
GraphicsEntity::OnHide()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Called each frame to reset per-frame statistics
*/
void 
GraphicsEntity::OnReset()
{
	// empty
}

//------------------------------------------------------------------------------
/**
    This method is called at the beginning of a frame before culling
    happens on EVERY entity.
*/
void
GraphicsEntity::OnCullBefore(Timing::Time time_, Timing::Time timeFactor_, IndexT frameIndex)
{
    if (this->transformChanged || this->globalBoxDirty)
    {
        this->transformChanged = false;
        this->stage->NotifyOfEntityTransformChange(this);
    }

    // reset clipstatus to invalid, will be set in OnNotifyCullingVisible
    this->UpdateClipStatus(ClipStatus::Outside);

    // save the given time
    this->UpdateTime(time_, timeFactor_);

    // update the update frame index (to detect multiple calls to OnUpdate()
    this->updateFrameIndex = frameIndex;
}

//------------------------------------------------------------------------------
/**
    This method is called during visibility linking when an observed entity
    is found to be visible by an observer (a camera or a light entity).
    NOTE that this method will be called several times per frame, so it may
    be a good idea to use the graphics server's frame counter to protect
    expensive code from multiple execution.
*/
void
GraphicsEntity::OnNotifyCullingVisible(const Ptr<GraphicsEntity>& observer, IndexT frameIndex)
{
    // reset clipstatus to inside
    this->UpdateClipStatus(ClipStatus::Inside);

    // update the frame index (to detect multiple call to OnNotifyVisible()
    this->notifyCullingVisibleFrameIndex = frameIndex;

	// set our local box to whatever it is, even if its modified
	//this->localBoxModified = false;
}

//------------------------------------------------------------------------------
/**
    This method is called on the entity from View::Render()
    once per frame for every visible entity.
*/
void
GraphicsEntity::OnRenderBefore(IndexT frameIndex)
{
    this->renderBeforeFrameIndex = frameIndex;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsEntity::SetTransform(const Math::matrix44& m)
{
    this->transform = m;
    this->globalBoxDirty = true;
    // immediately call transform changed to apply before any attachment is updated
    this->OnTransformChanged();
    this->transformChanged = true;
}

//------------------------------------------------------------------------------
/**
    This method is called whenever the the view comes to its Render method. 
    Override this method in a subclass to define the visible objects. Models must
    add the modelinstances here. LightEntities must be added to the LightServer and
    depending on generating shadows to the ShadowServer.
*/
void
GraphicsEntity::OnResolveVisibility(IndexT frameIndex, bool updateLod /*= false*/)
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called whenever the transformation matrix has changed
    by a call to SetTransform(). Override this method in a subclass if
    it wants to be informed about changes to the transformation matrix.
*/
void
GraphicsEntity::OnTransformChanged()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    This method is called on the graphics entity to render a debug 
    visualization of itself.
*/
void
GraphicsEntity::OnRenderDebug()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsEntity::ClearLinks(LinkType linkType)
{
    this->links[linkType].Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsEntity::UpdateGlobalBoundingBox()
{
    n_assert(this->globalBoxDirty);
    this->globalBox = this->localBox;
    this->globalBox.transform(this->transform);
    this->globalBoxDirty = false;
}

//------------------------------------------------------------------------------
/**
*/
const Ptr<Stage>&
GraphicsEntity::GetStage() const
{
    return this->stage;
}

//------------------------------------------------------------------------------
/**
*/
bool
GraphicsEntity::IsAttachedToStage() const
{
    return this->stage.isvalid();
}

//------------------------------------------------------------------------------
/**
    Compute the clip status between this entity and a bounding box in
    global space. This method must be overwritten in a derived class.
*/
ClipStatus::Type
GraphicsEntity::ComputeClipStatus(const bbox& box)
{
    return ClipStatus::Outside;
}

//------------------------------------------------------------------------------
/**
*/
void
GraphicsEntity::SetVisible(bool b)
{
    if (b != this->isVisible)
    {
        this->isVisible = b;
        if (this->isVisible)
        {
            this->OnShow();
        }
        else
        {
            this->OnHide();
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsEntity::SetAlwaysVisible( bool b )
{
	this->isAlwaysVisible = b;
	if (b)
	{
		this->SetVisible(true);
	}
}


//------------------------------------------------------------------------------
/**
    Message handlers may decide to defer message handling until
    the object has become valid.
*/
void
GraphicsEntity::AddDeferredMessage(const Ptr<Message>& msg)
{
    n_assert(!msg->IsDeferred());
    n_assert(!this->IsValid());
    this->deferredMessages.Append(msg);
    msg->SetDeferred(true);
}

//------------------------------------------------------------------------------
/**
    This method is called once when an object with deferred validation
    (e.g. ModelEntities) become valid (usually after their resources
    have finished loading). Any deferred messages will be processed here.
*/
void
GraphicsEntity::HandleDeferredMessages()
{
    n_assert(this->IsValid());
    IndexT i;
    for (i = 0; i < this->deferredMessages.Size(); i++)
    {
        this->HandleMessage(this->deferredMessages[i]);
        this->deferredMessages[i]->SetDeferredHandled(true);
    }
    this->deferredMessages.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsEntity::Send( const Ptr<Graphics::GraphicsEntityMessage>& msg )
{
    n_assert(msg.isvalid());
    this->HandleMessage(msg.upcast<Messaging::Message>());
    if (this->IsValid())
    {
        msg->SetHandled(true);
    }
}

//------------------------------------------------------------------------------
/**
    Handle a message, override this method accordingly in subclasses!
*/
void
GraphicsEntity::HandleMessage(const Ptr<Message>& msg)
{
    __Dispatch(GraphicsEntity, this, msg);
}

} // namespace Graphics