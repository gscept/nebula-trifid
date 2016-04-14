#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::Stage
    
    A graphics stage groups graphics entities (models, cameras and lights)
    together for rendering. The main job of a Stage is to speed up
    visibility queries between the attached graphics entities. Different
    visibility query strategies are implemented by Stage subclasses. 
    Nebula3 comes with a set of generic Stage subclasses for different
    purposes, but applications are free to derive their own subclasses
    which implement visibility query mechanisms tailored to the application.
    
    Visibility queries exist in the following variations:
    
    - Camera->Light: this finds all light entities visible from a
      given camera
    - Camera->Model: this finds all model entities visible from a
      given camera
    - Light->Model: this finds all model entities which are lit
      by a given light source
    
    Those visibility queries establish so-called visibility links
    between graphics entities which are then used by the lower
    level rendering subsystems to speed up rendering.

    To render the content of a stage, at least one View object is 
    needed. A View object binds renders a stage through a camera entity
    into a render target. Any number of View objects can exist in
    parallel, and may be bound to any Stage. Furthermore, dependencies
    between View objects may be defined (so that a View object will
    first ask the View objects it depends on to render themselves).

    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/stringatom.h"
#include "graphics/graphicsentity.h"
#include "graphics/graphicsentitytype.h"
#include "timing/time.h"                   
#include "visibility/visibilitychecker.h"

//------------------------------------------------------------------------------
namespace Graphics
{
class CameraEntity;

class Stage : public Core::RefCounted
{
    __DeclareClass(Stage);
public:
    /// constructor
    Stage();
    /// destructor
    virtual ~Stage();

    /// return true if currently attached to graphics server
    bool IsAttachedToServer() const;
    /// get human readable name of the stage
    const Util::StringAtom& GetName() const;
    
    /// attach an entity to the stage
    virtual void AttachEntity(const Ptr<GraphicsEntity>& graphicsEntity);
    /// remove an entity from the stage
    virtual void RemoveEntity(const Ptr<GraphicsEntity>& entity);
    /// notify of an transform change
    virtual void NotifyOfEntityTransformChange(const Ptr<GraphicsEntity>& entity);
    /// get an array of all entities attached to the stage
    const Util::Array<Ptr<GraphicsEntity> >& GetEntities() const;
    /// get entities by type
    const Util::Array<Ptr<GraphicsEntity> >& GetEntitiesByType(GraphicsEntityType::Code type) const;
    /// remove all entities
    void RemoveAllEntities();

	/// get bounding box for entire stage
	const Math::bbox& GetGlobalBoundingBox() const;

    /// call OnCullBefore in entities in the stage
    virtual void OnCullBefore(Timing::Time curTime, Timing::Time globalTimeFactor, IndexT frameIndex);
    /// update camera links for a given camera
    virtual void UpdateCameraLinks(const Ptr<CameraEntity>& cameraEntity);
    /// update light links for all visible lights (after updating camera links!)
    virtual void UpdateLightLinks();

    /// attach visibility systems to checker
    void AttachVisibilitySystems(const Util::Array<Ptr<Visibility::VisibilitySystemBase> >& systems);
    /// get visibility checker
    Visibility::VisibilityChecker& GetVisibilityChecker();
    /// on render debug
    void OnRenderDebug();

protected:
    friend class GraphicsServer;

    /// set a human readable name on the stage
    void SetName(const Util::StringAtom& name);
    /// called when the stage is attached to graphics server
    virtual void OnAttachToServer();
    /// called when the stage is detached from graphics server
    virtual void OnRemoveFromServer();

    bool isAttachedToServer;
    IndexT curFrameIndex;
    Util::StringAtom name;
    Visibility::VisibilityChecker visibilityChecker;    
	Math::bbox globalBoundingBox;
    
    Util::Array<Ptr<GraphicsEntity> > entities;
    Util::Array<Ptr<GraphicsEntity> > entitiesByType[GraphicsEntityType::NumTypes];
};

//------------------------------------------------------------------------------
/**
*/
inline bool
Stage::IsAttachedToServer() const
{
    return this->isAttachedToServer;
}

//------------------------------------------------------------------------------
/**
*/
inline void
Stage::SetName(const Util::StringAtom& n)
{
    this->name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom&
Stage::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Graphics::GraphicsEntity> >&
Stage::GetEntities() const
{
    return this->entities;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<Graphics::GraphicsEntity> >&
Stage::GetEntitiesByType(GraphicsEntityType::Code entityType) const
{
    n_assert((entityType >= 0) && (entityType < GraphicsEntityType::NumTypes));
    return this->entitiesByType[entityType];
}


//------------------------------------------------------------------------------
/**
*/
inline const Math::bbox& 
Stage::GetGlobalBoundingBox() const
{
	return this->globalBoundingBox;
}

} // namespace Graphics
//------------------------------------------------------------------------------
