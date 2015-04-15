//------------------------------------------------------------------------------
//  translatefeature.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "translatefeature.h"
#include "managers/envquerymanager.h"
#include "debugrender/debugrender.h"
#include "graphicsfeatureunit.h"
#include "input/inputserver.h"
#include "input/mouse.h"
#include "graphics/view.h"
#include "physics/contact.h"
#include "physics/physicsserver.h"

namespace LevelEditor2
{
__ImplementClass(LevelEditor2::TranslateFeature,'LTRF', LevelEditor2::TransformFeature);

using namespace Math;
using namespace Input;

//------------------------------------------------------------------------------
/**
    Constructor
*/
TranslateFeature::TranslateFeature() :
    handleDistance(5.0f),
    relativeMode(false),
    axisLockingMode(false),
    dragPlaneOffset(0.0f),
    freeModeRequested(false),
    dragStartMouseRayOffset(0.0f,0.0f,0.0f),
	snapOffset(1.0f)
{
	// empty
}

//------------------------------------------------------------------------------
/**
    Destructor	
*/
TranslateFeature::~TranslateFeature()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TranslateFeature::DragMode 
TranslateFeature::GetMouseHandle( const Math::line& worldMouseRay )
{
	// Check distance to each of the handles of the feature.
	// If the distance to a handle is close enough, activate drag mode.
	float distance;
	float activationDistance = 1.0f * this->handleScale;

	// needed for distance calculation of x,y,z handles
	point rayPoint,handlePoint;
	line axis;
	float axis_t;
	float nearestHandle = FLT_MAX;

	DragMode modes[] = { X_AXIS, Y_AXIS, Z_AXIS };
	vector handles[] = { xAxis, yAxis, zAxis };
	vector handlePointSize[] = { vector(this->handleScale, 0, 0), vector(0, this->handleScale, 0), vector(0, 0, this->handleScale) };
	DragMode retval = NONE;

	// if we're not above the origin handle, we proceed to check the other handles
	IndexT i;
	for (i = 0; i < 3; i++)
	{
		// check z handle
		axis.set(this->origin + handlePointSize[i] * 0.5f, handles[i] + handlePointSize[i]);
		worldMouseRay.intersect(axis, rayPoint, handlePoint);
		axis_t = axis.closestpoint(rayPoint);
		distance = axis.distance(rayPoint);
		if (distance < activationDistance && axis_t < 1 && axis_t > 0 && distance < nearestHandle)
		{
			nearestHandle = distance;
			retval = modes[i];
		}
	}

	// check origin handle
	distance = worldMouseRay.distance(this->origin);
	if (distance < activationDistance)
	{
		retval = ORIGIN;
	}

	return retval;
}

//------------------------------------------------------------------------------
/**
    Checks the mouse position on screen and tries to drag a handle of the feature.
*/
void
TranslateFeature::StartDrag()
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
    TransformFeature::StartDrag();
    
    point rayPoint,handlePoint;

    // get mouse position on screen
    const float2& mousePos = InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();

