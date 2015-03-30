//------------------------------------------------------------------------------
//  attachmentmanager.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicsfeature/managers/attachmentmanager.h"
#include "graphics/graphicsprotocol.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "graphics/graphicsinterface.h"
#include "game/entity.h"
#include "basegamefeature/managers/entitymanager.h"
#include "basegamefeature/basegameattr/basegameattributes.h"
#include "basegamefeature/basegametiming/gametimesource.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"

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
AttachmentManager::OnFrame()
{
    // check temporary attachments
    this->CheckTemporaryAttachments();
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::AddGraphicsAttachment( const Util::StringAtom& jointName,
                                          const Ptr<Game::Entity>& baseEntityPtr,
                                          const Util::StringAtom& entityToAttachResId,
                                          const Math::matrix44& offset,
                                          bool keepLocal,
                                          Graphics::AttachmentServer::AttachmentRotation rotation)
{
    this->AddGraphicsAttachmentTemporary( jointName,
                                          baseEntityPtr,
                                          entityToAttachResId,
                                          offset,
                                          keepLocal,
                                          rotation,
                                          -1);
}


//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::AddGraphicsAttachmentTemporary( const Util::StringAtom& jointName, 
                                                  const Ptr<Game::Entity>& baseEntityPtr, 
                                                  const Util::StringAtom& entityToAttachResId, 
                                                  const Math::matrix44& offset, 
                                                  bool keepLocal, 
                                                  Graphics::AttachmentServer::AttachmentRotation rotation,
                                                  Timing::Time duration)
{
    // first attach as pending
    Attachment newAttachment;
    newAttachment.joint = jointName;
    newAttachment.offset = offset;
    newAttachment.keepLocal = keepLocal;
    newAttachment.rotation = rotation;
    newAttachment.duration = duration;
	newAttachment.startTime = BaseGameFeature::GameTimeSource::Instance()->GetTime();

    Ptr<GetModelEntity> getGfx1 = GetModelEntity::Create();
    baseEntityPtr->SendSync(getGfx1.cast<Messaging::Message>());
    newAttachment.baseEntity = getGfx1->GetEntity();

    // transform
    Math::matrix44 t = Math::matrix44::multiply(newAttachment.baseEntity->GetTransform(), offset);

    // create attachment model entity
    Ptr<Graphics::ModelEntity> attachedEntity = Graphics::ModelEntity::Create();
    attachedEntity->SetResourceId(entityToAttachResId);
    attachedEntity->SetTransform(t);
    attachedEntity->SetVisible(true);
    const Ptr<Graphics::Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
    stage->AttachEntity(attachedEntity.cast<Graphics::GraphicsEntity>());
    newAttachment.attachedEntity = attachedEntity;
    
    this->attachments.Append(newAttachment);

    // setup and send BindAttachment message
    this->SendAttachmentMessage(newAttachment);
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::AddGraphicsAttachment( const Util::StringAtom& jointName, 
											const Ptr<Game::Entity>& baseEntityPtr,
											const Ptr<Graphics::GraphicsEntity>& entityToAttach, 
											const Math::matrix44& offset,
											bool keepLocal,
											Graphics::AttachmentServer::AttachmentRotation rotation)
{        
	
	this->AddGraphicsAttachmentTemporary( jointName, 
		baseEntityPtr, 
		entityToAttach, 
		offset, 
		keepLocal, 
		rotation, 
		-1);
}
//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::AddGraphicsAttachmentTemporary( const Util::StringAtom& jointName, 
													const Ptr<Game::Entity>& baseEntityPtr, 
													const Ptr<Graphics::GraphicsEntity>& entityToAttach,
													const Math::matrix44& offset, 
													bool keepLocal, 
													Graphics::AttachmentServer::AttachmentRotation rotation,
													Timing::Time duration)
{
	// first attach as pending
	Attachment newAttachment;
	newAttachment.joint = jointName;
	newAttachment.offset = offset;
	newAttachment.keepLocal = keepLocal;
	newAttachment.rotation = rotation;
	newAttachment.duration = duration;
	newAttachment.startTime = BaseGameFeature::GameTimeSource::Instance()->GetTime();

	Ptr<GetModelEntity> getGfx1 = GetModelEntity::Create();
	baseEntityPtr->SendSync(getGfx1.cast<Messaging::Message>());
	newAttachment.baseEntity = getGfx1->GetEntity();

	// transform
	Math::matrix44 t = Math::matrix44::multiply(newAttachment.baseEntity->GetTransform(), offset);

	entityToAttach->SetTransform(t);
	entityToAttach->SetVisible(true);
	const Ptr<Graphics::Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
	stage->AttachEntity(entityToAttach.cast<Graphics::GraphicsEntity>());
	newAttachment.attachedEntity = entityToAttach;

	this->attachments.Append(newAttachment);

	// setup and send BindAttachment message
	this->SendAttachmentMessage(newAttachment);
}


