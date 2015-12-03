// ------------------------------------------------------------------------------
//  rotationfeature.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
// ------------------------------------------------------------------------------
 #include "stdneb.h"
#include "rotationfeature.h"
#include "input/inputserver.h"
#include "managers/envquerymanager.h"
#include "graphics/view.h"
#include "graphicsfeatureunit.h"
#include "input/mouse.h"
#include "debugrender/debugrender.h"
#include "debugrender/debugshaperenderer.h"

using namespace Math;

namespace LevelEditor2
{

__ImplementClass(LevelEditor2::RotationFeature, 'LERF', LevelEditor2::TransformFeature);

// ------------------------------------------------------------------------------
/**
    Constructor	
*/
RotationFeature::RotationFeature() :
    handleDistance(5.0f),    
    outerCircleScale(1.2f),
    startDragOrientation(0.0f,0.0f,0.0f)
{
	this->startDragMatrix = matrix44::identity();
    this->lastStartDragDeltaMatrix = matrix44::identity();
}

// ------------------------------------------------------------------------------
/**
    Destructor	
*/
RotationFeature::~RotationFeature()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
RotationFeature::DragMode 
RotationFeature::GetMouseHandle( const Math::line& worldMouseRay )
{
	const float maxAngleDifference(0.2f);
	const float outerRadiusTolerance(0.5f);

	// define picking sphere radius
	float radius(this->handleDistance * this->handleScale);
	float outerRadius(radius * this->outerCircleScale);

	// get collision point of ray with sphere
	vector collisionPoint = GetSphereIntersectionPoint(worldMouseRay, this->origin, radius);

	// vector from collision point to handle origin
	vector colVec = collisionPoint - this->origin;
	colVec = vector::normalize(colVec);

	if (worldMouseRay.distance(this->origin) < radius)
	{
		float closestHandle = 0;
		vector plane_normal1, plane_normal2, plane_normal3;
		DragMode retval = NONE;
		float smallestAngle = FLT_MAX;	
		DragMode modes[] = {X_AXIS, Y_AXIS, Z_AXIS};
		vector normals[] = {xPlane.get_normal(), yPlane.get_normal(), zPlane.get_normal()};

		IndexT i;
		for (i = 0; i < 3; i++)
		{
			vector normal = normals[i];
			float angle;
			normal = vector::normalize(normal);
			angle = 0.5f*(float)PI - ((float)acos(vector::dot3(normal, colVec)));
			if (abs(angle) < maxAngleDifference &&
				abs(angle) < smallestAngle) 
			{
				retval = modes[i];
				smallestAngle = angle;
			}
		}

		return retval;
	}
	else
	{
		float distanceRayHandle(worldMouseRay.distance(this->origin));
		if (distanceRayHandle < outerRadius + outerRadiusTolerance * this->handleScale &&
			distanceRayHandle > outerRadius - outerRadiusTolerance * this->handleScale)
		{
			return VIEW_AXIS;
		}
	}

	return NONE;
}

//------------------------------------------------------------------------------
/**
	Checks mouse position and if it's over a rotation feature handle
    start the rotation mode.

*/
void
RotationFeature::StartDrag()
{
    TransformFeature::StartDrag();

	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
    
    // store copy of delta matrix
    this->lastStartDragDeltaMatrix = this->deltaMatrix;
    
    this->xDragStart = this->xAxis;
    this->yDragStart = this->yAxis;
    this->zDragStart = this->zAxis;

    // get mouse position on screen
	const float2& mousePos = Input::InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();

    // get mouse ray
    const float rayLength = 5000.0f;
    line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);
    
    // define picking sphere radius
    float radius(this->handleDistance * this->handleScale);
    float distanceRayHandle(worldMouseRay.distance(this->origin));
        
	// get collision point of ray with sphere
	vector collisionPoint = GetSphereIntersectionPoint(worldMouseRay, this->origin, radius);

    // vector from collision point to handle origin
    vector colVec = collisionPoint - this->origin;
	colVec = vector::normalize(colVec);
    
	// calculate start drag orientation
	this->startDragOrientation = this->ComputeDragVector(worldMouseRay, this->currentDragMode);
}

