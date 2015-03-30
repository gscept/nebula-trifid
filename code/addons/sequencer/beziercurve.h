#ifndef SEQUENCER_BEZIERCURVE_H
#define SEQUENCER_BEZIERCURVE_H
//------------------------------------------------------------------------------
/**
    @class Sequencer::BezierCurve
    
    A 2dim Bezier curve of degree 3 that is used for key animation.
*/
#include "foundation/core/refcounted.h"
#include "util/fixedarray.h"
#include "math/float2.h"
#include "timing/time.h"

using namespace Math;
using namespace Util;

namespace Sequencer
{

class BezierCurve : public Core::RefCounted
{
    __DeclareClass(BezierCurve);
public:
    /// constructor
    BezierCurve();
    /// destructor
    virtual ~BezierCurve();
    /// get start point of curve
    float2 GetStartP() const;
    /// set start point of curve
    void SetStartP(float2 p);
    /// get first control point of curve
    float2 GetStartCP() const;
    /// set first control point of curve
    void SetStartCP(float2 cp);
    /// get end point of curve
    float2 GetEndP() const;
    /// set end point of curve
    void SetEndP(float2 p);
    /// get second control point of curve
    float2 GetEndCP() const;
    /// set second control point of curve
    void SetEndCP(float2 cp);
    /// get points of curve in a size 4 array
    const FixedArray<float2>& GetPoints() const;
    /// calculate the bezier co-efficients from bezier points
    void CalculatePolynome();
    /// Get the value of the bezier curve at a certain time
    /// In other words, get the y value from x value
    float EvaluateAtTime(Timing::Time time);

private:
    FixedArray<float2> points;
    float2 a;
    float2 b;
    float2 c;
    float2 d;
};
//------------------------------------------------------------------------------
/**
*/
inline
float2 BezierCurve::GetStartP() const
{
    return points[0];
}
//------------------------------------------------------------------------------
/**
*/
inline
void BezierCurve::SetStartP(float2 p)
{
    points[0] = p;
}
//------------------------------------------------------------------------------
/**
*/
inline
float2 BezierCurve::GetStartCP() const
{
    return points[1];
}
//------------------------------------------------------------------------------
/**
*/
inline
void BezierCurve::SetStartCP(float2 cp)
{
    points[1] = cp;
}

//------------------------------------------------------------------------------
/**
*/
inline
float2 BezierCurve::GetEndP() const
{
    return points[3];
}
//------------------------------------------------------------------------------
/**
*/
inline
void BezierCurve::SetEndP(float2 p)
{
    points[3] = p;
}
//------------------------------------------------------------------------------
/**
*/
inline
float2 BezierCurve::GetEndCP() const
{
    return points[2];
}
//------------------------------------------------------------------------------
/**
*/
inline
void BezierCurve::SetEndCP(float2 cp)
{
    points[2] = cp;
}
//------------------------------------------------------------------------------
/**
*/
inline
const FixedArray<float2>& BezierCurve::GetPoints() const
{
    return points;
}

} // namespace Sequencer
//------------------------------------------------------------------------------
#endif

