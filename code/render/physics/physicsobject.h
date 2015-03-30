#pragma once
//------------------------------------------------------------------------------
/**
    @class Physics::PhysicsObject

	Abstract class for most physics-object classes to derive from.
        
    (C) 2012 Johannes Hirche, LTU Skelleftea
*/
#include "core/refcounted.h"
#include "math/matrix44.h"
#include "physics/collider.h"
#include "core/weakptr.h"
#include "physics/materialtable.h"
#include "physics/contact.h"
#include "physics/model/templates.h"


//------------------------------------------------------------------------------

namespace Physics
{

class BaseScene;
class Collider;
class PhysicsObject;

class PhysicsUserData : public Core::RefCounted
{	
	__DeclareClass(PhysicsUserData);
public:
	PhysicsUserData():physicsObject(NULL),enableCollisionCallback(false){}
	PhysicsObject* physicsObject;
	Ptr<Core::RefCounted> object;
	bool enableCollisionCallback;
};

class PhysicsObject : public Core::RefCounted
{

	__DeclareClass(PhysicsObject);
public:  

	/// an unique physics id
	typedef unsigned int Id;

	PhysicsObject();
	virtual ~PhysicsObject();



	virtual void RenderDebug();

	virtual void SetTransform(const Math::matrix44 & trans);
	virtual const Math::matrix44 & GetTransform();

	/// get pointer to Collider we are attached to
	const Ptr<Collider>& GetCollider() const;

	virtual void SetCollideCategory(CollideCategory coll);
	CollideCategory GetCollideCategory() const;	

	virtual void SetCollideFilter(uint mask);
	uint GetCollideFilter() const;

	/// get the objects unique id
	Id GetUniqueId() const;

	/// set name of rigid body
	void SetName(const Util::String& n);
	/// get name of rigid body
	const Util::String& GetName() const;
	

	/// called before simulation step is taken
	virtual void OnStepBefore(){}
	/// called after simulation step is taken
	virtual void OnStepAfter(){}
	/// called before simulation takes place
	virtual void OnFrameBefore(){}
	/// called after simulation takes place
	virtual void OnFrameAfter(){}

	/// enable/disable the object
	virtual void SetEnabled(bool b);
	/// get enabled/disabled state of the object
	virtual bool IsEnabled() const;

	void SetMaterialType(MaterialType t);
	MaterialType GetMaterialType() const;
	void SetUserData(const Ptr<RefCounted> & object);
	PhysicsUserData* GetUserData() const;

	virtual void SetEnableCollisionCallback(bool enable);
	bool GetEnableCollisionCallback();

	static Util::Array<Ptr<PhysicsObject>> CreateFromStream(const Util::String & filename, const Math::matrix44 & transform);
	static Ptr<PhysicsObject> CreateFromTemplate(const PhysicsCommon & tmpl);	
	PhysicsCommon & GetTemplate();

protected:

	friend class BaseScene;
	virtual void Attach(BaseScene * world) = 0;
	virtual void Detach() = 0;
	virtual void SetupFromTemplate(const PhysicsCommon & tmpl);
	bool enabled;
	Ptr<PhysicsUserData> userData;
	Math::matrix44 transform;
	
	Id uniqueId;
	PhysicsCommon common;
	bool attached;
private:
	static Id uniqueIdCounter;
	
};

inline PhysicsCommon & 
PhysicsObject::GetTemplate() 
{
	return this->common;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PhysicsObject::SetTransform(const Math::matrix44 & trans)
{
	this->transform = trans;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::matrix44 &
PhysicsObject::GetTransform()
{
	return this->transform;
}

inline
void
PhysicsObject::SetEnabled(bool b)
{
	this->enabled = b;
}

inline
bool
PhysicsObject::IsEnabled() const
{
	return this->enabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
PhysicsObject::Id
PhysicsObject::GetUniqueId() const
{
	return this->uniqueId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
PhysicsObject::SetName(const Util::String& n)
{
	this->common.name = n;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Util::String&
PhysicsObject::GetName() const
{
	return this->common.name;
}

inline void
PhysicsObject::SetUserData(const Ptr<Core::RefCounted> & data)
{
	this->userData->object = data;
}
inline PhysicsUserData* 
PhysicsObject::GetUserData() const
{
	return this->userData.get();
}

inline const Ptr<Collider> &
PhysicsObject::GetCollider() const
{
	return this->common.collider;
}

inline
CollideCategory 
PhysicsObject::GetCollideCategory() const
{
	return this->common.category;
}

inline
void
PhysicsObject::SetCollideCategory(CollideCategory coll)
{
	this->common.category = coll;
}

inline
uint 
PhysicsObject::GetCollideFilter() const
{
	return this->common.collideFilterMask;
}

inline
void
PhysicsObject::SetCollideFilter(uint coll)
{
	this->common.collideFilterMask = coll;
}

inline
void
PhysicsObject::SetMaterialType(MaterialType mat)
{
	this->common.material = mat;
}
inline
MaterialType
PhysicsObject::GetMaterialType() const
{
	return this->common.material;
}
inline
bool
PhysicsObject::GetEnableCollisionCallback()
{
	return this->userData->enableCollisionCallback;
}
inline
void
PhysicsObject::SetEnableCollisionCallback(bool enable)
{
	this->userData->enableCollisionCallback = enable;
}
}