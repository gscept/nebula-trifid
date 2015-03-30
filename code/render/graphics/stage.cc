//------------------------------------------------------------------------------
//  stage.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "graphics/stage.h"
#include "graphics/cameraentity.h"
#include "timing/timer.h"
#include "rendermodules/rt/rtpluginregistry.h"
#include "visibility/visibilitysystems/visibilityquadtree.h"
#include "coregraphics/shaperenderer.h"
#include "coregraphics/rendershape.h"
#include "graphics/globallightentity.h"
#include "graphics/abstractlightentity.h"
#include "threading/thread.h"

namespace Graphics
{
__ImplementClass(Graphics::Stage, 'IGSG', Core::RefCounted);

using namespace Math;
using namespace Util;
using namespace Visibility;
using namespace CoreGraphics;
using namespace Lighting;

//------------------------------------------------------------------------------
/**
*/
Stage::Stage() :
    isAttachedToServer(false),
    curFrameIndex(InvalidIndex)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
Stage::~Stage()
{
    // make sure we've been properly cleaned up
    n_assert(!this->IsAttachedToServer());
    n_assert(this->entities.IsEmpty());
    IndexT i;
    for (i = 0; i < GraphicsEntityType::NumTypes; i++)
    {
        n_assert(this->entitiesByType[i].IsEmpty());
    }
}

//------------------------------------------------------------------------------
/**
*/
void
Stage::OnAttachToServer()
{
    n_assert(!this->IsAttachedToServer());
    this->isAttachedToServer = true;
        
    // open checker, will open its attached visibility systems
    this->visibilityChecker.Open();
}

//------------------------------------------------------------------------------
/**
*/
void
Stage::OnRemoveFromServer()
{
    n_assert(this->IsAttachedToServer());

    // properly cleanup entities
    IndexT entityIndex;
    for (entityIndex = 0; entityIndex < this->entities.Size(); entityIndex++)
    {
        const Ptr<GraphicsEntity>& curEntity = this->entities[entityIndex];
        curEntity->OnRemoveFromStage();
        curEntity->OnDeactivate();
    }
    this->entities.Clear();
    IndexT typeIndex;
    for (typeIndex = 0; typeIndex < GraphicsEntityType::NumTypes; typeIndex++)
    {
        this->entitiesByType[typeIndex].Clear();
    }

    // close visibility checker
    this->visibilityChecker.Close();

    this->isAttachedToServer = false;
}

//------------------------------------------------------------------------------
/**
    Add an entity to the stage. The method OnAttachToStage() will be
    invoked on the entity, and the entity will be inserted into 
    the cell hierarchy of the stage (which in turn call OnAttachToCell()
    on the entity).
*/
void
Stage::AttachEntity(const Ptr<GraphicsEntity>& entity)
{
    n_assert(!entity->IsActive());
    n_assert(!entity->IsAttachedToStage());
    n_assert(entity->GetType() < GraphicsEntityType::NumTypes);

    this->entities.Append(entity);
    this->entitiesByType[entity->GetType()].Append(entity);
    entity->OnActivate();
    entity->OnAttachToStage(this);
    this->visibilityChecker.RegisterEntity(entity);

    // notify render-thread plugins
    RenderModules::RTPluginRegistry::Instance()->OnAttachEntity(entity);
}

//------------------------------------------------------------------------------
/**
    Remove an entity from the stage. This will remove the entity from
    the cell hierarchy of the stage (which invoked OnRemoveFromCell()
    on the entity), and then the method OnRemoveFromStage() will
    be called on the entity.
*/
void
Stage::RemoveEntity(const Ptr<GraphicsEntity>& entity)
{
    n_assert(entity->IsActive());
    n_assert(entity->IsAttachedToStage());
    n_assert(entity->GetStage().get() == this);
    n_assert(entity->GetType() < GraphicsEntityType::NumTypes);

    // notify render-thread plugins
    RenderModules::RTPluginRegistry::Instance()->OnRemoveEntity(entity);

    // first remove entity from vischecker, stage and deactivate it
    this->visibilityChecker.UnregisterEntity(entity);
    entity->OnRemoveFromStage();
    entity->OnDeactivate();
    
    IndexT entitiesByTypeIndex = this->entitiesByType[entity->GetType()].FindIndex(entity);
    n_assert(InvalidIndex != entitiesByTypeIndex);
    this->entitiesByType[entity->GetType()].EraseIndex(entitiesByTypeIndex);

    IndexT entitiesIndex = this->entities.FindIndex(entity);
    n_assert(InvalidIndex != entitiesIndex);
    this->entities.EraseIndex(entitiesIndex);
}

//------------------------------------------------------------------------------
/**
*/
void
Stage::RemoveAllEntities()
{
    while (this->entities.Size() > 0)
    {
        const Ptr<GraphicsEntity>& curEntity = this->entities[0];
        this->RemoveEntity(curEntity);
    }
    this->entities.Clear();
    IndexT typeIndex;
    for (typeIndex = 0; typeIndex < GraphicsEntityType::NumTypes; typeIndex++)
    {
        this->entitiesByType[typeIndex].Clear();
    }
}

//------------------------------------------------------------------------------
/**
    Call the OnCullBefore() method on ALL entities in the stage (no
    matter whether they are visible or not).
*/
void
Stage::OnCullBefore(Timing::Time curTime, Timing::Time globalTimeFactor, IndexT frameIndex)
{
    // check if entities have already been updated for this frame
    if (frameIndex != this->curFrameIndex)
    {
        this->curFrameIndex = frameIndex;
        
		// reset global bounding box
		this->globalBoundingBox.begin_extend();

        // start at end, because we might end up removing entities
        // during the loop!
        IndexT entityIndex;
        for (entityIndex = 0; entityIndex < this->entities.Size(); entityIndex++)
        {
			// hmm, there seems to be a bug here, because if we do this const ref we might actually get the scoped ptr to become NULL 
			// (todo: check if same on GCC)
            Ptr<GraphicsEntity> curEntity = this->entities[entityIndex];
            if (curEntity->IsMarkedForRemove())
            {
				this->RemoveEntity(curEntity);
				entityIndex--;
            }
            else
            {		
                curEntity->OnReset();
                curEntity->OnCullBefore(curTime, globalTimeFactor, frameIndex);

				if (curEntity->GetType() == GraphicsEntityType::Model)
				{
					// extend global bounding box
					this->globalBoundingBox.extend(curEntity->GetGlobalBoundingBox());
				}				
            }
        }

		// finish the global bounding box extension
		this->globalBoundingBox.end_extend();
    }
}

//------------------------------------------------------------------------------
/**
    Update visibility links for a given camera. This will create bidirectional
    visibility links between the camera and all other entities (most importantly
    light and model entities) which are visible through this camera. This
    method must be called once for each active camera after UpdateEntities()
    and before UpdateVisibleLightLinks().
*/
void
Stage::UpdateCameraLinks(const Ptr<CameraEntity>& cameraEntity)
{
    n_assert(cameraEntity.isvalid());

    // clear old links
    visibilityChecker.ClearVisibilityLinks(GraphicsEntity::CameraLink);

    if (cameraEntity.isvalid())
    {   
        // resolve visible model and light entities from this camera
        uint entityTypeMask = (1 << GraphicsEntityType::Model) | (1 << GraphicsEntityType::Light) | (1 << GraphicsEntityType::LightProbe);
        this->visibilityChecker.PerformVisibilityQuery(this->curFrameIndex, cameraEntity.cast<GraphicsEntity>(), entityTypeMask);        
    }

	// go through and add all statically visible objects
	IndexT i;
	for (i = 0; i < this->entities.Size(); i++)
	{
		const Ptr<GraphicsEntity>& curEntity = this->entities[i];
		if (curEntity->IsAlwaysVisible())
		{
			cameraEntity->AddLink(GraphicsEntity::CameraLink, curEntity);
		}
	}
}

//------------------------------------------------------------------------------
/**
    For each visible light entity, this method will create light links
    between the light entities, and model entities influenced by 
    this light. This method must be called after UpdateCameraLinks() (this
    makes sure that no invisible lights and models will be checked).
*/
void
Stage::UpdateLightLinks()
{
    // clear old links
    visibilityChecker.ClearVisibilityLinks(GraphicsEntity::LightLink);
    // for each visible light...
    const Array<Ptr<GraphicsEntity> >& lightEntities = this->entitiesByType[GraphicsEntityType::Light];
    IndexT lightIndex;
    for (lightIndex = 0; lightIndex < lightEntities.Size(); lightIndex++)
    {
        const Ptr<GraphicsEntity>& lightEntity = lightEntities[lightIndex];
		const Ptr<AbstractLightEntity>& abstractLight = lightEntity.cast<AbstractLightEntity>();
        if (lightEntity->GetLinks(GraphicsEntity::CameraLink).Size() > 0 && abstractLight->GetCastShadows())
        {
            // find model entities influenced by this light
            this->visibilityChecker.PerformVisibilityQuery(this->curFrameIndex, lightEntity, (1<<GraphicsEntityType::Model));            
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void 
Stage::NotifyOfEntityTransformChange(const Ptr<GraphicsEntity>& entity)
{
    this->visibilityChecker.UpdateVisibilityContext(entity);
}

//------------------------------------------------------------------------------
/**
*/
void 
Stage::AttachVisibilitySystems(const Util::Array<Ptr<Visibility::VisibilitySystemBase> >& systems)
{
    this->visibilityChecker.AttachVisibilitySystems(systems);
}

//------------------------------------------------------------------------------
/**
*/
void 
Stage::OnRenderDebug()
{
    this->visibilityChecker.OnRenderDebug();

    // render light links
    const Array<Ptr<GraphicsEntity> >& lightEntities = this->entitiesByType[GraphicsEntityType::Light];
    IndexT lightIndex;
    for (lightIndex = 0; lightIndex < lightEntities.Size(); lightIndex++)
    {
        // skip global light
        if (lightEntities[lightIndex]->IsA(Graphics::GlobalLightEntity::RTTI))
        {
            continue;
        }

        const Util::Array<Ptr<GraphicsEntity> >& linkedEntities = lightEntities[lightIndex]->GetLinks(GraphicsEntity::LightLink);
        if (linkedEntities.Size() > 0)
        {   
            const Math::point& lightPos = lightEntities[lightIndex]->GetTransform().get_position();
            Util::Array<Math::point> lines;        
            IndexT i;
            for (i = 0; i < linkedEntities.Size(); ++i)
            {
                const Math::point& entityPos = linkedEntities[i]->GetTransform().get_position();	
                lines.Append(lightPos);                  
                lines.Append(entityPos);
            }   
        
            RenderShape shape;
            shape.SetupPrimitives(Threading::Thread::GetMyThreadId(),
                matrix44::identity(),
                PrimitiveTopology::LineList,
                lines.Size() / 2,
                &(lines.Front()),
                4,
                point(1,1,0),
				CoreGraphics::RenderShape::CheckDepth);
            ShapeRenderer::Instance()->AddShape(shape); 
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
Visibility::VisibilityChecker& 
Stage::GetVisibilityChecker()
{
    return this->visibilityChecker;
}
} // namespace Graphics