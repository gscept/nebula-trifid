//------------------------------------------------------------------------------
//  animeventattachmenthandler.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "effects/animeventhandler/animeventattachmenthandler.h"
#include "characters/character.h"
#include "effects/animeventregistry.h"
#include "graphics/graphicsserver.h"
#include "graphics/attachmentserver.h"
#include "graphics/modelentity.h"
#include "graphics/stage.h"

namespace EffectsFeature
{

using namespace Graphics;
using namespace Math;

__ImplementClass(EffectsFeature::AnimEventAttachmentHandler, 'AEPH', Animation::AnimEventHandlerBase);

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
		// hmm, if this becomes a const ref, the same bug found in stage.cc row 193 occurs, which leaves this ptr as NULL
        Ptr<ModelEntity> baseEntity = GraphicsServer::Instance()->GetEntityById(event.GetEntityId()).cast<ModelEntity>();
        n_assert(baseEntity.isvalid());

        // create
        Ptr<ModelEntity> modelEntity = ModelEntity::Create();        
        modelEntity->SetResourceId(effect.resource);
        modelEntity->SetVisible(false);

        // attach
        const Ptr<Stage>& stage = baseEntity->GetStage();
        stage->AttachEntity(modelEntity.cast<GraphicsEntity>());

        // attach on base entity
        AttachmentServer::Instance()->AttachEntityTemporary(matrix44::identity(), 
															AttachmentServer::ClearNone,
                                                            effect.jointName.Value(), 
                                                            modelEntity.cast<GraphicsEntity>(),
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
} // namespace EffectsFeature