    // get mouse ray
    const float rayLength = 5000.0f;
    line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);

    // check origin handle
    if (this->currentDragMode == ORIGIN || this->isInFreeDragMode)
    {
        line centerViewLine = envQueryManager->ComputeMouseWorldRay(float2(0.5f,0.5f), 1, defaultView);
        vector viewVector = centerViewLine.vec();
		vector v1 = FindOrtho(viewVector);
		vector v2 = vector::cross3(viewVector, v1);
        plane translatePlane(this->origin,this->origin+v1,this->origin+v2);
		float4 t;
        translatePlane.intersectline(worldMouseRay.start(), worldMouseRay.end(), t);
        this->dragStartMouseRayOffset = (float4(this->origin) - t);
        return;
    }

    vector axis;
    matrix44 cameraTrans = defaultView->GetCameraEntity()->GetTransform();
    vector zaxis = cameraTrans.get_zaxis();
	vector lockedAxis = zaxis;

    // determine which handle should be used, then adjust the zaxis to not point in said axis direction
	if (this->currentDragMode == X_AXIS)
    {
        // set drag plane offset
        this->dragPlaneOffset = this->handleDistance * this->handleScale;
        worldMouseRay.intersect(line(this->origin, this->xAxis + vector(this->handleScale, 0, 0)), rayPoint, handlePoint);
        this->dragStartMouseRayOffset = (rayPoint - this->xAxis);
		zaxis.set_x(0);
		lockedAxis.set_y(0);
		lockedAxis.set_z(0);
        axis = zaxis;
    }
	else if (this->currentDragMode == Y_AXIS)
    {
        // set drag plane offset
        this->dragPlaneOffset = this->handleDistance * this->handleScale;
        worldMouseRay.intersect(line(this->origin, this->yAxis + vector(0, this->handleScale, 0)), rayPoint, handlePoint);
        this->dragStartMouseRayOffset = (rayPoint -  this->yAxis);
		zaxis.set_y(0);
		lockedAxis.set_x(0);
		lockedAxis.set_z(0);
        axis = zaxis;
    }
	else if (this->currentDragMode == Z_AXIS)
    {
        // set drag plane offset
        this->dragPlaneOffset = this->handleDistance * this->handleScale;
        worldMouseRay.intersect(line(this->origin, this->zAxis + vector(0, 0, this->handleScale)), rayPoint, handlePoint);
        this->dragStartMouseRayOffset = (rayPoint - this->zAxis);
		zaxis.set_z(0);
		lockedAxis.set_x(0);
		lockedAxis.set_y(0);
        axis = zaxis;
    }
    else 
    {
        return;
    }

	point axispoint;

    // if we are in relative mode, we must project the dragstart back to the relative axis
    if (this->relativeMode)
    {
		plane viewPlane = this->viewPlane;
		viewPlane = plane::normalize(viewPlane);

		viewPlane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);
		this->dragStart = axispoint - point(this->initialMatrix.get_position());

        matrix44 rot = this->initialMatrix;
        rot.set_position(point(0, 0, 0));
        matrix44 localForward = matrix44::lookatlh(this->initialMatrix.get_position(), point(this->initialMatrix.get_position() + rot.get_zaxis()), rot.get_yaxis());

        if (this->currentDragMode == X_AXIS)
        {
            vector relativeAxis = matrix44::transform(vector(1, 0, 0), localForward);
            this->dragStart = relativeAxis * (float4::dot3(this->dragStart, relativeAxis) / float4::dot3(relativeAxis, relativeAxis));
        }
		else if (this->currentDragMode == Y_AXIS)
        {
            vector relativeAxis = matrix44::transform(vector(0, 1, 0), localForward);
            this->dragStart = relativeAxis * (float4::dot3(this->dragStart, relativeAxis) / float4::dot3(relativeAxis, relativeAxis));
        }
		else if (this->currentDragMode == Z_AXIS)
        {
            vector relativeAxis = matrix44::transform(vector(0, 0, 1), localForward);
            this->dragStart = relativeAxis * (float4::dot3(this->dragStart, relativeAxis) / float4::dot3(relativeAxis, relativeAxis));
        }
    }
	else
	{
		// calculate drag start position
		plane axisplane;

		axisplane.setup_from_point_and_normal(this->origin, zaxis);
		axisplane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);
		this->dragStart = axispoint - point(this->initialMatrix.get_position());

		axisplane.setup_from_point_and_normal(this->origin, lockedAxis);
		axisplane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);
		this->lockedDragStart = axispoint - point(this->initialMatrix.get_position());
	}
}

