//------------------------------------------------------------------------------
//  bdapplication.cc
//  (C) 2009 Radon Labs GmbH
//------------------------------------------------------------------------------

#include "stdneb.h"

#include "debugdrawer.h"
#include "core/debug.h"
#include "math/point.h"
#include "debug/debugfloat.h"
#include "debugrender/debugshaperenderer.h"
#include "coregraphics/textrenderer.h"
#include "debugrender/debugrender.h"

using namespace Math;
using namespace Debug;
using namespace CoreGraphics;

inline 
Math::matrix44 operator*(const Math::matrix44 &a, const Math::matrix44 &b)
{
    return Math::matrix44::multiply(a, b);
}

//------------------------------------------------------------------------------
/**
*/
DebugDrawer::DebugDrawer() :
    m_debugMode(0)
{
}

//------------------------------------------------------------------------------
/**
*/
DebugDrawer::~DebugDrawer()
{
}

//------------------------------------------------------------------------------
/**
*/
void	
DebugDrawer::drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
{
    // we use a box as a line
    n_assert(color.x() >= 0.0f);
    n_assert(color.x() <= 1.0f);
    n_assert(color.y() >= 0.0f);
    n_assert(color.y() <= 1.0f);
    n_assert(color.z() >= 0.0f);
    n_assert(color.z() <= 1.0f);

    const point nFrom(from.x(), from.y(), from.z());
    const point nTo(to.x(), to.y(), to.z());
    const vector dir = nTo - nFrom;
    const point center = nFrom + (dir * 0.5f);
    const float halfWidth = dir.length() * 0.5f;

    // calc scale, the *line* lies in the z-axis
    const matrix44 scale = matrix44::scaling(0.01f, 0.01f, halfWidth);
    
    // calc rotation
    matrix44 rot;
    static const vector axisZ(0.0f, 0.0f, 1.0f);
    const vector dirNormalized = float4::normalize(dir);
    vector rotAxis = float4::cross3(axisZ, dirNormalized);    
    if(0.0f == rotAxis.lengthsq())
    {
        rot = matrix44::identity();
    }
    else
    {
        const float angle = n_acos(float4::dot3(axisZ, dirNormalized));
        rotAxis = float4::normalize(rotAxis);
        rot = matrix44::rotationaxis(rotAxis, angle);
    }

    matrix44 m = scale * rot * matrix44::translation(center);
    DebugShapeRenderer::Instance()->DrawBox(m, float4(color.x(), color.y(), color.z(), 1.0f));
}

//------------------------------------------------------------------------------
/**
*/
void	
DebugDrawer::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
{
    const matrix44 m = matrix44::multiply(matrix44::scaling(0.02f, 0.02f, 0.02f),
                                          matrix44::translation(PointOnB.x(), PointOnB.y(), PointOnB.z()));
    DebugShapeRenderer::Instance()->DrawBox(m, float4(1.0f, 0.0f, 0.0f, 1.0f));
}

//------------------------------------------------------------------------------
/**
*/
void	
DebugDrawer::reportErrorWarning(const char* warningString)
{
    n_printf("DebugDrawer::reportErrorWarning %s\n", warningString);
}

//------------------------------------------------------------------------------
/**
*/
void	
DebugDrawer::draw3dText(const btVector3& location,const char* textString)
{
    _debug_text3D(Util::String(textString), 
                  point(location.x(), location.y(), location.z()), 
                  float4(1.0f, 0.0f, 0.0f, 1.0f));     
}
