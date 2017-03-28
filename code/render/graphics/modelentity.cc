//------------------------------------------------------------------------------
//  modelentity.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/modelentity.h"
#include "models/modelserver.h"
#include "coregraphics/shaperenderer.h"
#include "threading/thread.h"
#include "graphics/graphicsserver.h"
#include "characters/characternode.h"
#include "characters/characternodeinstance.h"
#include "characters/characterserver.h"
#include "animation/animeventserver.h"
#include "particles/particlesystemnode.h"
#include "resources/resourcemanager.h"
#include "models/nodes/statenodeinstance.h"
#include "materials/materialvariableinstance.h"
#include "renderutil/nodelookuputil.h"
#include "coreanimation/animevent.h"

namespace Graphics
{
__ImplementClass(Graphics::ModelEntity, 'IMDE', Graphics::GraphicsEntity);

using namespace Threading;
using namespace CoreGraphics;
using namespace Math;
using namespace Models;
using namespace Resources;
using namespace Characters;
using namespace Animation;
using namespace Messaging;
using namespace Util;

//------------------------------------------------------------------------------
/**
*/
ModelEntity::ModelEntity() :
    rootNodeOffsetMatrix(matrix44::identity()),
    rot180Transform(matrix44::identity()),
	instanced(false),
    nebula2CharacterRotationHack(true),
    animDrivenMotionTrackingEnabled(false),
    animDrivenMotionVector(vector::nullvec()),
    animEventTrackingEnabled(false),
    animEventOnlyDominatingClip(true),
    animEventLastTick(-1),
    jointTrackingEnabled(false),
	renderSkeleton(false),
    loadSynced(false)
{
    this->SetType(GraphicsEntityType::Model);

	// set default environment probe
	this->environmentProbe = Lighting::EnvironmentProbe::DefaultEnvironmentProbe;
}

//------------------------------------------------------------------------------
/**
*/
ModelEntity::~ModelEntity()
{
    n_assert(!this->modelInstance.isvalid());
}

//------------------------------------------------------------------------------
/**
*/
void
ModelEntity::ConfigureAnimDrivenMotionTracking(bool enabled, const StringAtom& jointName)
{
    this->animDrivenMotionTrackingEnabled = enabled;
    this->animDrivenMotionJointName = jointName;
}

//------------------------------------------------------------------------------
/**
*/
void
ModelEntity::ConfigureAnimEventTracking(bool enabled, bool onlyDominatingClip)
{
    this->animEventTrackingEnabled = enabled;
    this->animEventOnlyDominatingClip = onlyDominatingClip;
}

//------------------------------------------------------------------------------
/**
*/
void
ModelEntity::ConfigureCharJointTracking(bool enabled, const Array<StringAtom>& jointNames)
{
    this->jointTrackingEnabled = enabled;
    if (enabled)
    {
        n_assert(jointNames.Size() > 0);
        n_assert(this->trackedJoints.IsEmpty());
        this->trackedJoints.Reserve(jointNames.Size());
        IndexT i;
        for (i = 0; i < jointNames.Size(); i++)
        {
            this->trackedJoints.Add(jointNames[i], InvalidIndex);
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
ModelEntity::AddTrackedCharJoint(const StringAtom& jointName)
{
    n_assert(!this->trackedJoints.Contains(jointName));
    this->trackedJoints.Add(jointName, InvalidIndex);
    this->jointTrackingEnabled = true;
}

//------------------------------------------------------------------------------
/**
    NOTE: the method returns 0 if the joint is not valid or has not been found!
*/
const Characters::CharJointInfo*
ModelEntity::GetTrackedCharJointInfo(const Util::StringAtom& jointName) const
{
    // note: we assume that relatively few joints will be tracked
    if (this->jointTrackingEnabled && this->trackedJointInfosValid)
    {
        IndexT i;
        for (i = 0; i < this->trackedJointInfos.Size(); i++)
        {
            if (this->trackedJointInfos[i].GetJointName() == jointName)
            {
                return &(this->trackedJointInfos[i]);
            }
        }
    }
    // not found
    return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ModelEntity::SetSurfaceConstant(const Util::String& nodeName, const Util::String& varName, const Util::Variant& value)
{
    Ptr<UpdModelNodeInstanceSurfaceConstant> msg = UpdModelNodeInstanceSurfaceConstant::Create();
	msg->SetModelNodeInstanceName(nodeName);
	msg->SetName(varName);
	msg->SetValue(value);
	__Send(this, msg);
}

//------------------------------------------------------------------------------
/**
*/
void
ModelEntity::SetSurfaceConstant(const Util::String& nodeName, const Util::String& varName, const Ptr<CoreGraphics::Texture>& tex)
{
    Ptr<UpdModelNodeInstanceSurfaceConstant> msg = UpdModelNodeInstanceSurfaceConstant::Create();
	msg->SetModelNodeInstanceName(nodeName);
    msg->SetName(varName);
	msg->SetValue(Variant(tex));
	__Send(this, msg);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelEntity::SetMaterialVariableByName( const Util::String& varName, const Util::Variant& value )
{
	Ptr<UpdMaterialVariable> msg = UpdMaterialVariable::Create();
    msg->SetName(varName);
	msg->SetValue(value);
	__Send(this, msg);
}

//------------------------------------------------------------------------------
/**
    This will initialize the managed model inside the ModelEntity.
*/
void
ModelEntity::OnActivate()
{
    n_assert(!this->IsActive());
    n_assert(this->resId.IsValid());
    n_assert(!this->charInst.isvalid());
    n_assert(!this->character.isvalid());
    GraphicsEntity::OnActivate();
    
    // note: we will remain invalid until at least our model has loaded
    this->SetValid(false);
    if (!this->managedModel.isvalid())
    {
        this->managedModel = ModelServer::Instance()->LoadManagedModel(this->resId, loadSynced);
    }  
    this->ValidateModelInstance();

#if NEBULA3_DEBUG
    //n_printf("ModelEntity::OnActivate(): %s\n", this->resId.Value());
#endif
}

//------------------------------------------------------------------------------
/**
    Cleanup our managed model, and ModelInstance if it is already 
    initialized.
*/
void
ModelEntity::OnDeactivate()
{
    n_assert(this->IsActive());
    n_assert(this->managedModel.isvalid());

    // clear character instance reference (if valid)
    this->charInst = 0;
    this->character = 0;

    // discard our model instance (if exists)
    if (this->modelInstance.isvalid())
    {        
		if (this->modelInstance->IsValid())
		{
			// hmm... this doesn't look very elegant...
			this->modelInstance->GetModel()->DiscardInstance(this->modelInstance);
		}        
        this->modelInstance = 0;
    }
	else
	{
		// we might have unhandled deferred messages since our model failed to load, so lets just empty it
		this->deferredMessages.Clear();
	}

    // discard our managed model
    if (ModelServer::Instance()->HasManagedModel(this->managedModel->GetResourceId()))
    {
        ModelServer::Instance()->DiscardManagedModel(this->managedModel);
    }    
    this->managedModel = 0;

    // up to parent class
    GraphicsEntity::OnDeactivate();
}

//------------------------------------------------------------------------------
/**
    This method is called once per frame before OnCullBefore() to 
    initialize the shared data object with suitable data (which may be
    overwritten with uptodate-data later in the frame). This is necessary
    because the SharedData object is double buffered, and thus if an update
    if missed for one frame invalid data from the previous frame may "leak"
    into the next frame.
*/
void
ModelEntity::OnReset()
{
    this->trackedJointInfosValid = false;
	this->trackedJointInfos.Clear();
    this->animDrivenMotionVector = vector::nullvec();

    // initialize the tracked joint data with the bind pose joint positions,
    // the actual position will only be updated if the character is currently visible!
    matrix44 globalMatrix = this->rot180Transform;
    if (this->charInst.isvalid() && this->jointTrackingEnabled)
    {
        this->trackedJointInfosValid = true;
        IndexT i;
        for (i = 0; i < this->trackedJoints.Size(); i++)
        {
            // first validate joint indices in trackedJoints array which are
            // not valid yet (new joints may have been added dynamically)
            const StringAtom& jointName = this->trackedJoints.KeyAtIndex(i);
            IndexT jointIndex;
            if (this->trackedJoints.ValueAtIndex(i) == InvalidIndex)
            {
                jointIndex = this->character->Skeleton().GetJointIndexByName(jointName);
                if (InvalidIndex == jointIndex)
                {
                    n_error("ModelEntity::HandleTrackedJoints(): invalid joint name '%s'!\n", jointName.Value());
                }
                this->trackedJoints.ValueAtIndex(i) = jointIndex;
            }
            else
            {
                jointIndex = this->trackedJoints.ValueAtIndex(i);
            }

            // update joint matrices to pose matrix
            const matrix44& localPoseMatrix = this->character->Skeleton().GetJoint(jointIndex).GetPoseMatrix();
            matrix44 globalPoseMatrix = matrix44::multiply(localPoseMatrix, this->rot180Transform);
            CharJointInfo jointInfo(jointName, localPoseMatrix, globalPoseMatrix);
            this->trackedJointInfos.Append(jointInfo);
        }
    }
}

//------------------------------------------------------------------------------
/**
    This method is called whenever the the view comes to its Render method. 
    Add light entities to the LightServer or to the ShadowServer.
*/
void
ModelEntity::OnResolveVisibility(IndexT frameIndex, bool updateLod)
{
	if (this->modelInstance.isvalid())
	{
		VisResolver::Instance()->AttachVisibleModelInstancePlayerCamera(frameIndex, this->modelInstance, updateLod);
	}
}

//------------------------------------------------------------------------------
/**
    In OnTransformChanged() we need to update the transformation
    of our ModelInstance (if it has already been initialised).
*/
void
ModelEntity::OnTransformChanged()
{
    if (this->modelInstance.isvalid())
    {        
		this->modelInstance->SetTransform(this->GetTransform());        
    }
}

//------------------------------------------------------------------------------
/**
    This method is called each frame on each entity (visible or not),
    so keep this method cheap!
*/
void
ModelEntity::OnCullBefore(Timing::Time time_, Timing::Time timeFactor_, IndexT frameIndex)
{
    // only perform the update once per frame
    if (this->updateFrameIndex != frameIndex)
    {
        // first check if our model has been loaded already, if yes
        // our model instance must be initialized 
        if (!this->IsValid())
        {
            this->ValidateModelInstance();
        }  

        if (this->charInst.isvalid())
        {
            // update animation events even if character is not visible
            this->HandleCharacterAnimEvents(time_);

            // update character timings
            Timing::Tick ticks = Timing::SecondsToTicks(time_);
            this->charInst->UpdateTime(ticks);
        }
    }

    // important: call parent class!
    GraphicsEntity::OnCullBefore(time_, timeFactor_, frameIndex);
}

//------------------------------------------------------------------------------
/**
    This method is called once (per-view) before the entity is rendered,
    and only for visible entities. Please note that this method
    may be called several times per frame!
*/
void
ModelEntity::OnNotifyCullingVisible(const Ptr<GraphicsEntity>& observer, IndexT frameIndex)
{
    // only execute once per frame
    if (this->notifyCullingVisibleFrameIndex != frameIndex)
    {
        if (this->IsVisible())
        {
            // call back our model-instance
            if (this->modelInstance.isvalid())
            {
                this->modelInstance->OnNotifyCullingVisible(frameIndex, this->entityTime);	

				// reset bounding box
				if (this->localBoxModified && this->modelInstance->IsValid())
				{
					this->SetLocalBoundingBox(this->modelInstance->GetModel()->GetBoundingBox());
				}
            }
        }

        // call parent-class
        GraphicsEntity::OnNotifyCullingVisible(observer, frameIndex);
    }
}

//------------------------------------------------------------------------------
/**
    This method is called on the entity from View::Render()
    once per frame for every visible entity.
*/
void
ModelEntity::OnRenderBefore(IndexT frameIndex)
{    
	if (this->renderBeforeFrameIndex != frameIndex)
	{
		// update character if valid
		if (this->charInst.isvalid() && this->charInst->IsValid())
		{
			this->charInst->WaitUpdateDone();
			this->HandleCharacterAnimDrivenMotion();
			this->HandleTrackedJoints();

			// render skeleton
			if (this->renderSkeleton)
			{
				this->charInst->RenderDebug(this->rootNodeOffsetMatrix);
			}
		}

		// if our model instance is valid, let it update itself
		if (this->modelInstance.isvalid())
		{
			// update model instance
			this->modelInstance->OnRenderBefore(frameIndex, this->entityTime);
		}

		GraphicsEntity::OnRenderBefore(frameIndex);
	}  
}

//------------------------------------------------------------------------------
/**
    This creates and initializes our ModelInstance if needed. Since Model 
    loading happens asynchronously this may happen at any time after
    the ModelEntity is activated.
*/
void
ModelEntity::ValidateModelInstance()
{
    n_assert(!this->IsValid());
    if (!this->modelInstance.isvalid())
    {
		// if model is loaded (resource) or failed (placeholder), setup entity
		// if pending, wait for it to load
        if (this->managedModel->GetState() == Resource::Loaded ||
			this->managedModel->GetState() == Resource::Failed)
        {
            const Ptr<Model>& model = this->managedModel->GetModel();
            n_assert(model->IsLoaded());
            
            // only set the entity to valid if the complete 
            // model node is completely loaded (no placeholders)
            // FIXME: this should be better adjustable...
            // e.g. meshes must be loaded, but textures may
            // still be loading?
            if (model->CheckPendingResources())
            {
                this->SetValid(true);

                // reset local bounding box
                this->SetLocalBoundingBox(model->GetBoundingBox());

                // set transform changed, to update global bounding box
                this->transformChanged = true;

                if (this->rootNodePath.IsValid())
                {
                    this->modelInstance = model->CreatePartialInstance(this->rootNodePath, this->rootNodeOffsetMatrix);
                }
                else
                {
                    this->modelInstance = model->CreateInstance();
                }
				this->modelInstance->SetPickingId(this->pickingId);
                this->modelInstance->SetTransform(this->transform);
                this->modelInstance->SetModelEntity(this);
				this->modelInstance->SetInstanced(this->instanced);
				if (this->isVisible)
                {
                    this->OnShow();
                }
                else
                {
                    this->OnHide();
                }

                // setup character if this model is a character
                this->ValidateCharacter();

                // handle deferred messages
                this->HandleDeferredMessages();
            }
        }
		/*
        else if (this->managedModel->GetState() == Resource::Failed)
        {
            // loading failed, will use placeholder
            this->SetValid(true);
            const Ptr<Model>& model = this->managedModel->GetModel();

            // create instance
            this->modelInstance = model->CreateInstance();

            // set id, transform and instance parameter
            this->modelInstance->SetPickingId(this->pickingId);
            this->modelInstance->SetTransform(this->GetTransform());
            this->modelInstance->SetModelEntity(this);
            this->modelInstance->SetInstanced(this->instanced);
            
            // show/hide depending on how this entity is visible
            if (this->IsVisible())
            {
                this->OnShow();
            }
            else
            {
                this->OnHide();
            }

            // output message to inform this entity is placeholder
            n_printf("Failed to load model entity '%s'!", this->resId.Value());
        }
		*/
    }
}


//------------------------------------------------------------------------------
/**
    This will see if the model resource is a character, and if so copy the character instance and character pointer to this model entity.

    This is really only viable to call from outside the model entity if the model has been reloaded since 
    the model entity is completely unaware of the state of resource when the modelInstance pointer is set.
*/
void 
ModelEntity::ValidateCharacter()
{
    n_assert(this->modelInstance.isvalid());

    // check if the model instance contains a character
    const Ptr<ModelNodeInstance>& rootNodeInst = this->modelInstance->GetRootNodeInstance();
    if (rootNodeInst->IsA(CharacterNodeInstance::RTTI))
    {
        this->charInst = rootNodeInst.downcast<CharacterNodeInstance>()->GetCharacterInstance();
        this->character = rootNodeInst->GetModelNode().downcast<CharacterNode>()->GetCharacter();

        // setup anim driven motion if requested
        if (this->animDrivenMotionTrackingEnabled)
        {
            IndexT jointIndex = this->character->Skeleton().GetJointIndexByName(this->animDrivenMotionJointName);
            this->charInst->AnimController().SetupAnimDrivenMotion(jointIndex);
        }
    }
}

//------------------------------------------------------------------------------
/**
    Handle character anim event stuff, always call this method, even
    if anim event tracking is not enabled.
*/
void
ModelEntity::HandleCharacterAnimEvents(Timing::Time time)
{
    n_assert(this->charInst.isvalid());
    if (this->animEventTrackingEnabled)
    {
        // clear previous infos
        this->animEventInfos.Clear();

        // first frame time exception
        Timing::Tick timeInTicks = Timing::SecondsToTicks(time);
        if (-1 == this->animEventLastTick)
        {
            this->animEventLastTick = timeInTicks;
        }

        // now get animevents from last time till current time
        Timing::Tick startTime = this->animEventLastTick;
        Timing::Tick endTime = timeInTicks;
		Animation::AnimSequencer& sequencer = this->charInst->AnimController().AnimSequencer();
        this->animEventInfos = sequencer.EmitAnimEvents(startTime, endTime, this->animEventOnlyDominatingClip, "");
    
        if (this->animEventInfos.Size() > 0)
        { 
            // populate entity events with entity id
            IndexT i;
            for (i = 0; i < this->animEventInfos.Size(); i++)
            {
                this->animEventInfos[i].SetEntityId(this->GetId());
            }

            // perform render-thread-event handling
            Animation::AnimEventServer::Instance()->HandleAnimEvents(this->animEventInfos);
        }
        this->animEventLastTick = timeInTicks;
    }
}

//------------------------------------------------------------------------------
/**
    Handle character anim driven motion feedback. This is called once
    per frame for visible characters.
*/
void
ModelEntity::HandleCharacterAnimDrivenMotion()
{
    n_assert(this->charInst.isvalid());
    if (this->animDrivenMotionTrackingEnabled)
    {
        n_assert(this->charInst->AnimController().IsAnimDrivenMotionEnabled());
        Math::vector vec = this->charInst->AnimController().GetAnimDrivenMotionVector();
        vec = float4::multiply(vec, float4(this->timeFactor, this->timeFactor, this->timeFactor, 1.0f));
        vec = matrix44::transform(vec, this->GetTransform());
        this->animDrivenMotionVector = vec;
    }
}

//------------------------------------------------------------------------------
/**
    Handle the tracked character joints if joint tracking is enabled.
    NOTE: it's important that this method is called after the character's
    asynchronous update jobs have finished!
*/
void
ModelEntity::HandleTrackedJoints()
{
    n_assert(this->charInst.isvalid());
    if (this->jointTrackingEnabled)
    {
        // update tracked joint array in shared data, the array has
        // already been initialised with the pose matrix in OnResetSharedData!
        n_assert(this->trackedJointInfos.Size() == this->trackedJoints.Size());
        IndexT i;
        for (i = 0; i < this->trackedJoints.Size(); i++)
        {
            IndexT jointIndex = this->trackedJoints.ValueAtIndex(i);
            n_assert(InvalidIndex != jointIndex);
            const matrix44& localMatrix = this->charInst->Skeleton().GetJointMatrix(jointIndex);
            matrix44 globalMatrix = matrix44::multiply(localMatrix, this->rot180Transform);
            this->trackedJointInfos[i].SetLocalMatrix(localMatrix);
            this->trackedJointInfos[i].SetGlobalMatrix(globalMatrix);
        }
        this->trackedJointInfosValid = true;
    }
}

//------------------------------------------------------------------------------
/**
    Render our debug visualization (the bounding box).
*/
void
ModelEntity::OnRenderDebug()
{
    // render bounding box
    float4 color(1.0f, 0.0f, 0.0f, 1.0f);
    ShapeRenderer::Instance()->AddWireFrameBox(this->globalBox, color, Thread::GetMyThreadId());

    // display anim events
    IndexT i;
    for (i = 0; i < this->animEventInfos.Size(); i++)
    {
        const Animation::AnimEventInfo& info = this->animEventInfos[i];
        const CoreAnimation::AnimEvent& event = info.GetAnimEvent();
        
        n_printf("Anim event %s/%s triggered at time %d\n", 
            event.GetName().AsString().AsCharPtr(), 
            event.GetCategory().AsString().AsCharPtr(), 
            event.GetTime());
        /*
        Util::String text;
        text.Format("Anim event '%s/%s' emitted", info->GetAnimEvent().GetCategory().AsString().AsCharPtr(), info->GetAnimEvent().GetName().AsString().AsCharPtr());
        CoreGraphics::TextElement debugText(Threading::Thread::GetMyThreadId(), text, float4(1), float2(screenPos.x(), screenPos.y()), 16);
        TextRenderer::Instance()->AddTextElement(debugText);
        */
    }

    // let model instance render its debug stuff
    this->modelInstance->RenderDebug();
}

//------------------------------------------------------------------------------
/**
    This method is called from the SetVisible() method when the entity 
    changes from invisible to visible state.
*/
void
ModelEntity::OnShow()
{
    if (this->modelInstance.isvalid())
    {
        this->modelInstance->OnShow(this->entityTime);
    }
    GraphicsEntity::OnShow();
}

//------------------------------------------------------------------------------
/**
    This method is called from the SetVisible() method when the entity
    changes from visible to invisible state.
*/
void
ModelEntity::OnHide()
{
    if (this->modelInstance.isvalid())
    {
        this->modelInstance->OnHide(this->entityTime);
    }
    GraphicsEntity::OnHide();
}

//------------------------------------------------------------------------------
/**
    Handle a message, override this method accordingly in subclasses!
*/
void
ModelEntity::HandleMessage(const Ptr<Message>& msg)
{
	if (!this->IsValid())
	{
		this->ValidateModelInstance();
	}	
    __Dispatch(ModelEntity, this, msg);
}


} // namespace Graphics