//------------------------------------------------------------------------------
/**/

void AttachmentManager::AddLightAttachmentTemporary( const Util::StringAtom& jointName,
						const Ptr<Game::Entity>& baseEntityPtr,
						const Ptr<Graphics::AbstractLightEntity>& light,
						const Math::matrix44& offset,
						bool keepLocal,
						Graphics::AttachmentServer::AttachmentRotation rotation,
						Timing::Time duration)
{
	// first attach as pending
	Attachment newAttachment;
	newAttachment.joint = jointName;
	newAttachment.offset = offset;
	newAttachment.keepLocal = keepLocal;
	newAttachment.rotation = rotation;
	newAttachment.duration = duration;
	newAttachment.startTime = BaseGameFeature::GameTimeSource::Instance()->GetTime();

	Ptr<GetModelEntity> getGfx1 = GetModelEntity::Create();
	baseEntityPtr->SendSync(getGfx1.cast<Messaging::Message>());
	newAttachment.baseEntity = getGfx1->GetEntity();

	// transform
	Math::matrix44 t = Math::matrix44::multiply(newAttachment.baseEntity->GetTransform(), offset);

	// create attachment model entity
	light->SetTransform(t);
	//light->SetColor(Math::float4(0.0f, 0.0f, 0.0f, 1.0f));
	light->SetVisible(true);
	light->SetCastShadows(false);
	//light->SetLightType(Lighting::LightType::Point);
	const Ptr<Graphics::Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();
	stage->AttachEntity(light.cast<Graphics::GraphicsEntity>());
	newAttachment.attachedEntity = light;

	this->attachments.Append(newAttachment);

	// setup and send BindAttachment message
	this->SendAttachmentMessage(newAttachment);
}

//------------------------------------------------------------------------------
/**
*/
void AttachmentManager::AddLightAttachment( const Util::StringAtom& jointName,
						const Ptr<Game::Entity>& baseEntityPtr,
						const Ptr<Graphics::AbstractLightEntity>& light,
						const Math::matrix44& offset,
						bool keepLocal,
						Graphics::AttachmentServer::AttachmentRotation rotation)
{        
	// find base entity
	this->AddLightAttachmentTemporary( jointName,
		baseEntityPtr,
		light,
		offset,
		keepLocal,
		rotation,
		-1);
}

//------------------------------------------------------------------------------
/**
*/

