//------------------------------------------------------------------------------
//  attachmentserver.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/attachmentserver.h"
#include "graphics/graphicsprotocol.h"
#include "graphics/graphicsentity.h"
#include "graphics/modelentity.h"
#include "graphics/graphicsserver.h"
#include "graphics/view.h"
#include "characters/character.h"
#include "io/excelxmlreader.h"
#include "io/ioserver.h"
#include "animation/animeventinfo.h"

namespace Graphics
{
__ImplementClass(Graphics::AttachmentServer, 'ATSV', Core::RefCounted);
__ImplementSingleton(Graphics::AttachmentServer);

using namespace Util;
using namespace Math;
using namespace Characters;
using namespace Animation;

//------------------------------------------------------------------------------
/**
*/
AttachmentServer::AttachmentServer() : isOpen(false)
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
AttachmentServer::~AttachmentServer()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
bool
AttachmentServer::Open()
{
    n_assert(!this->IsOpen());    
    this->isOpen = true;
    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentServer::Close()
{
    n_assert(this->IsOpen());
    // clean up
    this->attachments.Clear();

    this->isOpen = false;
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentServer::AttachEntityTemporary(const Math::matrix44& offset, 
                                        const ClearType& clearType, 
                                        const Util::StringAtom& joint, 
                                        const Ptr<GraphicsEntity>& entityToAttach,
                                        const Ptr<ModelEntity>& baseEntity, 
                                        bool keepLocal,
                                        Timing::Time duration,
                                        AttachmentServer::AttachmentRotation rotation,
                                        bool attachedFromOtherThread)
{
    // first detach 
    this->DetachEntity(clearType, joint, entityToAttach, baseEntity);

    // create new
    Attachment attachment;
    attachment.attachedEntity = entityToAttach;
    n_assert(baseEntity->GetType() == GraphicsEntityType::Model);
    attachment.baseEntity = baseEntity;
    attachment.jointIndex = attachment.baseEntity->GetCharacter()->Skeleton().GetJointIndexByName(joint);
    attachment.offset = offset;
    attachment.startTime = this->time;
    attachment.duration = duration;
    attachment.keepLocal = keepLocal;
    attachment.initPosSet = false;
    attachment.rotation = rotation;
    attachment.attachedFromOtherThread = attachedFromOtherThread;

    // and attach
    this->attachments.Append(attachment);
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentServer::AttachEntity(const Math::matrix44& offset,
                               const ClearType& clearType,
                               const Util::StringAtom& joint,
                               const Ptr<GraphicsEntity>& entityToAttach,
                               const Ptr<ModelEntity>& baseEntity,
                               bool keepLocal,
                               AttachmentServer::AttachmentRotation rotation,
                               bool attachedFromOtherThread)
{
    this->AttachEntityTemporary(offset,
                                clearType,
                                joint,
                                entityToAttach,
                                baseEntity,
                                keepLocal,
                                -1,
                                rotation,
                                attachedFromOtherThread);
}

//------------------------------------------------------------------------------
/**
*/
void 
AttachmentServer::SwitchEntity(const Util::StringAtom& oldJoint, const Util::StringAtom& newJoint, const Ptr<ModelEntity>&  baseEntity, const Ptr<GraphicsEntity>& entityToAttach)
{    
    // get attachment
    IndexT i;
    for (i = 0; i < this->attachments.Size(); ++i)
    {
        if (baseEntity == this->attachments[i].baseEntity)
        {
            IndexT oldIndex = this->attachments[i].baseEntity->GetCharacter()->Skeleton().GetJointIndexByName(oldJoint);
            if (this->attachments[i].attachedEntity == entityToAttach
                && this->attachments[i].jointIndex == oldIndex)
            {
                this->attachments[i].jointIndex = this->attachments[i].baseEntity->GetCharacter()->Skeleton().GetJointIndexByName(newJoint);
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentServer::DetachEntity(const ClearType& clearType,
                               const Util::StringAtom& joint,
                               const Ptr<GraphicsEntity>& entityToAttach,
                               const Ptr<ModelEntity>& baseEntity)
{
    // TODO.. maybe optimize this method, there are 4 loops lookin' very redundant...
    //        thought about pulling' the switch case into one big loop... but i think
    //        this way its a little bit faster.... what do you think?!
    IndexT index;
    switch (clearType)
    {
    case ClearAll:
        // remove from stage...
        for (index = 0; index < this->attachments.Size(); index++)
        {
            this->attachments[index].attachedEntity->MarkRemove();
        }
        this->attachments.Clear();
        break;

    case ClearAllIdenticalOnEntity:
        for (index = 0; index < this->attachments.Size(); index++)
        {
            if (this->attachments[index].baseEntity == baseEntity)
            {
                if (this->attachments[index].attachedEntity == entityToAttach)
                {
                    // remove from stage...
                    this->attachments[index].attachedEntity->MarkRemove();

                    // ... and from this server
                    this->attachments.EraseIndex(index);
                    index--;                    
                }
            }
        }
        break;

    case ClearAllOnEntity:
        for(index = 0; index < this->attachments.Size(); index++)
        {
            if(this->attachments[index].baseEntity == baseEntity)
            {
                // remove from stage...
                this->attachments[index].attachedEntity->MarkRemove();

                // ... and from this server
                this->attachments.EraseIndex(index);
                index--;
            }
        }
        break;

    case ClearAllIdenticalOnJoint:
        for(index = 0; index < this->attachments.Size(); index++)
        {
            if (this->attachments[index].baseEntity == baseEntity)
            {
                int jointIndex = this->attachments[index].baseEntity->GetCharacter()->Skeleton().GetJointIndexByName(joint);
                if (this->attachments[index].jointIndex == jointIndex && 
                   this->attachments[index].attachedEntity == entityToAttach)
                {
                    // remove from stage...
                    this->attachments[index].attachedEntity->MarkRemove();

                    // ... and from this server
                    this->attachments.EraseIndex(index);
                    index--;
                }
            }
        }
        break;

    case ClearAllOnJoint:
        for(index = 0; index < this->attachments.Size(); index++)
        {
            if(this->attachments[index].baseEntity == baseEntity)
            {
                int jointIndex = this->attachments[index].baseEntity->GetCharacter()->Skeleton().GetJointIndexByName(joint);
                if(this->attachments[index].jointIndex == jointIndex)
                {
                    // remove from stage...
                    this->attachments[index].attachedEntity->MarkRemove();

                    // ... and from this server
                    this->attachments.EraseIndex(index);
                    index--;
                }
            }
        }
        break;

    case ClearNone:
    default:
        break;
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
AttachmentServer::DetachEntity(const Ptr<Graphics::GraphicsEntity>& entity)
{
    // search
    IndexT index;
    for (index = 0; index < this->attachments.Size(); index++)
    {
        if (entity == this->attachments[index].attachedEntity)
        {
            // remove from stage...                    
            this->attachments[index].attachedEntity->MarkRemove();

            // ... and from this server
            this->attachments.EraseIndex(index);
            return;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
AttachmentServer::RemoveInvalidAttachments()
{
    // do for all
    IndexT index;
    for (index = 0; index < this->attachments.Size(); index++)
    {
        // get current attachment
        Attachment& attachment = this->attachments[index];
                      
        // check if any resource isn't valid anymore, so any temporary attachment must be also removed
        // but do not check if attachment comes from other thread, in this case other thread is responsible for detaching it correctly
        bool removeAttachment = false;
        if (!attachment.attachedFromOtherThread)
        {
            removeAttachment = !attachment.baseEntity->HasCharacter();
        }
        
        // check duration
        if (-1 != attachment.duration)
        {   
            // check if temporary attachment has expired
            if (attachment.startTime + attachment.duration < this->time)
            {     
                // effect has been ended, remove from server
                removeAttachment = true;
            }
        }
        if (removeAttachment)
        {    
            // character not valid remove attachment
            this->DetachEntity(attachment.attachedEntity);
            index--;
        }
    }
}

//------------------------------------------------------------------------------
/**
    FIXME: this method depends on valid character skeletons, thus
    make sure that it is called after character skeletons have been
    updated in the character server!
*/
void
AttachmentServer::OnFrame(Timing::Time time)
{
    // first set time
    this->time = time;

    // remove invalid attachments
    this->RemoveInvalidAttachments();

    // a correctional matrix which rotates the joint matrix by 180
    // degree around global Y to account for the fact the Nebula2
    // characters look along +Z, not -Z
    // matrix44 rot180 = matrix44::rotationy(n_deg2rad(180.0f));

    // do for all
    IndexT index;
    for(index = 0; index < this->attachments.Size(); index++)
    {
        // get current attachment
        Attachment& attachment = this->attachments[index]; 
        // do not update attachments of invalid characters, but do not remove if it was attached from mainthread
        // mainthread is responsible for removing its attachments
        bool baseEntityIsValid = attachment.baseEntity->HasCharacter();
        if (baseEntityIsValid && attachment.attachedEntity->IsValid() && attachment.attachedEntity->IsActive())
        {
            if (attachment.keepLocal || !attachment.initPosSet)
            {            
                // get world matrix of char entity
                const matrix44& worldMatrix = attachment.baseEntity->GetTransform();

                // get joint, but we need to make sure that the skeleton is uptodate!
                // FIXME: the WaitUpdateDone() is EVIL!
                const Ptr<CharacterInstance>& charInst = attachment.baseEntity->GetCharacterInstance();
                charInst->WaitUpdateDone();
                const matrix44& jointMatrix = charInst->Skeleton().GetJointMatrix(attachment.jointIndex);

                // calc and rotation and transformation depending on settings
                matrix44 transformation = attachment.offset;
                switch (attachment.rotation)
                {
                case Local:
                    transformation = matrix44::multiply(transformation, jointMatrix);                    
                    // transformation = matrix44::multiply(transformation, rot180);
                    transformation = matrix44::multiply(transformation, worldMatrix);
                    break;                
                case Entity:
                    // transformation = matrix44::multiply(transformation, rot180);
                    transformation = matrix44::multiply(transformation, worldMatrix);
                    transformation.translate(vector(jointMatrix.get_position()));
                    break;
                case World:
                    transformation.translate(vector(worldMatrix.get_position()));
                    break;
                default:
                    n_error("AttachmentServer::OnFrame: unsupported attachment rotation used!\n");
                    break;
                }                
                attachment.attachedEntity->SetTransform(transformation);

                if (!attachment.initPosSet)
                {
                    attachment.initPosSet = true;
                    attachment.attachedEntity->SetVisible(true);
                }                
            }

            //CoreGraphics::Shape shape;hotSpotAttachments
            //shape.SetupSimpleShape(Threading::Thread::GetMyThreadId(),
            //    CoreGraphics::Shape::Sphere,
            //    baseTransform,
            //    Math::float4(1,0,0,0.4f));
            //CoreGraphics::ShapeRenderer::Instance()->AddShape(shape);

            //CoreGraphics::Shape shape2;
            //shape2.SetupSimpleShape(Threading::Thread::GetMyThreadId(),
            //    CoreGraphics::Shape::Sphere,
            //    transform,
            //    Math::float4(0,1,0,0.4f));
            //CoreGraphics::ShapeRenderer::Instance()->AddShape(shape2);     
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
AttachmentServer::SetVisibilityOnAttachments(const Ptr<Graphics::ModelEntity>& baseEntity, bool visible)
{
    // do for all attachments of basentity
    IndexT index;
    for (index = 0; index < this->attachments.Size(); index++)
    {
        // get current attachment
        Attachment& attachment = this->attachments[index];
        if (attachment.baseEntity == baseEntity)
        {
            attachment.attachedEntity->SetVisible(visible);
        }
    }
}
} // namespace Graphics