//------------------------------------------------------------------------------
/**
    Updates current translation from user input. This is only done when in drag mode.

    For axis translation it checks the intersection between the world mouse ray and the 
    line that represents the local selected axis of the feature.
    Gets the point on the selected axis, which is the shortest to the mouse world ray
    and translate the selected handle to that point.

    If the origin handle is selected the feature will snap to a parallel plane of the camera
    plane or if ground mode is enabled to the physics environment.
*/
void
TranslateFeature::Drag()
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();

    TransformFeature::Drag();
    this->deltaMatrix = Math::matrix44::identity();

    // get mouse position on screen
    const float2& mousePos = InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();

    // get mouse ray
    const float rayLength = 5000.0f;
    line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);
    vector translation;

	plane viewPlane = this->viewPlane;
	viewPlane = plane::normalize(viewPlane);

    matrix44 cameraTrans = defaultView->GetCameraEntity()->GetTransform();
    vector zaxis = cameraTrans.get_zaxis();

	matrix44 rot = this->initialMatrix;
	rot.set_position(0);

	matrix44 localForward = matrix44::lookatlh(this->initialMatrix.get_position(), point(this->initialMatrix.get_position() + rot.get_zaxis()), rot.get_yaxis());
        
    // update in X direction
    if (X_AXIS == this->currentDragMode)
    {
		point axispoint;

		if (this->axisLockingMode)
        {
			zaxis.set_y(0);
			zaxis.set_z(0);

			plane xplane;
			xplane.setup_from_point_and_normal(this->initialMatrix.get_position(), zaxis);
			xplane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);

			// the movement difference is then the one we perform in the view plane
			float diff1 = axispoint.y() - this->initialMatrix.get_position().y();
			float diff2 = axispoint.z() - this->initialMatrix.get_position().z();

			// also add the view-space offset from when we start dragging
			translation.set_y(diff1 - this->lockedDragStart.y());
			translation.set_z(diff2 - this->lockedDragStart.z());
        }
        else
		{
			// set x-factor of axis to 0
			zaxis.set_x(0);

            if (this->relativeMode)
            {
				// intersect with view plane
				viewPlane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);

                // project our translation vector onto the coordinate vector relative to the object
                // also calculate projected startoff point for dragging
                float4 diff = axispoint - point(this->initialMatrix.get_position());
                vector relativeAxis = matrix44::transform(vector(1, 0, 0), localForward);
                vector trans = relativeAxis * (float4::dot3(diff, relativeAxis) / float4::dot3(relativeAxis, relativeAxis));
                    
                translation.set_x(trans.x() - this->dragStart.x());
                translation.set_y(trans.y() - this->dragStart.y());
                translation.set_z(trans.z() - this->dragStart.z());
            }
            else
            {
				plane xplane;
				xplane.setup_from_point_and_normal(this->initialMatrix.get_position(), zaxis);
				xplane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);

                // the movement difference is then the one we perform in the view plane
                float diff = axispoint.x() - this->initialMatrix.get_position().x();                

                // also add the view-space offset from when we start dragging
                translation.set_x(diff - this->dragStart.x());
            }            
        }
		if (this->snapMode)
		{
			n_assert(this->snapOffset != 0);
			translation.set_x(translation.x() - fmod(translation.x() , this->snapOffset) - fmod(initialMatrix.get_position().x(), snapOffset));
		}
    }
    // update in Y direction
	if (Y_AXIS == this->currentDragMode)
    {
		point axispoint;
        if (this->axisLockingMode)
        {
			zaxis.set_x(0);
			zaxis.set_z(0);

			plane xplane;
			xplane.setup_from_point_and_normal(this->initialMatrix.get_position(), zaxis);
			xplane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);

			// the movement difference is then the one we perform in the view plane
			float diff1 = axispoint.x() - this->initialMatrix.get_position().x();
			float diff2 = axispoint.z() - this->initialMatrix.get_position().z();

			// also add the view-space offset from when we start dragging
			translation.set_x(diff1 - this->lockedDragStart.x());
			translation.set_z(diff2 - this->lockedDragStart.z());
        }
        else
		{
            // calculate intersection with arbitrary x/z-plane
            zaxis.set_y(0);

            if (this->relativeMode)
            {
				viewPlane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);

                // project our translation vector onto the coordinate vector relative to the object
                // also calculate projected startoff point for dragging
                float4 diff = axispoint - point(this->initialMatrix.get_position());
                vector relativeAxis = matrix44::transform(vector(0, 1, 0), localForward);
                vector trans = relativeAxis * (float4::dot3(diff, relativeAxis) / float4::dot3(relativeAxis, relativeAxis));

                translation.set_x(trans.x() - this->dragStart.x());
                translation.set_y(trans.y() - this->dragStart.y());
                translation.set_z(trans.z() - this->dragStart.z());
            }
            else
            {
				plane yplane;
				yplane.setup_from_point_and_normal(this->initialMatrix.get_position(), zaxis);
				yplane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);

                // the movement difference is then the one we perform in the view plane
                float diff = axispoint.y() - this->initialMatrix.get_position().y();                

                // also add the view-space offset from when we start dragging
                translation.set_y(diff - this->dragStart.y());
            }
        }
		if(this->snapMode)
		{
			n_assert(this->snapOffset != 0);
			translation.set_y(translation.y() - fmod(translation.y() , this->snapOffset) - fmod(initialMatrix.get_position().y(), snapOffset));
		}
    }
    // update in Z direction
	if (Z_AXIS == this->currentDragMode)
    {
		point axispoint;
        if (this->axisLockingMode)
        {
			zaxis.set_x(0);
			zaxis.set_y(0);

			plane xplane;
			xplane.setup_from_point_and_normal(this->initialMatrix.get_position(), zaxis);
			xplane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);

			// the movement difference is then the one we perform in the view plane
			float diff1 = axispoint.x() - this->initialMatrix.get_position().x();
			float diff2 = axispoint.y() - this->initialMatrix.get_position().y();

			// also add the view-space offset from when we start dragging
			translation.set_x(diff1 - this->lockedDragStart.x());
			translation.set_y(diff2 - this->lockedDragStart.y());
        }
        else
		{
            // calculate intersection between axis line and mouse ray in the x/z-plane
            zaxis.set_z(0);

            if (this->relativeMode)
            { 
				viewPlane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);

                // project our translation vector onto the coordinate vector relative to the object
                // also calculate projected startoff point for dragging
                float4 diff = axispoint - point(this->initialMatrix.get_position());
                vector relativeAxis = matrix44::transform(vector(0, 0, -1), localForward);
                vector trans = relativeAxis * (float4::dot3(diff, relativeAxis) / float4::dot3(relativeAxis, relativeAxis));

                translation.set_x(trans.x() - this->dragStart.x());
                translation.set_y(trans.y() - this->dragStart.y());
                translation.set_z(trans.z() - this->dragStart.z());
            }
            else
            {
				plane zplane;
                zplane.setup_from_point_and_normal(this->initialMatrix.get_position(), zaxis);
                zplane.intersectline(worldMouseRay.start(), worldMouseRay.end(), axispoint);

                // the movement difference is then the one we perform in the view plane
                float diff = axispoint.z() - this->initialMatrix.get_position().z();                

                // also add the view-space offset from when we start dragging
                translation.set_z(diff - this->dragStart.z());
            }
        }
		if (this->snapMode)
		{
			n_assert(this->snapOffset != 0);
			translation.set_z(translation.z() - fmod(translation.z() , this->snapOffset) - fmod(initialMatrix.get_position().z(), snapOffset));
		}
    }
    // special mode, when origin is dragged
    if (ORIGIN == this->currentDragMode)
    {            
        // place matrix on a plane that goes through the last origin point of
        // the feature and is parallel to the camera clip planes
        line centerViewLine = envQueryManager->ComputeMouseWorldRay(float2(0.5f,0.5f), 1, defaultView);
        vector viewVector = centerViewLine.vec();
		vector v1 = FindOrtho(viewVector);
		vector v2 = vector::cross3(viewVector, v1);
        plane translatePlane(this->startDragMatrix.get_position(), this->startDragMatrix.get_position() + v1, this->startDragMatrix.get_position() + v2);

		float4 t;
        translatePlane.intersectline(worldMouseRay.start(), worldMouseRay.end(), t);
        translation = t + this->dragStartMouseRayOffset - this->initialMatrix.get_position();

		if (this->snapMode)
		{
			n_assert(this->snapOffset != 0);
			translation.set( translation.x() - fmod(translation.x() , this->snapOffset) - fmod(initialMatrix.get_position().x(), snapOffset),
				translation.y() - fmod(translation.y() , this->snapOffset) - fmod(initialMatrix.get_position().y(), snapOffset),
				translation.z() - fmod(translation.z() , this->snapOffset) - fmod(initialMatrix.get_position().z(), snapOffset));
		}
    }

    this->deltaMatrix.translate(translation);
}

