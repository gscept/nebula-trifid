//------------------------------------------------------------------------------
//  modelentityhandler.cc
//  (C) 2010 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/graphicsprotocol.h"
#include "messaging/staticmessagehandler.h"
#include "graphics/modelentity.h"
#include "graphics/graphicsserver.h"
#include "graphics/stage.h"
#include "framesync/framesynctimer.h"
#include "characters/character.h"
#include "characters/characterinstance.h"
#include "models/nodes/statenodeinstance.h"
#include "models/nodes/statenode.h"
#include "materials/materialvariableinstance.h"
#include "renderutil/nodelookuputil.h"
#include "animation/animeventserver.h"
#include "animation/playclipjob.h"
#include "coreanimation/animresource.h"
#include "particles/particlesystemnodeinstance.h"
#include "particles/particlesystemnode.h"
#include "resources/resourcemanager.h"
#include "models/modelnodeinstance.h"
#include "graphics/billboardentity.h"
#include "resources/managedtexture.h"
#include "materials/surfaceconstantinstance.h"

using namespace CoreGraphics;
using namespace Graphics;
using namespace Graphics;
using namespace Animation;
using namespace FrameSync;
using namespace Characters;
using namespace Math;
using namespace CoreAnimation;
using namespace Models;
using namespace Billboards;

