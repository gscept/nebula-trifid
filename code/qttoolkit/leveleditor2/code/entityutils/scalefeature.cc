//------------------------------------------------------------------------------
// scalefeature.cc
// (C) 2009 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "scalefeature.h"
#include "managers/envquerymanager.h"
#include "graphics/view.h"
#include "graphicsfeatureunit.h"
#include "input/mouse.h"
//------------------------------------------------------------------------------

using namespace Math;

namespace LevelEditor2
{
__ImplementClass(LevelEditor2::ScaleFeature,'LESF',LevelEditor2::TransformFeature);
    
//------------------------------------------------------------------------------
/**
*/
ScaleFeature::ScaleFeature() :
    handleDistance(5.0f),
    scale(1.0f,1.0f,1.0f),
    axisLocking(false)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ScaleFeature::~ScaleFeature()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
TransformFeature::DragMode
ScaleFeature::GetMouseHandle(const Math::line& worldMouseRay)
{
	// Check distance to each of the handles of the feature.
	// If the distance to a handle is close enough, activate drag mode.
	float distance;
	float activationDistance = 1.0f * this->handleScale;

	// needed for distance calculation of x,y,z handles
	point rayPoint, handlePoint;
	line axis;
	float axis_t;
	float nearestHandle = FLT_MAX;

	DragMode modes[] = { X_AXIS, Y_AXIS, Z_AXIS };
	vector handles[] = { this->xAxis, this->yAxis, this->zAxis };
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
    Checks the mouse position and returns true if the mouse hovers over
    the handle for the given drag mode.
*/
bool
ScaleFeature::IsMouseOverHandle(DragMode handle,const line& worldMouseRay)
{
    // Check distance to each of the handles of the feature.
    // If the distance to a handle is close enough, activate drag mode.
    float distance;
    float activationDistance = 1.0f;

    // needed for distance calculation of x,y,z handles
    point rayPoint,handlePoint;
    line axis;
    float axis_t;

    if (ORIGIN == handle)
    {
        distance = worldMouseRay.distance(this->origin);
        return (distance < activationDistance * this->handleScale);
    }
    else if (X_AXIS == handle)
    {
        // check x handle
        axis.set(this->origin,this->xAxis+vector(this->handleScale,0,0));
        worldMouseRay.intersect(axis,rayPoint,handlePoint);
        axis_t = axis.closestpoint(rayPoint);
        distance = axis.distance(rayPoint);
        return (distance < activationDistance * this->handleScale && axis_t < 1 && axis_t > 0);
    }
    else if (Y_AXIS == handle)
    {
        // check y handle
        axis.set(this->origin,this->yAxis+vector(0,this->handleScale,0));
        worldMouseRay.intersect(axis,rayPoint,handlePoint);
        axis_t = axis.closestpoint(rayPoint);
        distance = axis.distance(rayPoint);
        return (distance < activationDistance * this->handleScale && axis_t < 1 && axis_t > 0);
    }
    else if (Z_AXIS == handle)
    {
        // check z handle
        axis.set(this->origin,this->zAxis+vector(0,0,this->handleScale));
        worldMouseRay.intersect(axis,rayPoint,handlePoint);
        axis_t = axis.closestpoint(rayPoint);
        distance = axis.distance(rayPoint);
        return (distance < activationDistance * this->handleScale && axis_t < 1 && axis_t > 0);
    }
    return false;
}

//------------------------------------------------------------------------------
/**
    If the mouse points to a handle of the feature, the drag mode will start.    	
*/
void
ScaleFeature::StartDrag()
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();

    TransformFeature::StartDrag();
    
    // store copy of delta matrix
    this->lastStartDragDeltaMatrix = this->deltaMatrix;
    
    point rayPoint,handlePoint;
    // get mouse position on screen
    const float2& mousePos = Input::InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();

    // get mouse ray
    const float rayLength = 5000.0f;
    line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);

