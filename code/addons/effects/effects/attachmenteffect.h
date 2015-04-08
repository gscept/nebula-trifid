#pragma once
//------------------------------------------------------------------------------
/**
    @class EffectsFeature::AttachmentEffect
    
    Implements a temporary attachment
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "effect.h"
#include "resources/resourceid.h"
#include "game/entity.h"
#include "math/matrix44.h"
#include "graphics/attachmentserver.h"
#include "graphicsfeature/managers/attachmentmanager.h"
namespace EffectsFeature
{
class AttachmentEffect : public Effect
{
	__DeclareClass(AttachmentEffect);
public:
	/// constructor
	AttachmentEffect();
	/// destructor
	virtual ~AttachmentEffect();
	
	/// start attachment
	void OnStart(Timing::Time time);
	/// deactivate the effect
	virtual void OnDeactivate();

	/// sets the joint name
	void SetJoint(const Util::StringAtom& joint);
	/// get the joint name
	const Util::StringAtom& GetJoint();
	/// sets the base entity
	void SetBaseEntity(const Ptr<Graphics::ModelEntity>& entity);
	/// get the base entity
	const Ptr<Graphics::ModelEntity>& GetBaseEntity();
	/// set attachment resource
	void SetAttachmentResource(const Resources::ResourceId& res);
	/// get attachment resources
	const Resources::ResourceId& GetAttachmentResource() const;
	/// set offset
	void SetOffset(const Math::matrix44& offset);
	/// get offset
	const Math::matrix44& GetOffset() const;
	/// set rotation mode
	void SetRotation(const GraphicsFeature::AttachmentManager::AttachmentRotation& rotation);
	/// get rotation mode
	const GraphicsFeature::AttachmentManager::AttachmentRotation& GetRotation();

private:
	Util::StringAtom joint;
	Ptr<Graphics::ModelEntity> baseEntity;
	Ptr<Graphics::GraphicsEntity> graphicsEntity;
	Resources::ResourceId attachment;
	Math::matrix44 offset;	
	GraphicsFeature::AttachmentManager::AttachmentRotation rotation;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
AttachmentEffect::SetJoint( const Util::StringAtom& joint )
{
	this->joint = joint;
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::StringAtom& 
AttachmentEffect::GetJoint()
{
	return this->joint;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AttachmentEffect::SetBaseEntity(const Ptr<Graphics::ModelEntity>& entity)
{
	n_assert(entity.isvalid());
	this->baseEntity = entity;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Graphics::ModelEntity>&
AttachmentEffect::GetBaseEntity()
{
	return this->baseEntity;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AttachmentEffect::SetAttachmentResource( const Resources::ResourceId& res )
{
	this->attachment = res;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId& 
AttachmentEffect::GetAttachmentResource() const
{
	return this->attachment;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AttachmentEffect::SetOffset( const Math::matrix44& offset )
{
	this->offset = offset;
}

//------------------------------------------------------------------------------
/**
*/
inline const Math::matrix44& 
AttachmentEffect::GetOffset() const
{
	return this->offset;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AttachmentEffect::SetRotation(const GraphicsFeature::AttachmentManager::AttachmentRotation& rotation)
{
	this->rotation = rotation;
}

//------------------------------------------------------------------------------
/**
*/
inline const GraphicsFeature::AttachmentManager::AttachmentRotation&
AttachmentEffect::GetRotation()
{
	return this->rotation;
}

} // namespace EffectsFeature
//------------------------------------------------------------------------------