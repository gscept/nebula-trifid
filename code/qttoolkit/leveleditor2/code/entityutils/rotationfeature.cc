// ------------------------------------------------------------------------------
//  rotationfeature.cc
//  (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
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
#include "coregraphics/memoryvertexbufferloader.h"
#include "models/nodes/meshnodeinstance.h"
#include "math/rectangle.h"

using namespace Math;
using namespace CoreGraphics;
using namespace Graphics;

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
void
RotationFeature::Setup()
{
	Util::Array<CoreGraphics::VertexComponent> comps;
	comps.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float4));

	const uint numHandles = 4;
	this->handleGraphicsEntities.Resize(numHandles);
	this->handleSurfaces.Resize(numHandles);
	this->handleIndices.Resize(numHandles);
	this->handleColors.Resize(numHandles);

	this->vbos.Resize(2);

	IndexT i;

	RotationFeature::MeshBundle halfCircle = this->CreateHalfCircle(comps);
	RotationFeature::MeshBundle fullCircle = this->CreateCircle(comps);

	this->vbos[0] = halfCircle.vbo;
	this->vbos[1] = fullCircle.vbo;

	const Math::float4 colors[] = { 
		Math::float4(1, 0, 0, 0), 
		Math::float4(0, 1, 0, 0), 
		Math::float4(0, 0, 1, 0), 
		Math::float4(0.5f, 0.5f, 1.0f, 0) };

	RotationFeature::MeshBundle vbos[] =
	{
		halfCircle,
		halfCircle,
		halfCircle,
		fullCircle
	};

	for (i = 0; i < this->handleGraphicsEntities.Size(); i++)
	{ 
		CoreGraphics::PrimitiveGroup prim;
		prim.SetBaseIndex(0);
		prim.SetBaseVertex(0);
		prim.SetNumIndices(0);
		prim.SetNumVertices(vbos[i].vbo->GetNumVertices());
		prim.SetPrimitiveTopology(PrimitiveTopology::LineList);

		this->handleGraphicsEntities[i] = Graphics::MeshEntity::Create();
		this->handleGraphicsEntities[i]->SetVertexComponents(comps);
		this->handleGraphicsEntities[i]->SetVertexBuffer(0, vbos[i].vbo);
		this->handleGraphicsEntities[i]->AddNode("circle", prim, "sur:system/leveleditorhandlewireframe", vbos[i].box);
		this->handleGraphicsEntities[i]->SetAlwaysVisible(true);

		this->handleColors[i] = colors[i];
		this->handleIndices[i] = -(i + 1);
		this->handleGraphicsEntities[i]->SetPickingId(this->handleIndices[i]);
		GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage()->AttachEntity(this->handleGraphicsEntities[i].upcast<GraphicsEntity>());
		const Ptr<Models::ModelNodeInstance>& node = this->handleGraphicsEntities[i]->GetModelInstance()->LookupNodeInstance("root/circle");
		const Ptr<Models::MeshNodeInstance>& mshNode = node.cast<Models::MeshNodeInstance>();
		this->handleSurfaces[i] = mshNode->GetSurfaceInstance();
		this->handleSurfaces[i]->SetValue("MatDiffuse", colors[i]);
		this->handleGraphicsEntities[i]->SetVisible(false);
	}	
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
	
    if (X_DRAG == this->currentDragMode)
    {
		float dot = vector::dot3(currentDragVec, this->startDragOrientation);
		float det = vector::dot3(this->xAxis, vector::cross3(this->startDragOrientation, currentDragVec));
		float angleDiff = atan2f(det, dot);

		vector axis = matrix44::transform(this->xAxis, matrix44::inverse(this->deltaMatrix));
		this->deltaMatrix = matrix44::rotationaxis(axis, angleDiff);
    }
    else if (Y_DRAG == this->currentDragMode)
    {
		float dot = vector::dot3(currentDragVec, this->startDragOrientation);
		float det = vector::dot3(this->yAxis, vector::cross3(this->startDragOrientation, currentDragVec));
		float angleDiff = atan2f(det, dot);

		vector axis = matrix44::transform(this->yAxis, matrix44::inverse(this->deltaMatrix));
		this->deltaMatrix = matrix44::rotationaxis(axis, angleDiff);
    }
    else if (Z_DRAG == this->currentDragMode)
    {
		float dot = vector::dot3(currentDragVec, this->startDragOrientation);
		float det = vector::dot3(this->zAxis, vector::cross3(this->startDragOrientation, currentDragVec));
		float angleDiff = atan2f(det, dot);

		vector axis = matrix44::transform(this->zAxis, matrix44::inverse(this->deltaMatrix));
		this->deltaMatrix = matrix44::rotationaxis(axis, angleDiff);
    }
    else if (GLOBAL_DRAG == this->currentDragMode)
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
	Renders the rotation handles, which can be dragged
