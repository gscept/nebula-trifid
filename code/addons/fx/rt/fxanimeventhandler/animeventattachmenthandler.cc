//------------------------------------------------------------------------------
//  animeventparticlehandler.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "fx/rt/fxanimeventhandler/animeventattachmenthandler.h"
#include "internalgraphics/internalmodelentity.h"
#include "internalgraphics/internalgraphicsentity.h"
#include "internalgraphics/internalgraphicsserver.h"
#include "internalgraphics/internalstage.h"
#include "internalgraphics/attachmentserver.h"
#include "characters/character.h"
#include "fx/rt/animeventregistry.h"

namespace FX
{

using namespace InternalGraphics;
using namespace Math;

__ImplementClass(FX::AnimEventAttachmentHandler, 'AEPH', Animation::AnimEventHandlerBase);

//------------------------------------------------------------------------------
/**
*/
AnimEventAttachmentHandler::AnimEventAttachmentHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
AnimEventAttachmentHandler::~AnimEventAttachmentHandler()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
bool
AnimEventAttachmentHandler::HandleEvent(const Animation::AnimEventInfo& event)
{   
    // first check if we got that attachment event    
    if (AnimEventRegistry::Instance()->HasAttachmentEvent(event.GetAnimEvent().GetName()))
    {
        // get desired effect
        const AnimEventRegistry::AttachmentEvent& effect = AnimEventRegistry::Instance()->GetAttachmentEvent(event.GetAnimEvent().GetName());
        
        // lookup stage from base entity and attach entity
        const Ptr<InternalModelEntity> baseEntity = InternalGraphicsServer::Instance()->GetEntityById(event.GetEntityId()).cast<InternalModelEntity>();
        n_assert(baseEntity.isvalid());

        // create
        Ptr<InternalModelEntity> modelEntity = InternalModelEntity::Create();        
        modelEntity->SetResourceId(effect.resource);
        modelEntity->SetVisible(false);

        // attach
        const Ptr<InternalStage>& stage = baseEntity->GetStage();
        stage->AttachEntity(modelEntity.cast<InternalGraphicsEntity>());

        // attach on base entity
        AttachmentServer::Instance()->AttachEntityTemporary(matrix44::identity(), 
                                                            AttachmentServer::ClearAllIdenticalOnJoint, 
                                                            effect.jointName.Value(), 
                                                            modelEntity.cast<InternalGraphics::InternalGraphicsEntity>(),
                                                            baseEntity,
                                                            effect.keepLocal,
                                                            effect.duration,
                                                            effect.rotation,
                                                            false);
        return true;
    }

    // not found
    return false;
}
} // namespace FX
