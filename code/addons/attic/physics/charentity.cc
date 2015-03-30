//------------------------------------------------------------------------------
//  physics/charentity.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "physics/charentity.h"
#include "physics/physicsserver.h"
#include "physics/rigidbody.h"
#include "physics/shape.h"
#include "physics/boxshape.h"
#include "physics/composite.h"
#include "physics/level.h"
#include "physics/physicsutil.h"
#include "physics/amotor.h"
//#include "physics/ragdoll.h"
#include "physics/compositeloader.h"

using namespace Math;
using namespace Util;

namespace Physics
{
__ImplementClass(Physics::CharEntity, 'PCHE', Physics::PhysicsEntity);

//------------------------------------------------------------------------------
/**
*/
CharEntity::CharEntity() :
    angularVelocityDirty(true),
    maxMovement(0.0f),
    radius(0.3f),
    height(1.8f),
    hover(0.3f),
    //nebCharacter(0),
    ragdollActive(false),
    groundMaterial(InvalidMaterial),
    wasDisabledInTheLastFrame(false),
    distToGround(0.0f),
    setToGround(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
CharEntity::~CharEntity()
{
    n_assert(this->baseBody == 0);
}

//------------------------------------------------------------------------------
/**
    Create the abstract composite for the character entity when the
    entity is "alive" (not in its ragdoll state).
*/
void
CharEntity::CreateDefaultComposite()
{
    PhysicsServer* physicsServer = PhysicsServer::Instance();

    // create a composite with a spherical rigid body and an angular motor
    this->defaultComposite = physicsServer->CreateComposite();

    // create a base rigid body object
    this->baseBody = physicsServer->CreateRigidBody();
    this->baseBody->SetName("CharEntityBody");
    this->baseBody->SetDampingActive(false);

    float capsuleLength = this->height - 2.0f * this->radius - this->hover;
    if (capsuleLength < 0.01) capsuleLength = 0.01;
    matrix44 upRight = matrix44::rotationx(n_deg2rad(90.0f));  
    upRight.translate(vector(0.0f, capsuleLength * 0.5f, 0.0f));
    MaterialType material = MaterialTable::StringToMaterialType("Character");
    if (this->overwriteMaterial.IsValid())
    {
        material = MaterialTable::StringToMaterialType(this->overwriteMaterial);
    }
    Ptr<Shape> shape = (Shape*) physicsServer->CreateCapsuleShape(upRight, material, this->radius, capsuleLength);
    this->baseBody->BeginShapes(1);
    this->baseBody->AddShape(shape);
    this->baseBody->EndShapes();
    this->defaultComposite->BeginBodies(1);
    this->defaultComposite->AddBody(this->baseBody);
    this->defaultComposite->EndBodies();

    this->defaultComposite->SetTransform(this->transform);
    // create an amotor connected to the base shape
    this->aMotor = physicsServer->CreateAMotor();
    this->aMotor->SetBodies(this->baseBody, 0);
    this->aMotor->SetNumAxes(2);
    this->aMotor->AxisParams(0).SetAxis(vector(1.0f, 0.0f, 0.0f));
    this->aMotor->AxisParams(1).SetAxis(vector(0.0f, 0.0f, 1.0f));
    this->aMotor->AxisParams(0).SetFMax(100000.0f);
    this->aMotor->AxisParams(1).SetFMax(100000.0f);
    this->defaultComposite->BeginJoints(1);
    this->defaultComposite->AddJoint(this->aMotor);
    this->defaultComposite->EndJoints();
}

//------------------------------------------------------------------------------
/**
    Create the optional ragdoll composite for the character entity.
*/
void
CharEntity::CreateRagdollComposite()
{
    // load and setup ragdoll
    if (this->resourceName.IsValid())
    {
       /* CompositeLoader compLoader;
        this->ragdollComposite = (Ragdoll*) compLoader.Load(this->resourceName)[0].get();
        n_assert(this->ragdollComposite->IsA(Ragdoll::RTTI));
        this->ragdollComposite->SetTransform(this->transform);
        this->ragdollComposite->SetCharacter(this->GetCharacter());
        this->ragdollComposite->Bind();*/
    }
}

//------------------------------------------------------------------------------
/**
    Called when the entity is attached to the game entity. This will
    initialize the embedded composite object.
*/
void
CharEntity::OnActivate()
{
    n_assert(this->baseBody == 0);
    this->active = true;

    // create the default composite object (when the character is alive)
    this->CreateDefaultComposite();

    // create optional ragdoll composite object
    this->CreateRagdollComposite();

    this->SetComposite(this->defaultComposite);
    this->SetAlive(true);

    this->setToGround = true;
    // NOTE: do NOT call parent class
}

//------------------------------------------------------------------------------
/**
    Called when the Physics entity is removed from the Game entity.
*/
void
CharEntity::OnDeactivate()
{
    n_assert(this->baseBody != 0);
    Level* level = PhysicsServer::Instance()->GetLevel();
    n_assert(level);

    PhysicsEntity::OnDeactivate();

    this->baseBody = 0;
    this->aMotor = 0;
    this->defaultComposite = 0;
    //this->ragdollComposite = 0;
    //this->ragdollImpulse = 0;
    //this->SetCharacter(0);
}

//------------------------------------------------------------------------------
/**
    Set world space transform. The actual physics transform will be
    offset by the ball radius in the Y direction.

    @param  m   a 4x4 transformation matrix in world space
*/
void
CharEntity::SetTransform(const matrix44& m)
{
    matrix44 offsetMatrix(m);
    float4 pos = offsetMatrix.get_position();
    pos.y() += this->radius + this->hover;
    offsetMatrix.set_position(pos);
    PhysicsEntity::SetTransform(offsetMatrix);
    this->lookatDirection = -m.get_zaxis();
}

//------------------------------------------------------------------------------
/**
    Get world space transform. The returned matrix will always be
    in the upright position (local y axis aligned with global y axis), 
    and the -z axis will look into the velocity direction.

    @return     a 4x4 transformation matrix in world space
*/
matrix44
CharEntity::GetTransform() const
{
    if (this->IsRagdollActive())
    {
        return PhysicsEntity::GetTransform();
    }
    else
    {
        const matrix44& tmp = PhysicsEntity::GetTransform();        
        matrix44 fixedTransform = matrix44::lookatrh(tmp.get_position(), tmp.get_position() + this->lookatDirection, vector::upvec());
        vector translation(tmp.get_position()); 
        float4 pos = fixedTransform.get_position();
        pos.y() -= this->radius + this->hover;
        fixedTransform.set_position(pos);
        return fixedTransform;
    }
}

//------------------------------------------------------------------------------
/**
    This function checks if the character is currently touching the ground
    by doing a vertical ray check.
*/
bool
CharEntity::CheckGround(float& dist)
{
    point pos = this->GetTransform().get_position();
    point from(pos.x(), pos.y() + this->radius * 2.0f, pos.z());
    point dir(0.0f, -this->radius * 4.0f, 0.0f);

    FilterSet excludeSet = this->groundExcludeSet;
    excludeSet.AddEntityId(this->GetUniqueId());
    const ContactPoint* contact = PhysicsServer::Instance()->GetClosestContactAlongRay(from, dir, excludeSet);    
    if (contact)
    {
        dist = vector(contact->GetPosition() - from).length() - this->radius * 2.0f;
        this->groundMaterial = contact->GetMaterial();
        return true;
    }
    else
    {
        const ContactPoint* contact = PhysicsServer::Instance()->GetClosestContactAlongRay(from, dir * -1, excludeSet);    

        if (contact)
        {
            dist = vector(contact->GetPosition() - from).length() - this->radius * 2.0f;
            this->groundMaterial = contact->GetMaterial();
            return true;
        }
        else
        {
            dist = 2.0f * this->radius;
            this->groundMaterial = InvalidMaterial;
            return false;
        }
    }
}

//------------------------------------------------------------------------------
/**
*/
void
CharEntity::SetDesiredVelocity(const vector& v, float m)
{
    this->desiredVelocity = v;
    this->angularVelocityDirty = true;
    this->maxMovement = m;

    float estimatedMovement = this->desiredVelocity.length() * (float) this->level->GetStepSize() * this->level->GetStepCount();
    if (estimatedMovement > this->maxMovement)
    {
        this->desiredVelocity = vector::normalize(this->desiredVelocity);
        this->desiredVelocity *= (this->maxMovement / ((float) this->level->GetStepSize() * this->level->GetStepCount()));
    }
}

//------------------------------------------------------------------------------
/**
    Called before a simulation step is taken. This will convert the
    linear desired velocity into an angular velocity.
*/
void
CharEntity::OnStepBefore()
{
    if (this->IsAlive() && !this->IsRagdollActive())
    {
        // detect a activation from collide or if the entity should move
        if (this->setToGround ||
            (n_abs(this->distToGround) > 0.01f) ||
            (this->wasDisabledInTheLastFrame && this->IsEnabled()) ||
            this->desiredVelocity.length() > 0.0f)
        {
            this->setToGround = false;
            this->wasDisabledInTheLastFrame = false;
            
            if (!this->IsEnabled())
            {
                this->SetEnabled(true);
            }
            PhysicsServer* physicsServer = PhysicsServer::Instance();
            RigidBody* body = this->baseBody;
            dBodyID odeBodyId = body->GetOdeBodyId();

            // get "touching ground status" and determine ground material
            this->CheckGround(this->distToGround);

            // get current velocity and mass of body
            vector desVel;
            desVel.x() = this->desiredVelocity.x();
            desVel.y() = -distToGround * 50.0f;
            desVel.z() = this->desiredVelocity.z();

            float m = body->GetMass();
            vector curVel = body->GetLinearVelocity();

            // compute resulting impulse
            vector p = (curVel - desVel)* -1  * m;

            // convert impulse to force
            dVector3 odeForce;
            dWorldImpulseToForce(physicsServer->GetOdeWorldId(), dReal(this->level->GetStepSize()), p.x(), p.y(), p.z(), odeForce);

            // set new force
            dBodyAddForce(odeBodyId, odeForce[0], odeForce[1], odeForce[2]);
        }
        else
        {
            this->wasDisabledInTheLastFrame = true;
            if (this->IsEnabled())
            {
                this->SetEnabled(false);
            }
        }
    }

    // call base class
    PhysicsEntity::OnStepBefore();
}

//------------------------------------------------------------------------------
/**
    Activate the optional ragdoll.
*/
void
CharEntity::ActivateRagdoll()
{
    //n_assert(this->nebCharacter);
    //n_assert(!this->IsRagdollActive());

    //this->ragdollActive = true;

    //if (this->ragdollComposite.isvalid())
    //{
    //    // get transform before detaching current composite
    //    matrix44 curTransform = PhysicsEntity::GetTransform();

    //    // switch current composite, this will reset the
    //    // composite's transform to the entity's initial transform
    //    this->SetAlive(false);
    //    this->SetComposite(this->ragdollComposite);
    //    this->SetAlive(true);

    //    // fix transform (all the 180 voodoo is necessary because Nebula2's
    //    // characters are rotated by 180 degree around the Y axis
    //    matrix44 rot180;
    //    rot180.rotate_y(n_deg2rad(180.0f));
    //    static vector upVector(0.0f, 1.0f, 0.0f);
    //    matrix44 m;
    //    m.lookatRh(this->lookatDirection, upVector);
    //    m.translate(curTransform.pos_component());
    //    m.M42 -= this->radius + this->hover;
    //    rot180.mult_simple(m);
    //    this->ragdollComposite->SetTransform(rot180);

    //    // sync physics joint angles with bind pose model
    //    this->ragdollComposite->ReadJoints();

    //    // apply a the stored impulse to the ragdoll
    //    if (this->ragdollImpulse.isvalid())
    //    {
    //        Server::Instance()->ApplyAreaImpulse(this->ragdollImpulse);
    //    }    
    //    this->ragdollImpulse = 0;
    //}
}

//------------------------------------------------------------------------------
/**
    Deactivate the optional ragdoll.
*/
void
CharEntity::DeactivateRagdoll()
{
    n_assert(this->IsRagdollActive());

    this->ragdollActive = false;

    //if (this->ragdollComposite.isvalid())
    //{
    //    this->SetAlive(false);
    //    this->SetComposite(this->defaultComposite);
    //    this->SetAlive(true);
    //}
}

//------------------------------------------------------------------------------
/**
    Overwrite this in a subclass
*/
bool
CharEntity::OnCollide(Shape* collidee)
{
    bool validCollision = PhysicsEntity::OnCollide(collidee);
    validCollision &= !this->groundExcludeSet.CheckShape(collidee);
    return validCollision;
}

//------------------------------------------------------------------------------
/**
    Always return the desired velocity instead of the real velocity.
*/
Math::vector
CharEntity::GetVelocity() const
{
    return this->desiredVelocity;
}

} // namespace Physics