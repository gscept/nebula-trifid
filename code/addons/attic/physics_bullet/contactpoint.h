#ifndef Physics_CONTACTPOINT_H
#define Physics_CONTACTPOINT_H
//------------------------------------------------------------------------------
/**
    @class Physics::ContactPoint

    Contact points with position and Face-Up vector.

    (C) 2003 RadonLabs GmbH
*/
#include "math/vector.h"
#include "Physics/materialtable.h"
#include "util/array.h"

//------------------------------------------------------------------------------
namespace Physics
{
class RigidBody;


class ContactPoint : public Core::RefCounted
{
	__DeclareClass(ContactPoint);
public:
	

    enum Type
    {
        StaticType,             // contactpoint between static and dynamic objects
        DynamicType,            // contactpoint between dynamic and dynamic objects

        RayCheck,               // contactpoint caused through a raycheck

        NumTypes,
        InvalidType
    };

    /// Construct.
    ContactPoint();

	~ContactPoint();

    /// clear content
    void Clear();
    /// set position to `v'
    void SetPosition(const Math::vector& v);
    /// get position
    const Math::vector& GetPosition() const;

    /// set up vector to `v'
    void SetUpVector(const Math::vector& v);
    /// get up vector
    const Math::vector& GetUpVector() const;

	/// set normal vector to `v'
	void SetNormalVector(const Math::vector& v);
	/// get normal vector
	const Math::vector& GetNormalVector() const;

    /// get penetration depth
    float GetDepth() const;
    /// set penetration depth
    void SetDepth(float d);
    
    /// set optional rigid body
    void SetRigidBody(RigidBody*);    
    /// find rigid body of contact, slow, may return 0
    RigidBody* GetRigidBody() const;

    /// Set material of contact
    void SetMaterial(MaterialType material);
    /// get material of contact
    MaterialType GetMaterial() const;   

    /// set the type of the contact point
    void SetType(Type type);
    /// get the type
    const Type& GetType() const;
	const Util::Array<Ptr<ContactPoint>> & GetAllContacts();
	void AddContact(Ptr<ContactPoint> contacts);

private:
    Math::vector position;
    Math::vector upVector;
	Math::vector normal;
    float depth;    
	Ptr<RigidBody> rigidBody;
    MaterialType material;
    Type type;
	Util::Array<Ptr<ContactPoint>> otherContacts;
};

//------------------------------------------------------------------------------
/**
*/
inline
ContactPoint::ContactPoint() :
    upVector(0.0f, 1.0f, 0.0f),
    depth(0.f),    
    material(InvalidMaterial),
    type(InvalidType)
{
    // empty
}

inline
ContactPoint::~ContactPoint()
{	
	this->rigidBody = 0;
	for(int i=0;i<this->otherContacts.Size();i++)
		this->otherContacts[i] = 0;

	this->otherContacts.Clear();

}
//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::Clear()
{
    this->position.set(0.0f, 0.0f, 0.0f);
    this->upVector.set(0.0f, 1.0f, 0.0f);    
    this->rigidBody = 0;
    this->material = InvalidMaterial;
    this->type = InvalidType;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
ContactPoint::SetPosition(const Math::vector& v)
{
    position = v;
}

inline
void
ContactPoint::SetRigidBody(RigidBody * body)
{
	rigidBody = body;
}
//------------------------------------------------------------------------------
/**
*/
inline 
const Math::vector& 
ContactPoint::GetPosition() const
{
    return position;
}


//------------------------------------------------------------------------------
/**
*/
inline 
void 
ContactPoint::SetNormalVector(const Math::vector& v)
{
	normal = v;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Math::vector& 
ContactPoint::GetNormalVector() const
{
	return normal;
}
//------------------------------------------------------------------------------
/**
*/
inline 
void 
ContactPoint::SetType(Type type)
{
    this->type = type;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const ContactPoint::Type& 
ContactPoint::GetType() const
{
    return this->type;
}

//------------------------------------------------------------------------------
/**
*/
inline 
void 
ContactPoint::SetUpVector(const Math::vector& v)
{
    upVector = v;
}

//------------------------------------------------------------------------------
/**
*/
inline 
const Math::vector& 
ContactPoint::GetUpVector() const
{
    return upVector;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
ContactPoint::GetDepth() const
{
    return this->depth;    
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetDepth(float d)
{
    this->depth = d;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
ContactPoint::SetMaterial(MaterialType mat)
{
    this->material = mat;
}

//------------------------------------------------------------------------------
/**
*/
inline
MaterialType
ContactPoint::GetMaterial() const
{
    return this->material;
}

} // namespace Physics
//------------------------------------------------------------------------------
#endif