//------------------------------------------------------------------------------
/**
	Sets current drag mode to none.
*/
void
TranslateFeature::ReleaseDrag()
{
    TransformFeature::ReleaseDrag();
}

//------------------------------------------------------------------------------
/**
    Renders the feature	
*/
void
TranslateFeature::RenderHandles()
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
    
    // get mouse position on screen
    const float2& mousePos = InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();

    // get mouse ray
    const float rayLength = 5000.0f;
    line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);
	DragMode mode = NONE; 
	if (!this->isInDragMode) mode = this->GetMouseHandle(worldMouseRay);

    float4 color;
    matrix44 m;

	// create buffer for line primitives and triangle primitives
	vector line[2];

	// get camera transform
	matrix44 cameraTrans = defaultView->GetCameraEntity()->GetTransform();
    
    // draw origin
	if (ORIGIN == this->currentDragMode || ORIGIN == mode)
    {
        color.set(1.0f,1.0f,0.0f,1.0f);
    }
    else
    {
        color.set(0.85f,0.85f,0.85f,0.5f);
    }
	
	m = matrix44::identity();
    m.scale(vector(this->handleScale, this->handleScale, this->handleScale));
    m.set_position(this->origin);
	Debug::DebugShapeRenderer::Instance()->DrawSphere(m, color, CoreGraphics::RenderShape::AlwaysOnTop);

    // draw X axis + handle
	if (X_AXIS == this->currentDragMode || X_AXIS == mode)
    {
		if (this->axisLockingMode)	color.set(1, 1, 1, 1);
		else					color.set(1, 1, 0, 1);
    }
	else						color.set(.8f, 0, 0, 1);
	
    if (this->relativeMode)
    {
        point axis = this->initialMatrix.get_xaxis();
        m = matrix44::lookatrh(point(0,0,0), axis, this->initialMatrix.get_yaxis());
    }
    else    
    {
        m = matrix44::lookatrh(point(0,0,0), point(1, 0, 0), vector(0, 1, 0));
    }
	//m = matrix44::multiply(matrix44::rotationx(n_deg2rad(90)), m);
	m.scale(vector(this->handleScale, this->handleScale, this->handleScale));
	m.set_position(this->xAxis);
	Debug::DebugShapeRenderer::Instance()->DrawCone(m, color, CoreGraphics::RenderShape::AlwaysOnTop);

	line[0] = this->origin;
	line[1] = this->xAxis;
	Debug::DebugShapeRenderer::Instance()->DrawPrimitives(matrix44::identity(), CoreGraphics::PrimitiveTopology::LineList, 1, line, 4, color, CoreGraphics::RenderShape::AlwaysOnTop);

    // draw Y axis + handle  
	if (Y_AXIS == this->currentDragMode || Y_AXIS == mode)
    {
		if (this->axisLockingMode)	color.set(1, 1, 1, 1);
		else					color.set(1, 1, 0, 1);
    }
	else						color.set(0, .8f, 0, 1);
	
    if (this->relativeMode)
    {
        point axis = this->initialMatrix.get_yaxis();
        m = matrix44::lookatrh(point(0,0,0), axis, this->initialMatrix.get_zaxis());
    }
    else
    {
        m = matrix44::lookatrh(point(0,0,0), point(0, 1, 0), vector(0, 0, 1));
    }
	//m = matrix44::multiply(matrix44::rotationx(n_deg2rad(90)), m);
	m.scale(vector(this->handleScale, this->handleScale, this->handleScale));
	m.set_position(this->yAxis);
	Debug::DebugShapeRenderer::Instance()->DrawCone(m, color, CoreGraphics::RenderShape::AlwaysOnTop);
	
	m = matrix44::identity();
	line[0] = this->origin;
	line[1] = this->yAxis;
	Debug::DebugShapeRenderer::Instance()->DrawPrimitives(m, CoreGraphics::PrimitiveTopology::LineList, 1, line, 4, color, CoreGraphics::RenderShape::AlwaysOnTop);

    // draw Z axis + handle    
	if (Z_AXIS == this->currentDragMode || Z_AXIS == mode)
    {
		if (this->axisLockingMode)	color.set(1, 1, 1, 1);
		else					color.set(1, 1, 0, 1);
    }
	else						color.set(0, 0, .8f, 1);
	
    if (this->relativeMode)
    {
        point axis = this->initialMatrix.get_zaxis();
        m = matrix44::lookatrh(point(0,0,0), axis, this->initialMatrix.get_yaxis());
    }
    else
    {
        m = matrix44::lookatrh(point(0,0,0), point(0, 0, 1), vector(0, 1, 0));
    }
	//m = matrix44::multiply(matrix44::rotationx(n_deg2rad(90)), m);
    m.scale(vector(this->handleScale, this->handleScale, this->handleScale));
    m.set_position(this->zAxis);
	Debug::DebugShapeRenderer::Instance()->DrawCone(m, color, CoreGraphics::RenderShape::AlwaysOnTop);
	
	m = matrix44::identity();
	line[0] = this->origin;
	line[1] = this->zAxis;
	Debug::DebugShapeRenderer::Instance()->DrawPrimitives(m, CoreGraphics::PrimitiveTopology::LineList, 1, line, 4, color, CoreGraphics::RenderShape::AlwaysOnTop);
}

