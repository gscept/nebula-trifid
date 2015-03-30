#ifndef PHYSICS_RAY_H
#define PHYSICS_RAY_H
//------------------------------------------------------------------------------
/**
    @class Physics::Ray

    Ray objects are used to perform ray checks on the physics world
    representation.
    
    (C) 2003 RadonLabs GmbH
*/
#include "core/refcounted.h"
#include "physics/contactpoint.h"
#include "physics/filterset.h"
#include "ode/ode.h"
#define BAN_OPCODE_AUTOLINK
#include "opcode/opcode.h"

//------------------------------------------------------------------------------
namespace Physics
{
class Ray : public Core::RefCounted
{
	__DeclareClass(Ray);
public:
    /// constructor
    Ray();
    /// destructor
    virtual ~Ray();
    /// set ray origin
    void SetOrigin(const Math::vector& v);
    /// get ray origin
    const Math::vector& GetOrigin() const;
    /// set direction and length vector
    void SetVector(const Math::vector& v);
    /// get direction and length vector
    const Math::vector& GetVector() const;
    /// get normalized vector
    const Math::vector& GetNormVector() const;
    /// set exclusion filter set
    void SetExcludeFilterSet(const FilterSet& f);
    /// get exclusion filter set
    const FilterSet& GetExcludeFilterSet() const;
    /// do a complete ray check, return all contact points unsorted in user provided contacts array
    virtual int DoRayCheckAllContacts(const Math::matrix44& transform, Util::Array<ContactPoint>& contacts);
    /// get ode geom id
    dGeomID GetGeomId() const;

private:
    /// ODE collision callback function
    static void OdeRayCallback(void* data, dGeomID o1, dGeomID o2);
    
    enum
    {
        MaxContacts = 16,
    };
    Math::vector orig;
    Math::vector vec;
    Math::vector normVec;
    FilterSet excludeFilterSet;
    dGeomID odeRayId;
    static Util::Array<ContactPoint>* contactsArray;             // not owned!
};

//------------------------------------------------------------------------------
/**
*/
inline
dGeomID
Ray::GetGeomId() const
{
    return this->odeRayId;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Ray::SetOrigin(const Math::vector& v)
{
    this->orig = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
Ray::GetOrigin() const
{
    return this->orig;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Ray::SetVector(const Math::vector& v)
{
    this->vec = v;
    this->normVec = Math::vector::normalize(v);    
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
Ray::GetVector() const
{
    return this->vec;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
Ray::GetNormVector() const
{
    return this->normVec;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
Ray::SetExcludeFilterSet(const FilterSet& f)
{
    this->excludeFilterSet = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
const FilterSet&
Ray::GetExcludeFilterSet() const
{
    return this->excludeFilterSet;
}

}; // namespace Physics

//------------------------------------------------------------------------------
#endif

