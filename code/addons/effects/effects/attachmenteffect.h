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

	/// sets the joint name
	void SetJoint(const Util::StringAtom& joint);
	/// get the joint name
	const Util::StringAtom& GetJoint();
	/// sets the base entity
	void SetBaseEntity(const Ptr<Game::Entity>& entity);
	/// get the base entity
	const Ptr<Game::Entity>& GetBaseEntity();
	/// set attachment resource
	void SetAttachmentResource(const Resources::ResourceId& res);
	/// get attachment resources
	const Resources::ResourceId& GetAttachmentResource() const;
	/// set offset
	void SetOffset(const Math::matrix44& offset);
	/// get offset
	const Math::matrix44& GetOffset() const;
	/// sets the keep local flag
	void SetKeepLocal(bool b);
	///get the keep local flag
	bool GetKeepLocal() const;
	/// set rotation mode
	void SetRotation(const Graphics::AttachmentServer::AttachmentRotation& rotation);
	/// get rotation mode
	const Graphics::AttachmentServer::AttachmentRotation& GetRotation();

private:
	Util::StringAtom joint;
	Ptr<Game::Entity> baseEntity;
	Resources::ResourceId attachment;
	Math::matrix44 offset;
	bool keepLocal;
	Graphics::AttachmentServer::AttachmentRotation rotation;
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
AttachmentEffect::SetBaseEntity( const Ptr<Game::Entity>& entity )
{
	n_assert(entity.isvalid());
	this->baseEntity = entity;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Game::Entity>& 
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
AttachmentEffect::SetKeepLocal( bool b )
{
	this->keepLocal = b;
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
AttachmentEffect::GetKeepLocal() const
{
	return this->keepLocal;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
AttachmentEffect::SetRotation( const Graphics::AttachmentServer::AttachmentRotation& rotation )
{
	this->rotation = rotation;
}

//------------------------------------------------------------------------------
/**
*/
inline const Graphics::AttachmentServer::AttachmentRotation& 
AttachmentEffect::GetRotation()
{
	return this->rotation;
}

} // namespace EffectsFeature
//------------------------------------------------------------------------------