//------------------------------------------------------------------------------
/**
    Checks mouse position, and if in active drag mode, it will update the
    delta rotation matrix.
*/
void
RotationFeature::Drag()
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();

    TransformFeature::Drag();
    const float2& mousePos = Input::InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();

    // get mouse ray
    const float rayLength = 5000.0f;
    line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);
    vector currentDragVec = this->ComputeDragVector(worldMouseRay, this->currentDragMode);

	
	//float angleDiff = n_acos(vector::dot3(currentDragVec, this->startDragOrientation));
	//vector cross = vector::cross3(currentDragVec, this->startDragOrientation);
	//if (vector::dot3(this->view_plane.get_normal(), cross) >= 0) angleDiff = -angleDiff;
	
    if (X_AXIS == this->currentDragMode)
    {
		float dot = vector::dot3(currentDragVec, this->startDragOrientation);
		float det = vector::dot3(this->xAxis, vector::cross3(this->startDragOrientation, currentDragVec));
		float angleDiff = atan2f(det, dot);

		this->deltaMatrix = matrix44::rotationaxis(this->xAxis, angleDiff);
    }
    else if (Y_AXIS == this->currentDragMode)
    {
		float dot = vector::dot3(currentDragVec, this->startDragOrientation);
		float det = vector::dot3(this->yAxis, vector::cross3(this->startDragOrientation, currentDragVec));
		float angleDiff = atan2f(det, dot);

		this->deltaMatrix = matrix44::rotationaxis(this->yAxis, angleDiff);
    }
    else if (Z_AXIS == this->currentDragMode)
    {
		float dot = vector::dot3(currentDragVec, this->startDragOrientation);
		float det = vector::dot3(this->zAxis, vector::cross3(this->startDragOrientation, currentDragVec));
		float angleDiff = atan2f(det, dot);

		this->deltaMatrix = matrix44::rotationaxis(this->zAxis, angleDiff);
    }
    else if (VIEW_AXIS == this->currentDragMode)
    {
		float dot = vector::dot3(currentDragVec, this->startDragOrientation);
		float det = vector::dot3(this->viewAxis, vector::cross3(this->startDragOrientation, currentDragVec));
		float angleDiff = atan2f(det, dot);

		this->deltaMatrix = matrix44::rotationaxis(this->viewAxis, angleDiff);
    }
}

//------------------------------------------------------------------------------
/**
	Computes a vector that points to the intersection point of given line with
    a layer depending of the given drag mode. The angle between vectors computed
    by this method can used to calculate the rotation delta of the feature.
*/
vector
RotationFeature::ComputeDragVector(const line& ray, DragMode mode)
{
	float4 t;
	this->viewPlane.intersectline(ray.start(), ray.end(), t);
	return vector::normalize(t - this->origin);
}

//------------------------------------------------------------------------------
/**
    This is called when the user finished dragging a feature	
*/
void
RotationFeature::ReleaseDrag()
{
    TransformFeature::ReleaseDrag();
}

