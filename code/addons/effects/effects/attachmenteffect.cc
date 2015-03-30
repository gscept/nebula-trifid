//------------------------------------------------------------------------------
//  attachmenteffect.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "attachmenteffect.h"
#include "graphicsfeature/managers/attachmentmanager.h"

namespace EffectsFeature
{
__ImplementClass(EffectsFeature::AttachmentEffect, 'ATEF', EffectsFeature::Effect);

//------------------------------------------------------------------------------
/**
*/
AttachmentEffect::AttachmentEffect() :
	baseEntity(0),
	rotation(Graphics::AttachmentServer::Local)
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
	GraphicsFeature::AttachmentManager::Instance()->AddGraphicsAttachmentTemporary(this->joint, this->baseEntity, this->attachment, this->offset, this->keepLocal, this->rotation, this->duration);
}

} // namespace EffectsFeature