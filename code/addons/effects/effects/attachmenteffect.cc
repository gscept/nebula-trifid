//------------------------------------------------------------------------------
//  attachmenteffect.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attachmenteffect.h"
#include "graphicsfeature/managers/attachmentmanager.h"
#include "graphics/modelentity.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::AttachmentEffect, 'ATEF', EffectsFeature::Effect);

//------------------------------------------------------------------------------
/**
*/
AttachmentEffect::AttachmentEffect() :
	baseEntity(0),
	rotation(GraphicsFeature::AttachmentManager::Joint)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AttachmentEffect::~AttachmentEffect()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
AttachmentEffect::OnStart( Timing::Time time )
{
	 // create attachment model entity
    Ptr<Graphics::ModelEntity> attachedEntity = Graphics::ModelEntity::Create();
    attachedEntity->SetResourceId(this->attachment);        
    const Ptr<Graphics::Stage>& stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();
	this->graphicsEntity = attachedEntity.cast<Graphics::GraphicsEntity>();
    stage->AttachEntity(this->graphicsEntity);
	GraphicsFeature::AttachmentManager::Instance()->Attach(this->baseEntity, this->joint, this->graphicsEntity, this->offset, this->rotation);
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentEffect::OnDeactivate()
{	
	if (this->graphicsEntity->IsValid() && !this->graphicsEntity->IsMarkedForRemove())
	{
		this->graphicsEntity->MarkRemove();
	}
	this->graphicsEntity = 0;
	this->baseEntity = 0;
}

} // namespace EffectsFeature