//------------------------------------------------------------------------------
/**
	Renders the rotation handles, which can be dragged
*/
void
RotationFeature::RenderHandles()
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();
    
    // The following part is needed to get a mouse over visualization
    // for the handles. For thta calculations like in StartDrag() are
    // required.
    
    // define picking sphere radius
    float radius(this->handleDistance * this->handleScale);

    // get mouse position on screen
	const float2& mousePos = Input::InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();

    // get mouse ray
    const float rayLength = 5000.0f;
    line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);
	DragMode mode = NONE;
	if (!this->isInDragMode) mode = this->GetMouseHandle(worldMouseRay);

    // get collision point of ray with sphere
    point collisionPoint = GetSphereIntersectionPoint(worldMouseRay, this->origin, radius);

    // compute angle between vector from collision point to handle origin
    // and axis planes.
    vector colVec = collisionPoint - this->origin;
	colVec = vector::normalize(colVec);

    // declare some variables
    float4 color;
    const int lineCount = 40;
    vector help;
    CoreGraphics::RenderShape::RenderShapeVertex handlePoints[lineCount * 2];
    vector circleVector;
	vector planeNormal;
    IndexT i;
    line intersectionLine;
	planeNormal = vector(this->viewPlane.a(),this->viewPlane.b(), this->viewPlane.c());

    // compute and draw x handle points    
	planeNormal = vector(this->xPlane.a(),this->xPlane.b(), this->xPlane.c());
    PlaneIntersect(this->viewPlane, this->xPlane, intersectionLine);
    circleVector = intersectionLine.vec();
	circleVector = vector::normalize(circleVector);
    circleVector *= this->handleDistance * this->handleScale;

	if (X_AXIS == this->currentDragMode || X_AXIS == mode)
	{
		color = float4(1, 1, 0, 1);
	}
	else
	{
		color = float4(.8f, 0, 0, 1);
	}

    for (i = 0; i < (lineCount*2) - 1; i += 2)
    {
        help.set(circleVector.x(),circleVector.y(),circleVector.z());
		this->RotateVector(help, this->xAxis, (float)-PI * (i / 2) / lineCount);
		handlePoints[i].pos = this->origin + help;

		help = circleVector;
		this->RotateVector(help, this->xAxis, (float)-PI * (i / 2 + 1) / lineCount);
		handlePoints[i+1].pos = this->origin + help;  

		handlePoints[i].color = color;
		handlePoints[i + 1].color = color;
    }
    
    // draw X axis
    Debug::DebugShapeRenderer::Instance()->DrawPrimitives(matrix44::identity(),
        CoreGraphics::PrimitiveTopology::LineList,
        lineCount,
        handlePoints,
        color,
		CoreGraphics::RenderShape::AlwaysOnTop);

    // compute and draw y handle points    
    PlaneIntersect(this->viewPlane, this->yPlane,intersectionLine);
    circleVector = intersectionLine.vec();
	circleVector = vector::normalize(circleVector);
    circleVector *= this->handleDistance * this->handleScale;


	if (Y_AXIS == this->currentDragMode || Y_AXIS == mode)
	{
		color = float4(1, 1, 0, 1);
	}
	else
	{
		color = float4(0, .8f, 0, 1);
	}

    for (i=0; i<(lineCount*2)-1; i+=2)
    {
        help = circleVector;
		this->RotateVector(help, this->yAxis, (float)PI * (i / 2) / lineCount);
        handlePoints[i].pos = this->origin + help;

        help = circleVector;
        this->RotateVector(help, this->yAxis,(float)PI * (i/2 +1) / lineCount);
        handlePoints[i+1].pos = this->origin + help;  

		handlePoints[i].color = color;
		handlePoints[i+1].color = color;
    }
    
    // draw Y axis
    Debug::DebugShapeRenderer::Instance()->DrawPrimitives(matrix44::identity(),
        CoreGraphics::PrimitiveTopology::LineList,
        lineCount,
        handlePoints,
        color,
		CoreGraphics::RenderShape::AlwaysOnTop);

    // compute and draw z handle points    
	PlaneIntersect(this->viewPlane, this->zPlane,intersectionLine);
    circleVector = intersectionLine.vec();
	circleVector = vector::normalize(circleVector);
    circleVector *= this->handleDistance * this->handleScale;

	if (Z_AXIS == this->currentDragMode || Z_AXIS == mode)
	{
		color = float4(1, 1, 0, 1);
	}
	else
	{
		color = float4(0, 0, .8f, 1);
	}

    for (i=0; i<(lineCount*2)-1; i+=2)
    {
        help = circleVector;
        this->RotateVector(help, this->zAxis,-(float)PI * (i/2) / lineCount);
        handlePoints[i].pos = this->origin + help;

        help = circleVector;
        this->RotateVector(help, this->zAxis,-(float)PI * (i/2 +1) / lineCount);
        handlePoints[i+1].pos = this->origin + help;  

		handlePoints[i].color = color;
		handlePoints[i + 1].color = color;
    }
    
    // draw Z axis
    Debug::DebugShapeRenderer::Instance()->DrawPrimitives(matrix44::identity(),
        CoreGraphics::PrimitiveTopology::LineList,
        lineCount,
        handlePoints,
        color,
		CoreGraphics::RenderShape::AlwaysOnTop);
    
    // draw outer circle
    vector normal = vector(this->viewPlane.a(), this->viewPlane.b(), this->viewPlane.c());
    circleVector = FindOrtho(normal);
	circleVector = vector::normalize(circleVector);
    circleVector *= this->handleDistance * this->handleScale * this->outerCircleScale;

	if (VIEW_AXIS == this->currentDragMode || VIEW_AXIS == mode)
	{
		color = float4(1, 1, 0, 1);
	}
	else
	{
		color = float4(.5f, .5f, 1.0f, 1.0f);
	}

    for (i=0; i<(lineCount*2)-1; i+=2)
    {
        help = circleVector;
        this->RotateVector(help, normal, 2*(float)PI * (i/2) / lineCount);
        handlePoints[i].pos = this->origin + help;

        help = circleVector;
        this->RotateVector(help, normal, 2*(float)PI * (i/2 +1) / lineCount);
        handlePoints[i+1].pos = this->origin + help;  

		handlePoints[i].color = color;
		handlePoints[i + 1].color = color;
    }

    // draw it
    Debug::DebugShapeRenderer::Instance()->DrawPrimitives(matrix44::identity(), 
        CoreGraphics::PrimitiveTopology::LineList, 
        lineCount, 
        handlePoints, 
        color, 
		CoreGraphics::RenderShape::AlwaysOnTop);
}

