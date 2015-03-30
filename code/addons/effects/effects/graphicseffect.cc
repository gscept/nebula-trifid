//------------------------------------------------------------------------------
//  graphicseffect.cc
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphicseffect.h"
#include "graphics/graphicsserver.h"
#include "graphicsfeature/graphicsfeatureunit.h"
#include "graphics/stage.h"

using namespace Graphics;
namespace EffectsFeature
{
__ImplementClass(EffectsFeature::GraphicsEffect, 'GAEF', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
GraphicsEffect::GraphicsEffect()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
GraphicsEffect::~GraphicsEffect()
{
	if(this->entity.isvalid())
	{
		this->OnDeactivate();
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsEffect::OnActivate( Timing::Time time )
{
	n_assert(this->resource.IsValid());
	Effect::OnActivate(time);
	this->entity = ModelEntity::Create();
	this->entity->SetResourceId(this->resource);
	this->entity->SetTransform(this->transform);
	GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage()->AttachEntity(this->entity.upcast<GraphicsEntity>());
}

//------------------------------------------------------------------------------
/**
*/
void 
GraphicsEffect::OnDeactivate()
{
	// remove entity
	this->entity->GetStage()->RemoveEntity(this->entity.upcast<GraphicsEntity>());
	this->entity = 0;

	Effect::OnDeactivate();
}

} // namespace FX