void
AttachmentManager::CheckTemporaryAttachments()
{
    // get time
    Timing::Time time = BaseGameFeature::GameTimeSource::Instance()->GetTime();

    // check all
    IndexT index;
    for (index = 0; index < this->attachments.Size(); index++)
    {
        const Attachment& attachment = this->attachments[index];

        // skip if endless
        if (-1 == attachment.duration)
        {
            continue;
        }

        // if time ran out
        if (attachment.startTime + attachment.duration < time)
        {
            // remove that shit
            this->SendDetachmentMessage(attachment);

            // check if remove from stage
            GraphicsFeatureUnit::Instance()->GetDefaultStage()->RemoveEntity(attachment.attachedEntity);
            
            // remove from array
            this->attachments.EraseIndex(index);
            index--;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::SendAttachmentMessage(const Attachment& attachment)
{
    Ptr<Graphics::BindAttachment> msg = Graphics::BindAttachment::Create();
    msg->SetJoint(attachment.joint.Value());
    msg->SetOffset(attachment.offset);
    msg->SetEntityToAttach(attachment.attachedEntity);
    msg->SetClearType(Graphics::AttachmentServer::ClearAllIdenticalOnJoint);
    msg->SetKeepLocal(attachment.keepLocal);
    msg->SetRotation(attachment.rotation);
	attachment.baseEntity->HandleMessage(msg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentManager::SendDetachmentMessage(const Attachment &detachment)
{
    Ptr<Graphics::UnbindAttachment> msg = Graphics::UnbindAttachment::Create();
    msg->SetEntityToAttach(detachment.attachedEntity);
    msg->SetJoint(detachment.joint.Value()); 
    msg->SetClearType(Graphics::AttachmentServer::ClearAllIdenticalOnJoint);
	detachment.baseEntity->HandleMessage(msg.cast<Messaging::Message>());
}

//------------------------------------------------------------------------------
/**
*/
void 
AttachmentManager::ClearAttachments()
{
    // clean up
    const Ptr<Graphics::Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage(); 
    IndexT i;
    for (i = 0; i < this->attachments.Size(); ++i)
    {
        if (this->attachments[i].attachedEntity.isvalid())
        {
            stage->RemoveEntity(this->attachments[i].attachedEntity.cast<Graphics::GraphicsEntity>());    	
        }        
    }
    this->attachments.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void 
AttachmentManager::ClearAttachmentsOnEntity(const Ptr<Game::Entity>& baseEntity)
{
    // get basegfxentity
    Ptr<GetModelEntity> getGfx1 = GetModelEntity::Create();
    baseEntity->SendSync(getGfx1.cast<Messaging::Message>());
    const Ptr<Graphics::GraphicsEntity>& baseGfxEntity = getGfx1->GetEntity().cast<Graphics::GraphicsEntity>();
    n_assert(baseGfxEntity.isvalid());

    // clean up
    const Ptr<Graphics::Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();    
    IndexT i;
    for (i = 0; i < this->attachments.Size(); ++i)
    {
        if (this->attachments[i].attachedEntity.isvalid()
            && this->attachments[i].baseEntity == baseGfxEntity)
        {
            this->SendDetachmentMessage(this->attachments[i]);

            stage->RemoveEntity(this->attachments[i].attachedEntity.cast<Graphics::GraphicsEntity>());    	
            this->attachments.EraseIndex(i);
            --i;
        }        
    }    
}


/// Clear Joint
void 
AttachmentManager::ClearAttachmentsOnJoint(const Util::StringAtom& joint,const Ptr<Game::Entity>& baseEntity)
{
	// get basegfxentity
	Ptr<GetModelEntity> getGfx1 = GetModelEntity::Create();
	baseEntity->SendSync(getGfx1.cast<Messaging::Message>());
	const Ptr<Graphics::GraphicsEntity>& baseGfxEntity = getGfx1->GetEntity().cast<Graphics::GraphicsEntity>();
	n_assert(baseGfxEntity.isvalid());

	// clean up
	const Ptr<Graphics::Stage>& stage = GraphicsFeatureUnit::Instance()->GetDefaultStage();    
	IndexT i;
	for (i = 0; i < this->attachments.Size(); ++i)
	{
		if (this->attachments[i].attachedEntity.isvalid()
			&& this->attachments[i].baseEntity == baseGfxEntity
			&& this->attachments[i].joint == joint)
		{
			this->SendDetachmentMessage(this->attachments[i]);

			stage->RemoveEntity(this->attachments[i].attachedEntity.cast<Graphics::GraphicsEntity>());    	
			this->attachments.EraseIndex(i);
			--i;
		}        
	} 
}
//------------------------------------------------------------------------------
/**
*/
Ptr<Graphics::GraphicsEntity>
AttachmentManager::GetAttachmentGfxEntity(const Ptr<Graphics::GraphicsEntity>& gfxBaseEntity, const Util::StringAtom& jointName) const
{    
    IndexT i;
    for (i = 0; i < this->attachments.Size(); ++i)
    {
        if (this->attachments[i].attachedEntity.isvalid()
            && this->attachments[i].baseEntity == gfxBaseEntity
            && this->attachments[i].joint == jointName)
        {
            return this->attachments[i].attachedEntity;
        }        
    }

    return NULL;
}

} // namespace GraphicsFeature