    // check origin handle
    if (this->IsMouseOverHandle(ORIGIN, worldMouseRay))
    {
        this->dragStartMousePosition = mousePos;
		this->currentDragMode = ORIGIN;
        this->isInDragMode = true;
        return;
    }

    // check x handle
    if (this->IsMouseOverHandle(X_AXIS,worldMouseRay))
    {
        worldMouseRay.intersect(line(this->origin,this->xAxis),rayPoint,handlePoint);
        this->dragStartMouseRayOffset = (
            rayPoint -
            this->xAxis
            );
		this->currentDragMode = X_AXIS;
        this->isInDragMode = true;
        return;
    }

    // check y handle
    if (this->IsMouseOverHandle(Y_AXIS,worldMouseRay))
    {
        worldMouseRay.intersect(line(this->origin,this->yAxis),rayPoint,handlePoint);
        this->dragStartMouseRayOffset = (
            rayPoint -
            this->yAxis
            );
		this->currentDragMode = Y_AXIS;
        this->isInDragMode = true;
        return;
    }

    // check z handle
    if (this->IsMouseOverHandle(Z_AXIS,worldMouseRay))
    {
        worldMouseRay.intersect(line(this->origin,this->zAxis),rayPoint,handlePoint);
        this->dragStartMouseRayOffset = (
            rayPoint -
            this->zAxis
            );
		this->currentDragMode = Z_AXIS;
        this->isInDragMode = true;
        return;
    }
   
    this->currentDragMode = NONE;
}

//------------------------------------------------------------------------------
/**
    If drag mode is active, the mouse position on screen is used to
    scale the feature.
*/
void
ScaleFeature::Drag()
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();

    TransformFeature::Drag();
    
    // get mouse position on screen
    const float2& mousePos = Input::InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();
    // get mouse ray
    const float rayLength = 5000.0f;
    line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);

    // update in X direction
    if (X_AXIS == this->currentDragMode)
    {
        line x_axis(this->origin,this->xAxis);
        point mouseraypoint;
        point x_axispoint;
        x_axis.intersect(worldMouseRay,x_axispoint,mouseraypoint);
        // scale is the distance of initial position plus
        // handle distance on the selected axis and new axis point
        float delta ((vector(x_axispoint) - (this->initialMatrix.get_position() + this->dragStartMouseRayOffset)).length());
        float x_scale = delta / (this->handleDistance*this->handleScale);
        // avoid zero factor scaling
        if(x_scale > 0.1f || x_scale < -0.1f)
		{
            if(this->axisLocking)
            {
                this->scale.set_y(x_scale);
                this->scale.set_z(x_scale);
            }
            else
            {
                this->scale.set_x(x_scale);
            }
        }
    }

    // update in Y direction
    if (Y_AXIS == this->currentDragMode)
    {
        line y_axis(this->origin,this->yAxis);
        point mouseraypoint;
        point y_axispoint;
        y_axis.intersect(worldMouseRay,y_axispoint,mouseraypoint);
        // scale is the distance of initial position plus
        // handle distance on the selected axis and new axis point
        float delta ((vector(y_axispoint) -(this->initialMatrix.get_position() + this->dragStartMouseRayOffset)).length());
        float y_scale = delta / (this->handleDistance*this->handleScale);
        // avoid zero factor scaling
        if(y_scale > 0.1f || y_scale < -0.1f)
		{
			if (this->axisLocking)
            {
                this->scale.set_x(y_scale);
                this->scale.set_z(y_scale);
            }
            else
            {
                this->scale.set_y(y_scale);
            }
        }
    }

    // update in Z direction
    if (Z_AXIS == this->currentDragMode)
    {
        line z_axis(this->origin,this->zAxis);
        point mouseraypoint;
        point z_axispoint;
        z_axis.intersect(worldMouseRay,z_axispoint,mouseraypoint);
        // scale is the distance of initial position plus
        // handle distance on the selected axis and new axis point
        float delta ((vector(z_axispoint) -(this->initialMatrix.get_position() + this->dragStartMouseRayOffset)).length());
        float z_scale = delta / (this->handleDistance*this->handleScale);
        // avoid zero factor scaling
        if(z_scale > 0.1f || z_scale < -0.1f)
		{
			if (this->axisLocking)
            {
                this->scale.set_y(z_scale);
                this->scale.set_x(z_scale);
            }
            else
            {
                this->scale.set_z(z_scale);
            }
        }
    }
        
    // update in ALL directions
    if (ORIGIN == this->currentDragMode)
    {
        float all_scale = 1;
        float2 mouseDelta = mousePos - this->dragStartMousePosition;
        if(mouseDelta.x() < 0)
        {
            all_scale = 1 / (1 + abs(mouseDelta.x())*10);
        }
        else if(mouseDelta.x() > 0)
        {
            all_scale = 1 + mouseDelta.x()*10;
        }
            
        if(all_scale!=0)
		{
            this->scale.set_x(all_scale);
            this->scale.set_y(all_scale);
            this->scale.set_z(all_scale);
        }
    }
    this->deltaMatrix = this->lastStartDragDeltaMatrix;
    this->deltaMatrix.scale(this->scale);
}

