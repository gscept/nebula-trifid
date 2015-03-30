#ifndef PHYSICS_JOINTAXIS_H
#define PHYSICS_JOINTAXIS_H
//------------------------------------------------------------------------------
/**
    @class Physics::JointAxis

    Hold parameter definitions for a joint axis.
    
    (C) 2005 Radon Labs GmbH
*/
#include "core/types.h"
#include "ode/ode.h"
#include "math/vector.h"

//------------------------------------------------------------------------------
namespace Physics
{
class JointAxis
{
public:
    /// constructor
    JointAxis();
    /// destructor
    ~JointAxis();
    /// set axis vector
    void SetAxis(const Math::vector& v);
    /// get axis vector
    const Math::vector& GetAxis() const;
    /// set current angle
    void SetAngle(float a);
    /// get current angle
    float GetAngle() const;
    /// enable/disable low stop
    void SetLoStopEnabled(bool b);
    /// get low stop enabled status
    bool IsLoStopEnabled() const;
    /// set low stop parameter
    void SetLoStop(float f);
    /// get low stop parameter.
    float GetLoStop() const;
    /// enable/disable low stop
    void SetHiStopEnabled(bool b);
    /// get low stop enabled status
    bool IsHiStopEnabled() const;
    /// set high stop parameter
    void SetHiStop(float f);
    /// get high stop parameter
    float GetHiStop() const;
    /// set angular motor velocity
    void SetVelocity(float f);
    /// get angular motor velocity
    float GetVelocity() const;
    /// set max motor velocity
    void SetFMax(float f);
    /// get max motor velocity
    float GetFMax() const;
    /// set motor fudge factor
    void SetFudgeFactor(float f);
    /// get motor fudge factor
    float GetFudgeFactor() const;
    /// set stop bounciness
    void SetBounce(float f);
    /// get stop bounciness
    float GetBounce() const;
    /// set CFM when not at stop
    void SetCFM(float f);
    /// get CFM when not at stop
    float GetCFM() const;
    /// set ERP used by stops
    void SetStopERP(float f);
    /// get ERP used by stops
    float GetStopERP() const;
    /// set CFM used by stops
    void SetStopCFM(float f);
    /// get CFM used by stops
    float GetStopCFM() const;

private:
    Math::vector axis;
    bool loStopEnabled;
    bool hiStopEnabled;
    float angle;
    float loStop;
    float hiStop;
    float velocity;
    float fMax;
    float fudgeFactor;
    float bounce;
    float cfm;
    float stopErp;
    float stopCfm;
};

//------------------------------------------------------------------------------
/**
*/
inline
JointAxis::JointAxis() :
    axis(0.0f, 1.0f, 0.0f),
    loStopEnabled(false),
    hiStopEnabled(false),
    angle(0.0f),
    loStop(0.0f),
    hiStop(0.0f),
    velocity(0.0f),
    fMax(0.0f),
    fudgeFactor(1.0f),
    bounce(0.0f),
    cfm(0.0f),
    stopErp(0.2f),
    stopCfm(0.0f)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
JointAxis::~JointAxis()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetAxis(const Math::vector& v)
{
    this->axis = v;
}

//------------------------------------------------------------------------------
/**
*/
inline
const Math::vector&
JointAxis::GetAxis() const
{
    return this->axis;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetAngle(float a)
{
    this->angle = a;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetAngle() const
{
    return this->angle;
}

//------------------------------------------------------------------------------
/**
    NOTE: setting LoStop to dInfinity to disable the lostop as said in the ODE 
    docs doesn't work. Thus, explicitely enable/disable low stops.
*/
inline
void
JointAxis::SetLoStopEnabled(bool b)
{
    this->loStopEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
JointAxis::IsLoStopEnabled() const
{
    return this->loStopEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetLoStop(float f)
{
    this->loStop = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetLoStop() const
{
    return this->loStop;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetHiStopEnabled(bool b)
{
    this->hiStopEnabled = b;
}

//------------------------------------------------------------------------------
/**
*/
inline
bool
JointAxis::IsHiStopEnabled() const
{
    return this->hiStopEnabled;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetHiStop(float f)
{
    this->hiStop = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetHiStop() const
{
    return this->hiStop;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetVelocity(float f)
{
    this->velocity = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetVelocity() const
{
    return this->velocity;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetFMax(float f)
{
    this->fMax = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetFMax() const
{
    return this->fMax;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetFudgeFactor(float f)
{
    this->fudgeFactor = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetFudgeFactor() const
{
    return this->fudgeFactor;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetBounce(float f)
{
    this->bounce = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetBounce() const
{
    return this->bounce;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetCFM(float f)
{
    this->cfm = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetCFM() const
{
    return this->cfm;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetStopERP(float f)
{
    this->stopErp = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetStopERP() const
{
    return this->stopErp;
}

//------------------------------------------------------------------------------
/**
*/
inline
void
JointAxis::SetStopCFM(float f)
{
    this->stopCfm = f;
}

//------------------------------------------------------------------------------
/**
*/
inline
float
JointAxis::GetStopCFM() const
{
    return this->stopCfm;
}

}; // namespace 

//------------------------------------------------------------------------------
#endif