//------------------------------------------------------------------------------
/**
    Computes the position of the handles and the scale of the whole feature.
    The scale correction is used to display handles in a big size, even if they are
    far away.
*/
void
TranslateFeature::UpdateHandlePositions()
{
    // compute origin position of feature
    this->origin = this->initialMatrix.get_position() + this->deltaMatrix.get_position();
    this->origin.set_w(1);

    // compute the scale factor
    vector cameraPosition;
    float distanceToView;
    
	n_assert(cameraEntity.isvalid());
	const matrix44 camTrans = cameraEntity->GetMatrix44(Attr::Transform);
    cameraPosition = camTrans.get_position();
    
	vector v = (cameraPosition - this->origin);
	distanceToView = v.length();
    
    this->handleScale = distanceToView;

    // make the handles small
    this->handleScale *= 0.025f;

    vector xdir, ydir, zdir;
    if (this->relativeMode)
    {
        matrix44 rot = this->initialMatrix;
        rot.set_position(float4(0));
        xdir = float4::normalize(matrix44::transform(vector(1, 0, 0), rot)) * handleDistance * handleScale;
        ydir = float4::normalize(matrix44::transform(vector(0, 1, 0), rot)) * handleDistance * handleScale;
        zdir = float4::normalize(matrix44::transform(vector(0, 0, 1), rot)) * handleDistance * handleScale;
    }
    else
    {
        xdir = vector(this->handleDistance * this->handleScale, 0.0f, 0.0f);
        ydir = vector(0.0f, this->handleDistance * this->handleScale, 0.0f);
        zdir = vector(0.0f, 0.0f, this->handleDistance * this->handleScale);
    }   

    // scale handles
    this->xAxis = this->origin + xdir;
    this->yAxis = this->origin + ydir;
    this->zAxis = this->origin + zdir;

	// create view plane
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
    vector forward(0, 0, -1);
    matrix44 cameraTrans = defaultView->GetCameraEntity()->GetTransform();
    forward = matrix44::transform(forward, cameraTrans);
	this->viewPlane.setup_from_point_and_normal(this->origin, forward);
}

//------------------------------------------------------------------------------
/**
*/
void
TranslateFeature::UpdateTransform( const Math::matrix44 & transform )
{
	this->initialMatrix = transform;
	this->deltaMatrix = Math::matrix44::identity();
}

//------------------------------------------------------------------------------
/**
*/
void
TranslateFeature::SetSnapOffset( float value )
{
	this->snapOffset = value;
}

}//namespace