//------------------------------------------------------------------------------
/**
    Sets current drag mode to none.
*/
void
ScaleFeature::ReleaseDrag()
{
    TransformFeature::ReleaseDrag();
    
    this->currentDragMode = NONE;
    this->scale = vector(1.0f,1.0f,1.0f);
}

//------------------------------------------------------------------------------
/**
    Renders handles for a scale feature.	
*/
void
ScaleFeature::RenderHandles()
{
	Ptr<BaseGameFeature::EnvQueryManager> envQueryManager = BaseGameFeature::EnvQueryManager::Instance();
	Ptr<Graphics::View> defaultView = GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultView();

    // get mouse position on screen
    const float2& mousePos = Input::InputServer::Instance()->GetDefaultMouse()->GetScreenPosition();

    // get mouse ray
    const float rayLength = 5000.0f;
    line worldMouseRay = envQueryManager->ComputeMouseWorldRay(mousePos, rayLength, defaultView);
	DragMode mode = NONE;
	if (!this->isInDragMode) mode = this->GetMouseHandle(worldMouseRay);

    float4 color;
    matrix44 m;
    
    // shrink box handles a little.
    const float boxScale(0.3f);

    // define a line pointing in x direction
    CoreGraphics::RenderShape::RenderShapeVertex line[2];
    line[0].pos = vector(0, 0, 0);
    line[1].pos = vector(1, 0, 0);

	// draw origin
	if (ORIGIN == this->currentDragMode || ORIGIN == mode)
	{
		color.set(1.0f, 1.0f, 0.0f, 1.0f);
	}
	else
	{
		color.set(0.85f, 0.85f, 0.85f, 0.5f);
	}

    m = matrix44::identity();
    m.scale(vector(this->handleScale,this->handleScale,this->handleScale));
    m = matrix44::multiply(m, this->decomposedRotation); // m*= this->decomposedRotation;
    m.set_position(this->origin);
	Debug::DebugShapeRenderer::Instance()->DrawBox(m, color, CoreGraphics::RenderShape::AlwaysOnTop);

    // draw X axis + handle
	if (X_AXIS == this->currentDragMode || X_AXIS == mode)
	{
		if (this->axisLocking)	color.set(1, 1, 1, 1);
		else					color.set(1, 1, 0, 1);
	}
    else
    {
		color = float4(.8f, 0, 0, 1);
    }

    m = matrix44::identity();
    m.scale(vector(this->handleScale,this->handleScale,this->handleScale));
	m = matrix44::multiply(m, this->decomposedRotation);
    m.set_position(this->xAxis);
	Debug::DebugShapeRenderer::Instance()->DrawBox(m, color, CoreGraphics::RenderShape::AlwaysOnTop);
    
	m = matrix44::identity();
	line[0].pos = this->origin;
    line[1].pos = this->xAxis;
    Debug::DebugShapeRenderer::Instance()->DrawPrimitives(matrix44::identity(),
        CoreGraphics::PrimitiveTopology::LineList,
        1,
        line,
        color,
		CoreGraphics::RenderShape::AlwaysOnTop);

    // draw Y axis + handle    
	if (Y_AXIS == this->currentDragMode || Y_AXIS == mode)
	{
		if (this->axisLocking)	color.set(1, 1, 1, 1);
		else					color.set(1, 1, 0, 1);
	}
    else
    {
		color = float4(0, .8f, 0, 1);
    }

    m = matrix44::identity();
    m.scale(vector(this->handleScale,this->handleScale,this->handleScale));
	m = matrix44::multiply(m, matrix44::rotationz(N_PI/2.0f));
    m = matrix44::multiply(m, this->decomposedRotation);
    m.set_position(this->yAxis);
	Debug::DebugShapeRenderer::Instance()->DrawBox(m, color, CoreGraphics::RenderShape::AlwaysOnTop);

    line[0].pos = this->origin;
    line[1].pos = this->yAxis;
    Debug::DebugShapeRenderer::Instance()->DrawPrimitives(matrix44::identity(),
        CoreGraphics::PrimitiveTopology::LineList,
        1,
        line,
        color,
		CoreGraphics::RenderShape::AlwaysOnTop);

    // draw Z axis + handle
	if (Z_AXIS == this->currentDragMode || Z_AXIS == mode)
	{
		if (this->axisLocking)	color.set(1, 1, 1, 1);
		else					color.set(1, 1, 0, 1);
	}
    else
    {
		color = float4(0, 0, .8f, 1);
    }

    m = matrix44::identity();
    m.scale(vector(this->handleScale,this->handleScale,this->handleScale));
    m = matrix44::multiply(m, matrix44::rotationy(-N_PI/2.0f));
    m = matrix44::multiply(m, this->decomposedRotation);
    m.set_position(this->zAxis);
	Debug::DebugShapeRenderer::Instance()->DrawBox(m,
        color,
        CoreGraphics::RenderShape::AlwaysOnTop);

    line[0].pos = this->origin;
    line[1].pos = this->zAxis;
	Debug::DebugShapeRenderer::Instance()->DrawPrimitives(matrix44::identity(),
        CoreGraphics::PrimitiveTopology::LineList,
        1,
        line,
        color,
        CoreGraphics::RenderShape::AlwaysOnTop);
}