// ------------------------------------------------------------------------------
/**
	Computes different variables, that are used for picking and rendering handles.
*/
void
RotationFeature::UpdateHandlePositions()
{
    this->DecomposeInitialMatrix();

	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
    
    // compute the scale factor
    vector cameraPosition;
    float distanceToView;

	n_assert(cameraEntity.isvalid());
	const matrix44 camTrans = cameraEntity->GetMatrix44(Attr::Transform);
    cameraPosition = camTrans.get_position();

	// calculate scale of handles relative to the distance to the camera
	vector v = (cameraPosition - this->origin);
	distanceToView = v.length();
    this->handleScale = distanceToView;

    // make the handles small
    this->handleScale *= 0.03f;

	this->xAxis = matrix44::transform(vector(1, 0, 0), this->deltaMatrix);
	this->yAxis = matrix44::transform(vector(0, 1, 0), this->deltaMatrix);
	this->zAxis = matrix44::transform(vector(0, 0, 1), this->deltaMatrix);
    this->origin = (this->decomposedTranslation);

    // create orthogonal planes to the feature axis
    this->xPlane.setup_from_points(this->origin, this->origin + point(this->yAxis), this->origin + point(this->zAxis));
    this->yPlane.setup_from_points(this->origin, this->origin + point(this->xAxis), this->origin + point(this->zAxis));
    this->zPlane.setup_from_points(this->origin, this->origin + point(this->xAxis), this->origin + point(this->yAxis));
    
	// calculate view plane
	vector forward(0, 0, -1);
	forward = matrix44::transform(forward, camTrans);
	this->viewPlane.setup_from_point_and_normal(this->origin, forward);
	this->viewAxis = this->viewPlane.get_normal();
}

// ------------------------------------------------------------------------------
/**
	Gets the closest point of intersection of line and sphere.
    If no intersection occurred, it returns the closest point on sphere to line.
*/
vector
RotationFeature::GetSphereIntersectionPoint(const line& ray,const vector& location, float radius)
{
    vector collisionPoint;

    float distanceRayLoc(ray.distance(location));
        
    // get shortest distance point on ray line
    float tForClosestPoint(ray.closestpoint(location));
	vector closestPoint = ray.pointat(tForClosestPoint);
    //vector closestPoint(ray.ipol(tForClosestPoint));
    
    if(distanceRayLoc<radius)
    {              
        // get distance of that point to sphere intersection points
        float distance(sqrt(radius * radius - distanceRayLoc * distanceRayLoc));

        // compute intersection point of ray with sphere
        // get closes point to ray start by "walking back" in the direction of the ray vector
        // starting at from closest point on line to handle origin
        vector rayNormal(ray.vec());
		rayNormal = vector::normalize(rayNormal);// rayNormal.norm();
        collisionPoint = (closestPoint + (rayNormal * (-distance)));
    }
    else
    {
        // calculate closest point on sphere
        collisionPoint = (location + (closestPoint - location) * radius);
    }

    return collisionPoint;
}

//------------------------------------------------------------------------------
/**
*/
void 
RotationFeature::RotateVector( vector& i_v, vector& axis, float angle )
{
    matrix44 rot = matrix44::rotationaxis(axis, angle);
    i_v = matrix44::transform(i_v, rot);
}

//------------------------------------------------------------------------------
/**
*/
bool 
RotationFeature::PlaneIntersect(plane& p1, plane &p2, line& l )
{
	vector n0(p1.a(),p1.b(),p1.c());
	vector n1(p2.a(),p2.b(),p2.c());
	float n00 = vector::dot3(n0,n0);
	float n01 = vector::dot3(n0,n1);
	float n11 = vector::dot3(n1,n1);
	float det = n00 * n11 - n01 * n01;
	const float tol = 1e-06f;
	if (fabs(det) < tol) 
	{
		return false;
	}
	else 
	{
		float inv_det = 1.0f/det;
		float c0 = (n11 * p1.d() - n01 * p2.d())* inv_det;
		float c1 = (n00 * p2.d() - n01 * p1.d())* inv_det;
		l.m = vector::cross3(n0,n1);
		l.b = n0 * c0 + n1 * c1;
		return true;
	}
}

//------------------------------------------------------------------------------
/**
*/
void RotationFeature::GetAnglesFromMat( const Math::matrix44& i_m, float& x, float& y, float& z )
{
	//extract rotation from matrix
	quaternion _q = matrix44::rotationmatrix(i_m);
	matrix44 m = matrix44::rotationquaternion(_q);

 	z = atan2(m.getrow1().x(), m.getrow0().x());
 	y = atan2(-m.getrow2().x(), sqrt(pow(m.getrow2().y(),2)+ pow(m.getrow2().z(),2)));
 	x = atan2(m.getrow2().y(), m.getrow2().z());
}

}//namespace