namespace Messaging
{

//------------------------------------------------------------------------------
/**
*/
__StaticHandler(CreateModelEntity)
{
    // create a new model entity
    Ptr<ModelEntity> modelEntity = ModelEntity::Create();
    modelEntity->ConfigureAnimDrivenMotionTracking(msg->GetAnimDrivenMotionEnabled(), msg->GetAnimDrivenMotionJointName());
    modelEntity->ConfigureAnimEventTracking(msg->GetAnimEventsEnabled(), msg->GetAnimEventsOnlyDominatingClip());
    modelEntity->ConfigureCharJointTracking(msg->GetCharJointTrackingEnabled(), msg->GetTrackedCharJointNames());
    modelEntity->SetTransform(msg->GetTransform());
    modelEntity->SetVisible(msg->GetVisible());
    modelEntity->SetResourceId(msg->GetResourceId());
    modelEntity->SetRootNodePath(msg->GetRootNodePath());
    modelEntity->SetRootNodeOffsetMatrix(msg->GetRootNodeOffsetMatrix());
	modelEntity->SetInstanced(msg->GetInstanced());
	modelEntity->SetInstanceCode(msg->GetInstanceCode());
	modelEntity->SetPickingId(msg->GetPickingId());

    // lookup stage and attach entity
    const Ptr<Stage>& stage = GraphicsServer::Instance()->GetStageByName(msg->GetStageName());
    stage->AttachEntity(modelEntity.cast<GraphicsEntity>());

    // set return value
    msg->GetObjectRef()->Validate<ModelEntity>(modelEntity.get());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, SetVisibility)
{
    obj->SetVisible(msg->GetVisible());
}


//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, SetRenderSkeleton)
{
	obj->SetRenderSkeleton(msg->GetRender());
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, UpdModelNodeInstanceVisibility)
{
    // need to defer handling until model is loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        Ptr<ModelInstance> modelInstance = obj->GetModelInstance();
        n_assert(modelInstance.isvalid());    
        Ptr<ModelNodeInstance> nodeInst = modelInstance->LookupNodeInstance(msg->GetModelNodeInstanceName().Value());
        n_assert(nodeInst.isvalid());
        nodeInst->SetVisible(msg->GetVisible(), obj->GetEntityTime());
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, UpdModelNodeInstanceSurfaceConstant)
{
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else
	{
		Ptr<StateNodeInstance> stateNodeInst = RenderUtil::NodeLookupUtil::LookupStateNodeInstance(obj, msg->GetModelNodeInstanceName().Value());
		if (stateNodeInst.isvalid())
		{
            const Ptr<Materials::SurfaceInstance>& surface = stateNodeInst->GetSurfaceInstance();
            const Ptr<Materials::SurfaceConstant>& var = surface->GetConstant(msg->GetName());
			
			// if the type is a string, load it as a texture
            const Util::Variant& value = msg->GetValue();
			if (value.GetType() == Util::Variant::String)
			{
				Ptr<Resources::ManagedTexture> tex = Resources::ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, value.GetString(), NULL, true).downcast<Resources::ManagedTexture>();
				var->SetTexture(tex->GetTexture());
				tex = 0;
			}
			else
			{
                // otherwise just set the value
				var->SetValue(value);   
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, UpdMaterialVariable)
{
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else
	{
		// get all nodes
		const Util::Array<Ptr<ModelNodeInstance>>& nodes = obj->GetModelInstance()->GetNodeInstances();

        // go through all nodes and set them
		const Util::Variant& value = msg->GetValue();
		for (IndexT i = 0; i < nodes.Size(); i++)
		{
			if (nodes[i]->IsA(StateNodeInstance::RTTI))
			{
				Ptr<StateNodeInstance> snode = nodes[i].cast<StateNodeInstance>();
                Ptr<Materials::SurfaceInstance> material = snode->GetSurfaceInstance();
                if (material->HasConstant(msg->GetName()))
				{
					Ptr<Materials::SurfaceConstant> var;

					// if the type is a string, load it as a texture
                    var = material->GetConstant(msg->GetName());
					if (value.GetType() == Util::Variant::String)
					{
						Ptr<Resources::ManagedTexture> tex = Resources::ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, value.GetString(), NULL, true).downcast<Resources::ManagedTexture>();
                        var->SetTexture(tex->GetTexture());
						tex = 0;
					}
					else
					{
						var->SetValue(value);
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, SetAnimatorTime)
{
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else
	{
		Ptr<AnimatorNodeInstance> nodeInst = RenderUtil::NodeLookupUtil::LookupAnimatorNodeInstance(obj, msg->GetNodeInstanceName().Value());
		if (nodeInst.isvalid())
		{
			nodeInst->OverwriteAnimationTime(msg->GetTime());
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimPlayClip)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());

		// create and enqueue a new play clip job
		Ptr<PlayClipJob> animJob = PlayClipJob::Create();
		animJob->SetClipName(msg->GetClipName());
		animJob->SetTrackIndex(msg->GetTrackIndex());
		animJob->SetLoopCount(msg->GetLoopCount());
		animJob->SetStartTime(msg->GetStartTime());
		animJob->SetFadeInTime(msg->GetFadeInTime());
		animJob->SetFadeOutTime(msg->GetFadeOutTime());
		animJob->SetTimeOffset(msg->GetTimeOffset());
		animJob->SetTimeFactor(msg->GetTimeFactor());
		animJob->SetBlendWeight(msg->GetBlendWeight());
		animJob->SetExclusiveTag(msg->GetExclusiveTag());
		animJob->SetEnqueueMode(msg->GetEnqueueMode());
		obj->GetCharacterInstance()->AnimController().EnqueueAnimJob(animJob.upcast<AnimJob>());
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, FetchClips)
{
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else if (obj->HasCharacter())
	{
		CharacterAnimationLibrary& animLib = obj->GetCharacter()->AnimationLibrary();
		const Ptr<CoreAnimation::AnimResource>& animResource = animLib.GetAnimResource();
		int numClips = animResource->GetNumClips();
		Util::Array<Util::StringAtom> clips;
		Util::Array<SizeT> lengths;
		if (numClips > 0)
		{
			clips.Reserve(numClips);
			for (int clipIndex = 0; clipIndex < numClips; clipIndex++)
			{
				clips.Append(animResource->GetClipByIndex(clipIndex).GetName());
				lengths.Append(animResource->GetClipByIndex(clipIndex).GetClipDuration());
			}
		}		
		msg->SetClips(clips);
		msg->SetLenghts(lengths);
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimStopTrack)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        obj->GetCharacterInstance()->AnimController().StopTrack(msg->GetTrackIndex(), msg->GetAllowFadeOut());
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimStopAllTracks)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        obj->GetCharacterInstance()->AnimController().StopAllTracks(msg->GetAllowFadeOut());
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimPauseTrack)
{
	// character related messages can only be handled once character has loaded
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else
	{
		n_assert(obj->HasCharacter());
		obj->GetCharacterInstance()->AnimController().PauseTrack(msg->GetTrackIndex(), msg->GetPause());
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimPauseAllTracks)
{
	// character related messages can only be handled once character has loaded
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else
	{
		n_assert(obj->HasCharacter());
		obj->GetCharacterInstance()->AnimController().PauseAllTracks(msg->GetPause());
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimSeek)
{
	// character related messages can only be handled once character has loaded
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else
	{
		n_assert(obj->HasCharacter());
		obj->GetCharacterInstance()->AnimController().SetTime(msg->GetTime());
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimTime)
{
	// character related messages can only be handled once character has loaded
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else
	{
		n_assert(obj->HasCharacter());
		msg->SetTime(obj->GetCharacterInstance()->AnimController().GetTime());
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimModifyBlendWeight)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        const AnimSequencer& sequencer = obj->GetCharacterInstance()->AnimController().AnimSequencer();
        IndexT trackIndex = msg->GetTrackIndex();
        float blendWeight = msg->GetBlendWeight();
		if (sequencer.GetAllAnimJobs().Size() > 0)
		{
			Util::Array<Ptr<AnimJob> > animJobs = sequencer.GetAnimJobsByTrackIndex(trackIndex);
			IndexT i;
			for (i = 0; i < animJobs.Size(); ++i)
			{
        		animJobs[i]->SetBlendWeight(blendWeight);
			}
		}
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimModifyJointMask)
{
	// character related messages can only be handled once character has loaded
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else
	{
		n_assert(obj->HasCharacter());
		const AnimSequencer& sequencer = obj->GetCharacterInstance()->AnimController().AnimSequencer();
		IndexT trackIndex = msg->GetTrackIndex();
		const Characters::CharacterJointMask* mask = 0;
		const Characters::CharacterSkeleton& skeleton = obj->GetCharacter()->Skeleton();
		IndexT maskIndex = skeleton.GetMaskIndexByName(msg->GetJointMask());
		if (maskIndex != InvalidIndex) mask = &skeleton.GetMask(maskIndex);
		if (sequencer.GetAllAnimJobs().Size() > 0)
		{
			Util::Array<Ptr<AnimJob> > animJobs = sequencer.GetAnimJobsByTrackIndex(trackIndex);
			
			IndexT i;
			for (i = 0; i < animJobs.Size(); ++i)
			{
				animJobs[i]->SetMask(mask);
			}
		}
	}
}


//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AnimModifyTimeFactor)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        const AnimSequencer& sequencer = obj->GetCharacterInstance()->AnimController().AnimSequencer();
        IndexT trackIndex = msg->GetTrackIndex();
        float timeFactor = msg->GetTimeFactor();
        Util::Array<Ptr<AnimJob> > animJobs = sequencer.GetAnimJobsByTrackIndex(trackIndex);
        IndexT i;
        for (i = 0; i < animJobs.Size(); ++i)
        {
            animJobs[i]->SetTimeFactor(msg->GetTimeFactor());
        }
    }
}

//------------------------------------------------------------------------------
/**
    FIXME: this method doesn't really fit with the Animation System's
    philosophy...
*/
__Handler(ModelEntity, AnimIsClipPlaying)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        const AnimSequencer& sequencer = obj->GetCharacterInstance()->AnimController().AnimSequencer();
        Util::Array<Ptr<AnimJob> > animJobs = sequencer.GetAnimJobsByName(msg->GetClipName());
        msg->SetPlaying(animJobs.Size() > 0);
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, ApplySkinList)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        if (obj->GetCharacter()->SkinLibrary().HasSkinList(msg->GetSkinList()))
        {
            obj->GetCharacterInstance()->SkinSet().ApplySkinList(msg->GetSkinList());
        }
        else
        {
            n_printf("ApplySkinList: invalid skin list '%s' on character '%s'!\n",
                msg->GetSkinList().Value(), obj->GetResourceId().Value());
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, FetchSkinList)
{
	if (!obj->IsValid())
	{
		obj->AddDeferredMessage(msg.cast<Message>());
	}
	else if (obj->HasCharacter())
	{
		Characters::CharacterSkinLibrary& skinLib = obj->GetCharacter()->SkinLibrary();
		int skinCount = skinLib.GetNumSkins();
		Util::Array<Util::StringAtom> skinList;
		skinList.Reserve(skinCount);
		for (int skinIndex = 0; skinIndex < skinCount; skinIndex++)
		{
			skinList.Append(skinLib.GetSkin(skinIndex).GetName());
		}
		msg->SetSkins(skinList);
	}
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, ShowSkin)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        obj->GetCharacterInstance()->SkinSet().AddSkin(msg->GetSkin());
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, HideSkin)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        obj->GetCharacterInstance()->SkinSet().RemoveSkin(msg->GetSkin());
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, SetVariation)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        obj->GetCharacterInstance()->SetVariationSetName(msg->GetVariationName());
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, AddTrackedCharJoint)
{
    // character related messages can only be handled once character has loaded
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        n_assert(obj->HasCharacter());
        obj->AddTrackedCharJoint(msg->GetJointName());
    }
}

//------------------------------------------------------------------------------
/**
    Special case, AnimEventMessages must be forwarded to the 
    AnimEventServer, but only when the ModelEntity has loaded
    its resources (the character must be valid).
*/
__Handler(ModelEntity, BaseAnimEventMessage)
{
    if (!obj->IsValid())
    {
        obj->AddDeferredMessage(msg.cast<Message>());
    }
    else
    {
        Animation::AnimEventServer::Instance()->HandleMessage(msg.cast<Message>());
    }
}

//------------------------------------------------------------------------------
/**
*/
__Handler(ModelEntity, SetParticleSystemPlaying)
{
	const Ptr<Models::ModelInstance>& instance = obj->GetModelInstance();

	if(!instance.isvalid())
		return;

	Ptr<ModelNodeInstance> node = instance->LookupNodeInstance(msg->GetNode());
	if(node.isvalid() && node->IsA(Particles::ParticleSystemNodeInstance::RTTI))
	{
		Ptr<Particles::ParticleSystemNodeInstance> pinstance = node.cast<Particles::ParticleSystemNodeInstance>();
		bool playing = msg->GetPlaying();
		const Ptr<Particles::ParticleSystemInstance>& psystem = pinstance->GetParticleSystemInstance();
		if (playing && !psystem->IsPlaying())
		{
			psystem->Start();
		}
		else if (!playing && psystem->IsPlaying())
		{
			psystem->Stop();
		}
	}	
}

//------------------------------------------------------------------------------
/**
    Dispatcher method (must be positioned after the handler methods to
    prevent automatic instantiation).
*/
__Dispatcher(ModelEntity)
{
    __HandleByRTTI(ModelEntity, BaseAnimEventMessage);
    __Handle(ModelEntity, SetVisibility);
	__Handle(ModelEntity, SetRenderSkeleton);
    __Handle(ModelEntity, UpdModelNodeInstanceVisibility);
    __Handle(ModelEntity, UpdModelNodeInstanceSurfaceConstant);
	__Handle(ModelEntity, UpdMaterialVariable);
    __Handle(ModelEntity, AnimPlayClip);
	__Handle(ModelEntity, FetchClips);
    __Handle(ModelEntity, AnimStopTrack);
    __Handle(ModelEntity, AnimStopAllTracks);
	__Handle(ModelEntity, AnimPauseTrack);
	__Handle(ModelEntity, AnimPauseAllTracks);
	__Handle(ModelEntity, AnimSeek);
	__Handle(ModelEntity, AnimTime);
    __Handle(ModelEntity, AnimIsClipPlaying);
    __Handle(ModelEntity, AnimModifyBlendWeight);
	__Handle(ModelEntity, AnimModifyJointMask);
    __Handle(ModelEntity, AnimModifyTimeFactor);
    __Handle(ModelEntity, ApplySkinList);
	__Handle(ModelEntity, FetchSkinList);
    __Handle(ModelEntity, ShowSkin);
    __Handle(ModelEntity, HideSkin);
    __Handle(ModelEntity, SetAnimatorTime);
    __Handle(ModelEntity, AddTrackedCharJoint);
    __Handle(ModelEntity, SetVariation);
	__Handle(ModelEntity, SetParticleSystemPlaying);
    __HandleUnknown(GraphicsEntity);
}

} // namespace Messaging