//------------------------------------------------------------------------------
/**
	Computes the position of the scale handles
*/
void
ScaleFeature::UpdateHandlePositions()
{
    this->DecomposeInitialMatrix();
    
    // compute origin position of feature
    this->origin = (this->decomposedTranslation);
	this->origin.set_w(1);
    
    // compute the scale factor
    vector cameraPosition;
    float distanceToView;

	n_assert(cameraEntity.isvalid());
    const matrix44 camTrans = cameraEntity->GetMatrix44(Attr::Transform);
    cameraPosition = camTrans.get_position();

	vector v(this->origin - cameraPosition);
    distanceToView = v.length();

    this->handleScale = distanceToView;
    // make the handles small
    this->handleScale *= 0.025f;
    
    // set x handle transform
	this->xAxis = this->origin + matrix44::transform(vector(this->handleDistance * this->handleScale * this->scale.x(), 0.0f, 0.0f), this->decomposedRotation);

    // set y handle transform
	this->yAxis = this->origin + matrix44::transform(vector(0.0f, this->handleDistance * this->handleScale * this->scale.y(), 0.0f), this->decomposedRotation);

    // set z handle transform
	this->zAxis = this->origin + matrix44::transform(vector(0.0f, 0.0f, this->handleDistance * this->handleScale * this->scale.z()), this->decomposedRotation);
}

//------------------------------------------------------------------------------
/**
*/
void
ScaleFeature::UpdateTransform(const Math::matrix44 & transform)
{
	this->initialMatrix = transform;
	this->deltaMatrix = Math::matrix44::identity();
}

}