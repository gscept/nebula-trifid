//------------------------------------------------------------------------------
//  attachmentutilutil.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicsutil/attachmentutil.h"
#include "graphicsfeature/graphicsfeatureprotocol.h"
#include "graphicsfeatureunit.h"

namespace GraphicsFeature
{
//------------------------------------------------------------------------------
/**
*/
void
AttachmentUtil::AddAttachment(const Ptr<Game::Entity> & entity, const Util::StringAtom& joint, const Util::StringAtom& resourceId, const Math::matrix44& offset, GraphicsFeature::AttachmentManager::AttachmentRotation rotation)
{
	Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
	__SendSync(entity, msg);
	// create attachment model entity
	Ptr<Graphics::ModelEntity> attachedEntity = Graphics::ModelEntity::Create();
	attachedEntity->SetResourceId(resourceId);
	const Ptr<Graphics::Stage>& stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();
	stage->AttachEntity(attachedEntity.cast<Graphics::GraphicsEntity>());
	GraphicsFeature::AttachmentManager::Instance()->Attach(msg->GetEntity(), joint, attachedEntity.cast<Graphics::GraphicsEntity>(), offset, rotation);
}

//------------------------------------------------------------------------------
/**
*/
void
AttachmentUtil::AddAttachment(const Ptr<Game::Entity> & entity, const Util::StringAtom& resourceId, const Math::matrix44& offset)
{
	Ptr<GraphicsFeature::GetModelEntity> msg = GraphicsFeature::GetModelEntity::Create();
	__SendSync(entity, msg);
	// create attachment model entity
	Ptr<Graphics::ModelEntity> attachedEntity = Graphics::ModelEntity::Create();
	attachedEntity->SetResourceId(resourceId);
	const Ptr<Graphics::Stage>& stage = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage();
	stage->AttachEntity(attachedEntity.cast<Graphics::GraphicsEntity>());
	GraphicsFeature::AttachmentManager::Instance()->Attach(msg->GetEntity().cast<Graphics::GraphicsEntity>(), attachedEntity.cast<Graphics::GraphicsEntity>(), offset, GraphicsFeature::AttachmentManager::TransformOnly);
}

} // namespace GraphicsFeature