*/
void
RotationFeature::RenderHandles()
{
	TransformFeature::RenderHandles();
    
    // The following part is needed to get a mouse over visualization
    // for the handles. For thta calculations like in StartDrag() are
    // required.

	plane planes[] =
	{
		this->xPlane,
		this->yPlane,
		this->zPlane,
		this->viewPlane
	};

	vector axis[] = 
	{
		this->xAxis,
		-this->yAxis,
		this->zAxis,
		this->viewAxis
	};

	vector normals[] =
	{
		this->xPlane.get_normal(),
		this->yPlane.get_normal(),
		this->zPlane.get_normal(),
		this->viewPlane.get_normal()
	};

	vector coordinateAxis[4];
	vector centerVector;
	vector planeNormal;
	line intersectionLine;
	planeNormal = vector(this->viewPlane.a(), this->viewPlane.b(), this->viewPlane.c());

	IndexT handleGfxIdx;
	for (handleGfxIdx = 0; handleGfxIdx < this->handleGraphicsEntities.Size() - 1; handleGfxIdx++)
	{
		planeNormal = vector(planes[handleGfxIdx].a(), planes[handleGfxIdx].b(), planes[handleGfxIdx].c());
		this->viewPlane.intersectplane(planes[handleGfxIdx], intersectionLine);
		vector circleVector = intersectionLine.vec();
		circleVector = vector::normalize(circleVector);
		circleVector *= this->handleDistance * this->handleScale;
		this->RotateVector(circleVector, axis[handleGfxIdx], PI/2);
		vector yvec = normals[handleGfxIdx];
		vector zvec = vector::cross3(yvec, circleVector);
		matrix44 modelTransform = matrix44::identity();
		modelTransform.set_xaxis(circleVector);
		modelTransform.set_yaxis(yvec);
		modelTransform.set_zaxis(zvec);
		modelTransform.set_position(this->origin);
		
		this->handleGraphicsEntities[handleGfxIdx]->SetTransform(modelTransform);
	}

	vector normal = this->viewPlane.get_normal();
	vector circleVector = FindOrtho(normal);
	circleVector = vector::normalize(circleVector);
	circleVector *= this->handleDistance * this->handleScale * this->outerCircleScale;

	vector yvec = normals[handleGfxIdx];
	vector zvec = vector::cross3(yvec, circleVector);
	matrix44 modelTransform = matrix44::identity();
	modelTransform.set_xaxis(circleVector);
	modelTransform.set_yaxis(yvec);
	modelTransform.set_zaxis(zvec);
	modelTransform.set_position(this->origin);

	this->handleGraphicsEntities[handleGfxIdx]->SetTransform(modelTransform);
	this->handleGraphicsEntities[handleGfxIdx]->SetVisible(true);

	IndexT i;
	for (i = 0; i < this->handleGraphicsEntities.Size(); i++)
	{
		if (this->currentDragMode != i + 1)	this->handleSurfaces[i]->SetValue("MatDiffuse", this->handleColors[i]);
	}
	if (this->currentHandleHovered != NONE) this->handleSurfaces[this->currentHandleHovered - 1]->SetValue("MatDiffuse", float4(1, 1, 0, 1));
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

	matrix44 orientation = matrix44::multiply(this->initialMatrix, this->deltaMatrix);
	this->xAxis = matrix44::transform(vector(1, 0, 0), orientation);
	this->yAxis = matrix44::transform(vector(0, 1, 0), orientation);
	this->zAxis = matrix44::transform(vector(0, 0, 1), orientation);
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
RotationFeature::MeshBundle
RotationFeature::CreateHalfCircle(const Util::Array<CoreGraphics::VertexComponent>& comps)
{
	const int lineCount = 40;
	vector help;
	Math::float4 handlePoints[lineCount * 2];
	Math::vector circleVector;
	circleVector = vector(0, 0, 1.0f);
	//Math::vector axis = Math::matrix44::transform(Math::vector::upvec(), Math::matrix44::rotationy(n_deg2rad(-180.0f)));
	Math::vector axis = Math::vector::upvec();
	Math::bbox visbox;
	visbox.begin_extend();
	IndexT i;
	for (i = 0; i < (lineCount * 2) - 1; i += 2)
	{
		help.set(circleVector.x(), circleVector.y(), circleVector.z());
		this->RotateVector(help, axis, (float)-PI * (i / 2) / lineCount);
		handlePoints[i] = point() + help;

		help = circleVector;
		this->RotateVector(help, axis, (float)-PI * (i / 2 + 1) / lineCount);
		handlePoints[i + 1] = point() + help;

		visbox.extend(handlePoints[i]);
		visbox.extend(handlePoints[i + 1]);
	}
	visbox.end_extend();

	// create vertex buffer
	Ptr<VertexBuffer> vbo = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vboLoader = MemoryVertexBufferLoader::Create();
	vboLoader->Setup(comps, lineCount * 2, handlePoints, sizeof(handlePoints), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);
	vbo->SetLoader(vboLoader.downcast<Resources::ResourceLoader>());
	vbo->SetAsyncEnabled(false);
	vbo->Load();
	n_assert(vbo->IsLoaded());
	vbo->SetLoader(NULL);

	MeshBundle mesh;
	mesh.vbo = vbo;
	mesh.box = visbox;
	return mesh;
}

//------------------------------------------------------------------------------
/**
*/
RotationFeature::MeshBundle
RotationFeature::CreateCircle(const Util::Array<CoreGraphics::VertexComponent>& comps)
{
	const int lineCount = 80;
	vector help;
	Math::float4 handlePoints[lineCount * 2];
	Math::vector circleVector;
	circleVector = vector(0, 0, 1.0f);
	Math::vector axis = Math::vector::upvec();
	Math::bbox visbox;
	visbox.begin_extend();

	IndexT i;
	for (i = 0; i < (lineCount * 2) - 1; i += 2)
	{
		help.set(circleVector.x(), circleVector.y(), circleVector.z());
		this->RotateVector(help, axis, 2 * (float)-PI * (i / 2) / lineCount);
		handlePoints[i] = point() + help;

		help = circleVector;
		this->RotateVector(help, axis, 2 * (float)-PI * (i / 2 + 1) / lineCount);
		handlePoints[i + 1] = point() + help;

		visbox.extend(handlePoints[i]);
		visbox.extend(handlePoints[i + 1]);
	}
	visbox.end_extend();

	// create vertex buffer
	Ptr<VertexBuffer> vbo = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vboLoader = MemoryVertexBufferLoader::Create();
	vboLoader->Setup(comps, lineCount * 2, handlePoints, sizeof(handlePoints), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);
	vbo->SetLoader(vboLoader.downcast<Resources::ResourceLoader>());
	vbo->SetAsyncEnabled(false);
	vbo->Load();
	n_assert(vbo->IsLoaded());
	vbo->SetLoader(NULL);

	MeshBundle mesh;
	mesh.vbo = vbo;
	mesh.box = visbox;
	return mesh;
}

//------------------------------------------------------------------------------
/**
*/
void RotationFeature::GetAnglesFromMat(const Math::matrix44& i_m, float& x, float& y, float& z)
{
	//extract rotation from matrix
	quaternion _q = matrix44::rotationmatrix(i_m);
	matrix44 m = matrix44::rotationquaternion(_q);

 	z = atan2(m.getrow1().x(), m.getrow0().x());
 	y = atan2(-m.getrow2().x(), sqrt(pow(m.getrow2().y(),2)+ pow(m.getrow2().z(),2)));
 	x = atan2(m.getrow2().y(), m.getrow2().z());
}

}//namespace