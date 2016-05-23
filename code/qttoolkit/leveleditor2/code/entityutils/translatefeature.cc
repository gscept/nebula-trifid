//------------------------------------------------------------------------------
//  translatefeature.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
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
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "models/nodes/meshnodeinstance.h"

using namespace Math;
using namespace Input;
using namespace CoreGraphics;
using namespace Graphics;
namespace LevelEditor2
{
__ImplementClass(LevelEditor2::TranslateFeature,'LTRF', LevelEditor2::TransformFeature);

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
void
TranslateFeature::Setup()
{
	Util::Array<CoreGraphics::VertexComponent> comps;
	comps.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float4));

	const uint numHandles = 4;
	this->handleGraphicsEntities.Resize(numHandles);
	this->handleSurfaces.Resize(numHandles);
	this->handleIndices.Resize(numHandles);
	this->handleColors.Resize(numHandles);

	this->vbos.Resize(2);
	this->ibos.Resize(1);

	TranslateFeature::MeshBundle handle = this->CreateHandle(comps);

	const Math::float4 colors[] = {
		Math::float4(1, 0, 0, 0),
		Math::float4(0, 1, 0, 0),
		Math::float4(0, 0, 1, 0),
		Math::float4(1, 1, 1, 0) };

	IndexT i;
	for (i = 0; i < this->handleGraphicsEntities.Size() - 1; i++)
	{
		CoreGraphics::PrimitiveGroup primBox;
		primBox.SetBaseIndex(0);
		primBox.SetBaseVertex(0);
		primBox.SetNumIndices(480);
		primBox.SetNumVertices(0);
		primBox.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

		CoreGraphics::PrimitiveGroup primLine;
		primLine.SetBaseIndex(0);
		primLine.SetBaseVertex(160);
		primLine.SetNumIndices(0);
		primLine.SetNumVertices(2);
		primLine.SetPrimitiveTopology(PrimitiveTopology::LineList);

		this->handleGraphicsEntities[i] = Graphics::MeshEntity::Create();
		this->handleGraphicsEntities[i]->SetVertexComponents(comps);
		this->handleGraphicsEntities[i]->SetVertexBuffer(0, handle.vbo);
		this->handleGraphicsEntities[i]->SetIndexBuffer(handle.ibo);
		this->handleGraphicsEntities[i]->AddNode("line", primLine, "sur:system/leveleditorhandlewireframenopick", handle.box);
		this->handleGraphicsEntities[i]->AddNode("box", primBox, "sur:system/leveleditorhandlesolid", handle.box);
		//this->handleGraphicsEntities[i]->AddNode("axis", axisLine, "sur:system/leveleditorhandlewireframe", handle.box);
		this->handleGraphicsEntities[i]->SetAlwaysVisible(true);

		this->handleColors[i] = colors[i];
		this->handleIndices[i] = -(i + 1);
		this->handleGraphicsEntities[i]->SetPickingId(this->handleIndices[i]);
		GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage()->AttachEntity(this->handleGraphicsEntities[i].upcast<GraphicsEntity>());
		Ptr<Models::ModelNodeInstance> mdlNode = this->handleGraphicsEntities[i]->GetModelInstance()->LookupNodeInstance("root/box");
		Ptr<Models::MeshNodeInstance> boxNode = mdlNode.cast<Models::MeshNodeInstance>();
		mdlNode = this->handleGraphicsEntities[i]->GetModelInstance()->LookupNodeInstance("root/line");
		Ptr<Models::MeshNodeInstance> lineNode = mdlNode.cast<Models::MeshNodeInstance>();

		this->handleSurfaces[i] = boxNode->GetSurfaceInstance();
		this->handleSurfaces[i]->SetValue("MatDiffuse", colors[i]);
		this->handleGraphicsEntities[i]->SetVisible(false);

		// set line to be gray
		lineNode->GetSurfaceInstance()->SetValue("MatDiffuse", float4(0.25f));

		/*
		mdlNode = this->handleGraphicsEntities[i]->GetModelInstance()->LookupNodeInstance("root/axis");
		Ptr<Models::MeshNodeInstance> axisNode = mdlNode.cast<Models::MeshNodeInstance>();
		axisNode->GetSurfaceInstance()->SetValue("MatDiffuse", float4(1));
		*/
	}

	TranslateFeature::MeshBundle square = this->CreateSquare(comps);

	CoreGraphics::PrimitiveGroup primBox;
	primBox.SetBaseIndex(0);
	primBox.SetBaseVertex(0);
	primBox.SetNumIndices(0);
	primBox.SetNumVertices(5);
	primBox.SetPrimitiveTopology(PrimitiveTopology::LineStrip);

	this->handleGraphicsEntities[i] = Graphics::MeshEntity::Create();
	this->handleGraphicsEntities[i]->SetVertexComponents(comps);
	this->handleGraphicsEntities[i]->SetVertexBuffer(0, square.vbo);
	this->handleGraphicsEntities[i]->AddNode("box", primBox, "sur:system/leveleditorhandlewireframe", square.box);
	this->handleGraphicsEntities[i]->SetAlwaysVisible(true);

	this->handleColors[i] = colors[i];
	this->handleIndices[i] = -(i + 1);
	this->handleGraphicsEntities[i]->SetPickingId(this->handleIndices[i]);
	GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage()->AttachEntity(this->handleGraphicsEntities[i].upcast<GraphicsEntity>());
	Ptr<Models::ModelNodeInstance> mdlNode = this->handleGraphicsEntities[i]->GetModelInstance()->LookupNodeInstance("root/box");
	Ptr<Models::MeshNodeInstance> boxNode = mdlNode.cast<Models::MeshNodeInstance>();
	boxNode->SetInViewSpace(true);

	this->handleSurfaces[i] = boxNode->GetSurfaceInstance();
	this->handleSurfaces[i]->SetValue("MatDiffuse", colors[i]);
	this->handleGraphicsEntities[i]->SetVisible(false);

	this->vbos[0] = handle.vbo;
	this->vbos[1] = square.vbo;
	this->ibos[0] = handle.ibo;
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
    if (this->currentDragMode == GLOBAL_DRAG || this->isInFreeDragMode)
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
	if (this->currentDragMode == X_DRAG)
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
	else if (this->currentDragMode == Y_DRAG)
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
	else if (this->currentDragMode == Z_DRAG)
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

        if (this->currentDragMode == X_DRAG)
        {
            vector relativeAxis = matrix44::transform(vector(1, 0, 0), localForward);
            this->dragStart = relativeAxis * (float4::dot3(this->dragStart, relativeAxis) / float4::dot3(relativeAxis, relativeAxis));
        }
		else if (this->currentDragMode == Y_DRAG)
        {
            vector relativeAxis = matrix44::transform(vector(0, 1, 0), localForward);
            this->dragStart = relativeAxis * (float4::dot3(this->dragStart, relativeAxis) / float4::dot3(relativeAxis, relativeAxis));
        }
		else if (this->currentDragMode == Z_DRAG)
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
    if (X_DRAG == this->currentDragMode)
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
	if (Y_DRAG == this->currentDragMode)
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
	if (Z_DRAG == this->currentDragMode)
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
	if (GLOBAL_DRAG == this->currentDragMode)
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
	TransformFeature::RenderHandles();
	matrix44 matrices[] =
	{
		matrix44::rotationy(n_deg2rad(90.0f)),
		matrix44::rotationx(n_deg2rad(-90.0f)),
		matrix44::identity(),
		matrix44::identity()
	};
	IndexT handleGfxIdx;
	for (handleGfxIdx = 0; handleGfxIdx < this->handleGraphicsEntities.Size() - 1; handleGfxIdx++)
	{
		matrix44 modelTransform = matrices[handleGfxIdx];
		modelTransform.scale(vector(this->handleScale * this->handleDistance));
		modelTransform = matrix44::multiply(modelTransform, this->decomposedRotation);
		modelTransform.set_position(this->origin);
		this->handleGraphicsEntities[handleGfxIdx]->SetTransform(modelTransform);
		this->handleGraphicsEntities[handleGfxIdx]->SetVisible(true);
	}

	matrix44 modelTransform = matrix44::identity();
	modelTransform.scale(vector(this->handleScale * this->handleDistance));
	modelTransform = matrix44::multiply(modelTransform, this->decomposedRotation);
	modelTransform.set_position(this->origin);
	this->handleGraphicsEntities[handleGfxIdx]->SetTransform(modelTransform);
	this->handleGraphicsEntities[handleGfxIdx]->SetVisible(true);

	IndexT i;
	for (i = 0; i < this->handleGraphicsEntities.Size(); i++)
	{
		if (this->axisLockingMode && this->currentHandleHovered != NONE)
		{
			this->handleSurfaces[i]->SetValue("MatDiffuse", float4(1, 1, 0, 1));
		}
		else
		{
			if (this->currentDragMode != i + 1)
			{
				this->handleSurfaces[i]->SetValue("MatDiffuse", this->handleColors[i]);
			}
		}
	}

	if (this->currentHandleHovered != NONE)
	{
		if (this->axisLockingMode)
		{
			this->handleSurfaces[this->currentHandleHovered - 1]->SetValue("MatDiffuse", float4(1, 1, 1, 1));
		}
		else
		{
			this->handleSurfaces[this->currentHandleHovered - 1]->SetValue("MatDiffuse", float4(1, 1, 0, 1));
		}
	}
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
TranslateFeature::MeshBundle
TranslateFeature::CreateHandle(const Util::Array<CoreGraphics::VertexComponent>& comps)
{
	const int lineCount = 80;
	vector help;
	float4 handlePoints[lineCount * 2 + 3]; // 3 points at the end is for 0, 0 to cone, cone bottom to top
	vector circleVector;
	circleVector = vector(0, 0.1f, 0);
	vector axis = vector(0, 0, 1);
	bbox visbox;
	visbox.begin_extend();

	IndexT i;
	for (i = 0; i < (lineCount * 2) - 1; i += 2)
	{
		help.set(circleVector.x(), circleVector.y(), circleVector.z());
		this->RotateVector(help, axis, 2 * (float)-PI * (i / 2) / lineCount);
		handlePoints[i] = point(0, 0, 1) + help;

		help = circleVector;
		this->RotateVector(help, axis, 2 * (float)-PI * (i / 2 + 1) / lineCount);
		handlePoints[i + 1] = point(0, 0, 1) + help;

		visbox.extend(handlePoints[i]);
		visbox.extend(handlePoints[i + 1]);
	}
	
	handlePoints[i] = point(0, 0, 0);
	handlePoints[i+1] = point(0, 0, 1);
	handlePoints[i+2] = point(0, 0, 1.2f);

	// add center and top point
	for (; i < lineCount * 2 + 3; i++)
	{
		visbox.extend(handlePoints[i]);
	}
	visbox.end_extend();

	Util::FixedArray<short> indices;
	indices.Resize(lineCount * 3 * 2 + 2);
	IndexT j = 0;
	for (i = 0; i < (lineCount * 2) - 1; i += 2, j += 3)
	{
		indices[j] = i;
		indices[j+1] = i+1;
		indices[j + 2] = lineCount * 2 + 1;
	}
	for (i = 0; i < (lineCount * 2) - 1; i += 2, j += 3)
	{
		indices[j] = i;
		indices[j + 1] = i + 1;
		indices[j + 2] = lineCount * 2 + 2;
	}
	indices[j] = (lineCount * 2) - 1;
	indices[j+1] = (lineCount * 2);

	// create vertex buffer
	Ptr<VertexBuffer> vbo = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vboLoader = MemoryVertexBufferLoader::Create();
	vboLoader->Setup(comps, lineCount * 2 + 3, handlePoints, sizeof(handlePoints), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);
	vbo->SetLoader(vboLoader.downcast<Resources::ResourceLoader>());
	vbo->SetAsyncEnabled(false);
	vbo->Load();
	n_assert(vbo->IsLoaded());
	vbo->SetLoader(NULL);

	Ptr<IndexBuffer> ibo = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> iboLoader = MemoryIndexBufferLoader::Create();
	iboLoader->Setup(IndexType::Index16, lineCount * 3 * 2 + 2, indices.Begin(), indices.Size() * sizeof(short));
	ibo->SetLoader(iboLoader.downcast<Resources::ResourceLoader>());
	ibo->SetAsyncEnabled(false);
	ibo->Load();
	n_assert(ibo->IsLoaded());
	ibo->SetLoader(NULL);

	MeshBundle mesh;
	mesh.vbo = vbo;
	mesh.box = visbox;
	mesh.ibo = ibo;
	return mesh;
}

//------------------------------------------------------------------------------
/**
*/
TranslateFeature::MeshBundle
TranslateFeature::CreateSquare(const Util::Array<CoreGraphics::VertexComponent>& comps)
{
	float4 handlePoints[] =
	{
		point(-0.1f, -0.1f, 0.0f),
		point(0.1f, -0.1f, 0.0f),
		point(0.1f, 0.1f, 0.0f),
		point(-0.1f, 0.1f, 0.0f),
		point(-0.1f, -0.1f, 0.0f),
	};

	// create vertex buffer
	Ptr<VertexBuffer> vbo = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vboLoader = MemoryVertexBufferLoader::Create();
	vboLoader->Setup(comps, 5, handlePoints, sizeof(handlePoints), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);
	vbo->SetLoader(vboLoader.downcast<Resources::ResourceLoader>());
	vbo->SetAsyncEnabled(false);
	vbo->Load();
	n_assert(vbo->IsLoaded());
	vbo->SetLoader(NULL);

	MeshBundle mesh;
	mesh.vbo = vbo;
	mesh.box = bbox(point(0, 0, 0), point(0.1f, 0.1f, 0.0f));
	mesh.ibo = 0;
	return mesh;
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