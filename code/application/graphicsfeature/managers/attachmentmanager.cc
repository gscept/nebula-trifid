//------------------------------------------------------------------------------
//  attachmentmanager.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicsfeature/managers/attachmentmanager.h"
#include "game/entity.h"
#include "basegamefeature/managers/entitymanager.h"
#include "graphics/modelentity.h"
#include "characters/character.h"
#include "basegamefeature/basegameprotocol.h"
#include "characters/base/skinnedmeshrendererbase.h"

namespace GraphicsFeature
{
__ImplementClass(AttachmentManager, 'ATMA', Game::Manager);
__ImplementSingleton(AttachmentManager);

using namespace Util;
using namespace Math;

//------------------------------------------------------------------------------
/**
*/
AttachmentManager::AttachmentManager()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
AttachmentManager::~AttachmentManager()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::OnBeginFrame()
{
	// clear attachments connects to removed entities
	this->ClearInvalid();
	// check for pending
	if (this->delayedAttachments.Size())
	{
		this->AttachPending();
	}
	
    // update attachments
	for (Array<Attachment>::Iterator iter = this->attachments.Begin(); iter != this->attachments.End();)
	{
		const Attachment & attach = *iter;

		Math::matrix44 newTransform = this->CalculateTransform(attach);
		switch (attach.type)
		{
			case GraphicsToGraphics:
			case GraphicsToJoint:
				attach.attachedEntity.cast<Graphics::GraphicsEntity>()->SetTransform(newTransform);
				attach.attachedEntity.cast<Graphics::GraphicsEntity>()->SetVisible(attach.baseEntity->IsVisible());
			break;
			case GameToGraphics:
			case GameToJoint:
				{
					Ptr<BaseGameFeature::SetTransform> msg = BaseGameFeature::SetTransform::Create();
					msg->SetMatrix(newTransform);
					__SendSync(attach.attachedEntity.cast<Game::Entity>(), msg);
				}

		}
		iter++;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::Attach(const Ptr<Graphics::GraphicsEntity>& baseEntity, const Ptr<Graphics::GraphicsEntity>& entityToAttach, const Math::matrix44& offset, AttachmentRotation rotation)
{
	Attachment attach;
	attach.type = GraphicsToGraphics;
	attach.baseEntity = baseEntity;
	attach.attachedEntity = entityToAttach.cast<Core::RefCounted>();
	attach.offset = offset;	
	attach.rotation = rotation;
	attach.jointIndex = InvalidIndex;
	this->attachments.Append(attach);
	entityToAttach->SetVisible(baseEntity->IsVisible());
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::Attach(const Ptr<Graphics::ModelEntity>& baseEntity, const Util::StringAtom& joint, const Ptr<Graphics::GraphicsEntity>& entityToAttach, const Math::matrix44& offset, AttachmentRotation rotation)
{	
	Attachment attach;
	attach.type = GraphicsToJoint;
	attach.baseEntity = baseEntity;
	attach.attachedEntity = entityToAttach.cast<Core::RefCounted>();
	attach.offset = offset;	
	attach.rotation = rotation;
	attach.joint = joint;
	attach.jointIndex = -1;
	if (baseEntity->GetModelResourceState() != Resources::Resource::Loaded)
	{
		this->delayedAttachments.Append(attach);
	}
	else
	{
		n_assert2(baseEntity->HasCharacter(), "Not a character");
		attach.jointIndex = baseEntity->GetCharacter()->Skeleton().GetJointIndexByName(joint);
		n_assert2(attach.jointIndex != InvalidIndex, "Invalid joint name");
		this->attachments.Append(attach);
		entityToAttach->SetVisible(baseEntity->IsVisible());
	}	
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::Attach(const Ptr<Graphics::GraphicsEntity>& baseEntity, const Ptr<Game::Entity>& entityToAttach, const Math::matrix44& offset, AttachmentRotation rotation)
{
	Attachment attach;
	attach.type = GameToGraphics;
	attach.baseEntity = baseEntity;
	attach.attachedEntity = entityToAttach.cast<Core::RefCounted>();
	attach.offset = offset;	
	attach.rotation = rotation;
	attach.jointIndex = InvalidIndex;
	this->attachments.Append(attach);
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::Attach(const Ptr<Graphics::ModelEntity>& baseEntity, const Util::StringAtom& joint, const Ptr<Game::Entity>& entityToAttach, const Math::matrix44& offset, AttachmentRotation rotation)
{
	Attachment attach;
	attach.type = GameToJoint;
	attach.baseEntity = baseEntity;
	attach.attachedEntity = entityToAttach.cast<Core::RefCounted>();
	attach.offset = offset;	
	attach.rotation = rotation;
	attach.joint = joint;
	attach.jointIndex = -1; 
	if (baseEntity->GetModelResourceState() != Resources::Resource::Loaded)
	{
		this->delayedAttachments.Append(attach);
	}
	else
	{
		n_assert2(baseEntity->HasCharacter(), "Not a character");
		attach.jointIndex = baseEntity->GetCharacter()->Skeleton().GetJointIndexByName(joint);
		n_assert2(attach.jointIndex != InvalidIndex, "Invalid joint name");
		this->attachments.Append(attach);
	}		
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::Detach(const Ptr<Graphics::GraphicsEntity>& entityToDetach)
{
	IndexT id = entityToDetach->GetId();
	for (Array<Attachment>::Iterator iter = this->attachments.Begin(); iter != this->attachments.End();)
	{
		if (id == iter->baseEntity.cast<Graphics::GraphicsEntity>()->GetId())
		{						
			this->attachments.Erase(iter);
			return;
		}
		else
		{
			iter++;
		}
	}
	// nothing was removed, check pending attachments
	for (Array<Attachment>::Iterator iter = this->delayedAttachments.Begin(); iter != this->delayedAttachments.End();)
	{
		if (id == iter->baseEntity.cast<Graphics::GraphicsEntity>()->GetId())
		{
			this->attachments.Erase(iter);
			return;
		}
		else
		{
			iter++;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::Detach(const Ptr<Game::Entity>& entityToDetach)
{
	IndexT id = entityToDetach->GetUniqueId();
	for (Array<Attachment>::Iterator iter = this->attachments.Begin(); iter != this->attachments.End();)
	{
		if (id == iter->baseEntity.cast<Game::Entity>()->GetUniqueId())
		{			
			this->attachments.Erase(iter);
			return;
		}
		else
		{
			iter++;
		}
	}
	// nothing was removed, check pending attachments
	for (Array<Attachment>::Iterator iter = this->delayedAttachments.Begin(); iter != this->delayedAttachments.End();)
	{
		if (id == iter->baseEntity.cast<Game::Entity>()->GetUniqueId())
		{
			this->delayedAttachments.Erase(iter);
			return;
		}
		else
		{
			iter++;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::ClearAttachmentsOnEntity(const Ptr<Graphics::GraphicsEntity>& baseEntity)
{
	for (Array<Attachment>::Iterator iter = this->attachments.Begin(); iter != this->attachments.End();)
	{		
		if (baseEntity == iter->baseEntity)
		{
			if (iter->type == GraphicsToGraphics || iter->type == GraphicsToJoint)
			{
				iter->attachedEntity.cast<Graphics::GraphicsEntity>()->MarkRemove();
			}
			this->attachments.Erase(iter);
		}
		else
		{
			iter++;
		}
	}
	for (Array<Attachment>::Iterator iter = this->delayedAttachments.Begin(); iter != this->delayedAttachments.End();)
	{
		if (baseEntity == iter->baseEntity)
		{
			this->delayedAttachments.Erase(iter);			
		}
		else
		{
			iter++;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::ClearAttachmentsOnJoint(const Util::StringAtom& joint, const Ptr<Graphics::GraphicsEntity>& baseEntity)
{
	n_assert(baseEntity.cast<Graphics::ModelEntity>()->HasCharacter());
	IndexT jointIndex = baseEntity.cast<Graphics::ModelEntity>()->GetCharacter()->Skeleton().GetJointIndexByName(joint);
	for (Array<Attachment>::Iterator iter = this->attachments.Begin(); iter != this->attachments.End();)
	{
		if (baseEntity == iter->baseEntity && iter->jointIndex == jointIndex)
		{
			if (iter->type == GraphicsToJoint)
			{
				iter->attachedEntity.cast<Graphics::GraphicsEntity>()->MarkRemove();
			}
			this->attachments.Erase(iter);
		}
		else
		{
			iter++;
		}
	}	
	for (Array<Attachment>::Iterator iter = this->delayedAttachments.Begin(); iter != this->delayedAttachments.End();)
	{
		if (baseEntity == iter->baseEntity && iter->jointIndex == jointIndex)
		{
			if (iter->type == GraphicsToJoint)
			{
				iter->attachedEntity.cast<Graphics::GraphicsEntity>()->MarkRemove();
			}
			this->delayedAttachments.Erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
Math::matrix44
AttachmentManager::CalculateTransform(const Attachment& attach)
{
	// get base transform of graphics entity
	Math::matrix44 baseTransform = attach.baseEntity->GetTransform();
	Math::matrix44 offset = attach.offset;
	switch (attach.rotation)
	{
	case TransformOnly:
		offset.translate(Math::vector(baseTransform.get_position()));
		return offset;
		break;
	case JointTranslationOnly:
	{
		const Ptr<Characters::CharacterInstance>& charInst = attach.baseEntity.cast<Graphics::ModelEntity>()->GetCharacterInstance();
		charInst->WaitUpdateDone();
		Math::matrix44 jointMatrix = charInst->Skeleton().GetJointMatrix(attach.jointIndex);
		offset = matrix44::multiply(offset, baseTransform);
		offset.translate(Math::vector(jointMatrix.get_position()));
		return offset;
	}
	break;
	case Joint:
	{
		const Ptr<Characters::CharacterInstance>& charInst = attach.baseEntity.cast<Graphics::ModelEntity>()->GetCharacterInstance();
		charInst->WaitUpdateDone();
		Math::matrix44 jointMatrix = charInst->Skeleton().GetJointMatrix(attach.jointIndex);

		offset = matrix44::multiply(offset, jointMatrix);
		offset = matrix44::multiply(offset, baseTransform);		
		return offset;
	}
	break;
	}	
	return baseTransform;
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::AttachPending()
{
	for (Array<Attachment>::Iterator iter = this->delayedAttachments.Begin();iter != this->delayedAttachments.End();)	
	{
		const Ptr<Graphics::ModelEntity> & base = iter->baseEntity.cast<Graphics::ModelEntity>();
		if (base->GetModelResourceState() == Resources::Resource::Loaded && base->HasCharacter())
		{			
			Attachment attach = *iter;						
			attach.jointIndex = base->GetCharacter()->Skeleton().GetJointIndexByName(attach.joint);
			n_assert2(attach.jointIndex != InvalidIndex, "Invalid joint name");
			this->attachments.Append(attach);			
			iter = this->delayedAttachments.Erase(iter);
		}
		else if (base->IsMarkedForRemove() || !base->IsAttachedToStage())
		{
			// we have been removed before we managed to attach
			if (iter->type == GraphicsToJoint)
			{
				iter->attachedEntity.cast<Graphics::GraphicsEntity>()->MarkRemove();
			}
			this->delayedAttachments.Erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::ClearInvalid()
{

	for (Array<Attachment>::Iterator iter = this->attachments.Begin(); iter != this->attachments.End();)
	{
		if (!iter->baseEntity->IsActive() || iter->baseEntity->IsMarkedForRemove())
		{
			if (iter->type == GraphicsToJoint || iter->type == GraphicsToGraphics)
			{
				iter->attachedEntity.cast<Graphics::GraphicsEntity>()->MarkRemove();
			}
			this->attachments.Erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

}