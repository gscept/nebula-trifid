#pragma once
//------------------------------------------------------------------------------
/**
    This file was generated with Nebula3's idlc compiler tool.
    DO NOT EDIT
*/
#include "messaging/message.h"
#include "math/vector.h"
#include "physics/physicsentity.h"

//------------------------------------------------------------------------------
namespace PhysicsFeature
{
//------------------------------------------------------------------------------
class ApplyImpulseAtPos : public Messaging::Message
{
    __DeclareClass(ApplyImpulseAtPos);
    __DeclareMsgId;
public:
    ApplyImpulseAtPos() 
    { };
public:
    void SetImpulse(const Math::vector& val)
    {
        n_assert(!this->handled);
        this->impulse = val;
    };
    const Math::vector& GetImpulse() const
    {
        return this->impulse;
    };
private:
    Math::vector impulse;
public:
    void SetPosition(const Math::vector& val)
    {
        n_assert(!this->handled);
        this->position = val;
    };
    const Math::vector& GetPosition() const
    {
        return this->position;
    };
private:
    Math::vector position;
public:
    void SetMultiplyByMass(bool val)
    {
        n_assert(!this->handled);
        this->multiplybymass = val;
    };
    bool GetMultiplyByMass() const
    {
        return this->multiplybymass;
    };
private:
    bool multiplybymass;
};
//------------------------------------------------------------------------------
class GetPhysicsEntity : public Messaging::Message
{
    __DeclareClass(GetPhysicsEntity);
    __DeclareMsgId;
public:
    GetPhysicsEntity() 
    { };
public:
    void SetEntity(const Ptr<Physics::PhysicsEntity>& val)
    {
        n_assert(!this->handled);
        this->entity = val;
    };
    const Ptr<Physics::PhysicsEntity>& GetEntity() const
    {
        return this->entity;
    };
private:
    Ptr<Physics::PhysicsEntity> entity;
};
//------------------------------------------------------------------------------
class SetTriggerActive : public Messaging::Message
{
    __DeclareClass(SetTriggerActive);
    __DeclareMsgId;
public:
    SetTriggerActive() 
    { };
public:
    void SetActive(bool val)
    {
        n_assert(!this->handled);
        this->active = val;
    };
    bool GetActive() const
    {
        return this->active;
    };
private:
    bool active;
};
//------------------------------------------------------------------------------
class IsTriggerActive : public Messaging::Message
{
    __DeclareClass(IsTriggerActive);
    __DeclareMsgId;
public:
    IsTriggerActive() 
    { };
public:
    void SetActive(bool val)
    {
        n_assert(!this->handled);
        this->active = val;
    };
    bool GetActive() const
    {
        n_assert(this->handled);
        return this->active;
    };
private:
    bool active;
};
//------------------------------------------------------------------------------
class SetTriggerScale : public Messaging::Message
{
    __DeclareClass(SetTriggerScale);
    __DeclareMsgId;
public:
    SetTriggerScale() 
    { };
public:
    void SetScale(const Math::vector& val)
    {
        n_assert(!this->handled);
        this->scale = val;
    };
    const Math::vector& GetScale() const
    {
        return this->scale;
    };
private:
    Math::vector scale;
};
} // namespace PhysicsFeature
//------------------------------------------------------------------------------
