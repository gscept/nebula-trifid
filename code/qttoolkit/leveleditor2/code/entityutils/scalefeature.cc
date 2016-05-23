//------------------------------------------------------------------------------
// scalefeature.cc
// (C) 2009 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------

#include "stdneb.h"
#include "scalefeature.h"
#include "managers/envquerymanager.h"
#include "graphics/view.h"
#include "graphicsfeatureunit.h"
#include "input/mouse.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "models/nodes/meshnodeinstance.h"
//------------------------------------------------------------------------------

using namespace Math;
using namespace CoreGraphics;
using namespace Graphics;

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
void
ScaleFeature::Setup()
{
	Util::Array<CoreGraphics::VertexComponent> comps;
	comps.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float4));

	const uint numHandles = 4;
	this->handleGraphicsEntities.Resize(numHandles);
	this->handleSurfaces.Resize(numHandles);
	this->handleIndices.Resize(numHandles);
	this->handleColors.Resize(numHandles);

	this->vbos.Resize(2);
	this->ibos.Resize(2);

	ScaleFeature::MeshBundle handle = this->CreateHandle(comps);

	const Math::float4 colors[] = {
		Math::float4(1, 0, 0, 0),
		Math::float4(0, 1, 0, 0),
		Math::float4(0, 0, 1, 0),
		Math::float4(1, 1, 1, 0)};

	IndexT i;
	for (i = 0; i < this->handleGraphicsEntities.Size() - 1; i++)
	{
		CoreGraphics::PrimitiveGroup primBox;
		primBox.SetBaseIndex(0);
		primBox.SetBaseVertex(0);
		primBox.SetNumIndices(36);
		primBox.SetNumVertices(0);
		primBox.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

		CoreGraphics::PrimitiveGroup primLine;
		primLine.SetBaseIndex(0);
		primLine.SetBaseVertex(8);
		primLine.SetNumIndices(0);
		primLine.SetNumVertices(2);
		primLine.SetPrimitiveTopology(PrimitiveTopology::LineList);

		CoreGraphics::PrimitiveGroup axisLine;
		axisLine.SetBaseIndex(0);
		axisLine.SetBaseVertex(10 + i * 3);
		axisLine.SetNumIndices(0);
		axisLine.SetNumVertices(3);
		axisLine.SetPrimitiveTopology(PrimitiveTopology::LineStrip);

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

		SurfaceBundle bundle = { boxNode->GetSurfaceInstance() };
		this->handleSurfaces[i] = bundle;
		this->handleSurfaces[i].box->SetValue("MatDiffuse", colors[i]);
		this->handleGraphicsEntities[i]->SetVisible(false);

		// set line to be gray
		lineNode->GetSurfaceInstance()->SetValue("MatDiffuse", float4(0.25f));

		/*
		mdlNode = this->handleGraphicsEntities[i]->GetModelInstance()->LookupNodeInstance("root/axis");
		Ptr<Models::MeshNodeInstance> axisNode = mdlNode.cast<Models::MeshNodeInstance>();
		axisNode->GetSurfaceInstance()->SetValue("MatDiffuse", float4(1));
		*/
	}

	ScaleFeature::MeshBundle box = this->CreateBox(comps);

	CoreGraphics::PrimitiveGroup primBox;
	primBox.SetBaseIndex(0);
	primBox.SetBaseVertex(0);
	primBox.SetNumIndices(36);
	primBox.SetNumVertices(0);
	primBox.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	this->handleGraphicsEntities[i] = Graphics::MeshEntity::Create();
	this->handleGraphicsEntities[i]->SetVertexComponents(comps);
	this->handleGraphicsEntities[i]->SetVertexBuffer(0, box.vbo);
	this->handleGraphicsEntities[i]->SetIndexBuffer(box.ibo);
	this->handleGraphicsEntities[i]->AddNode("box", primBox, "sur:system/leveleditorhandlesolid", box.box);
	this->handleGraphicsEntities[i]->SetAlwaysVisible(true);

	this->handleColors[i] = colors[i];
	this->handleIndices[i] = -(i + 1);
	this->handleGraphicsEntities[i]->SetPickingId(this->handleIndices[i]);
	GraphicsFeature::GraphicsFeatureUnit::Instance()->GetDefaultStage()->AttachEntity(this->handleGraphicsEntities[i].upcast<GraphicsEntity>());
	Ptr<Models::ModelNodeInstance> mdlNode = this->handleGraphicsEntities[i]->GetModelInstance()->LookupNodeInstance("root/box");
	Ptr<Models::MeshNodeInstance> boxNode = mdlNode.cast<Models::MeshNodeInstance>();

	SurfaceBundle bundle = { boxNode->GetSurfaceInstance() };
	this->handleSurfaces[i] = bundle;
	this->handleSurfaces[i].box->SetValue("MatDiffuse", colors[i]);
	this->handleGraphicsEntities[i]->SetVisible(false);

	this->vbos[0] = handle.vbo;
	this->ibos[0] = handle.ibo;
	this->vbos[1] = box.vbo;
	this->ibos[1] = box.ibo;
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
    if (this->currentDragMode == GLOBAL_DRAG)
    {
        this->dragStartMousePosition = mousePos;
        this->isInDragMode = true;
        return;
    }

    // check x handle
    if (this->currentDragMode == X_DRAG)
    {
        worldMouseRay.intersect(line(this->origin,this->xAxis),rayPoint,handlePoint);
        this->dragStartMouseRayOffset = (
            rayPoint -
            this->xAxis
            );
        this->isInDragMode = true;
        return;
    }

    // check y handle
    if (this->currentDragMode == Y_DRAG)
    {
        worldMouseRay.intersect(line(this->origin,this->yAxis),rayPoint,handlePoint);
        this->dragStartMouseRayOffset = (
            rayPoint -
            this->yAxis
            );
        this->isInDragMode = true;
        return;
    }

    // check z handle
	if (this->currentDragMode == Z_DRAG)
    {
        worldMouseRay.intersect(line(this->origin,this->zAxis),rayPoint,handlePoint);
        this->dragStartMouseRayOffset = (
            rayPoint -
            this->zAxis
            );
        this->isInDragMode = true;
        return;
    }
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
    if (X_DRAG == this->currentDragMode)
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
    if (Y_DRAG == this->currentDragMode)
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
    if (Z_DRAG == this->currentDragMode)
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
    if (GLOBAL_DRAG == this->currentDragMode)
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
    this->scale = vector(1.0f,1.0f,1.0f);
}

//------------------------------------------------------------------------------
/**
    Renders handles for a scale feature.	
*/
void
ScaleFeature::RenderHandles()
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
		if (this->axisLocking && this->currentHandleHovered != NONE)
		{
			this->handleSurfaces[i].box->SetValue("MatDiffuse", float4(1, 1, 0, 1));
		}
		else
		{
			if (this->currentDragMode != i + 1)
			{
				this->handleSurfaces[i].box->SetValue("MatDiffuse", this->handleColors[i]);
			}
		}
	}

	if (this->currentHandleHovered != NONE)
	{
		if (this->axisLocking)
		{
			this->handleSurfaces[this->currentHandleHovered - 1].box->SetValue("MatDiffuse", float4(1, 1, 1, 1));
		}
		else
		{
			this->handleSurfaces[this->currentHandleHovered - 1].box->SetValue("MatDiffuse", float4(1, 1, 0, 1));
		}
	}
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
ScaleFeature::MeshBundle
ScaleFeature::CreateHandle(const Util::Array<CoreGraphics::VertexComponent>& comps)
{
	point handlePoints[] =
	{
		point(0.1f, 0.1f, 0.9f),
		point(-0.1f, 0.1f, 0.9f),
		point(0.1f, -0.1f, 0.9f),
		point(-0.1f, -0.1f, 0.9f),
		point(0.1f, 0.1f, 1.1f),
		point(-0.1f, 0.1f, 1.1f),
		point(0.1f, -0.1f, 1.1f),
		point(-0.1f, -0.1f, 1.1f),
		point(0, 0, 0),					// line start
		point(0, 0, 1),					// line end
		point(0.2f, 0, 0),				// x-to-y handle start
		point(0.2f, 0.2f, 0),			// x-to-y handle corner
		point(0, 0.2f, 0),				// x-to-y handle end
		point(0, 0, 0.2f),				// z-to-y handle start
		point(0, 0.2f, 0.2f),			// z-to-y handle corner
		point(0, 0.2f, 0),				// z-to-y handle end
		point(0, 0, 0.2f),				// z-to-x handle start
		point(0.2f, 0, 0.2f),			// z-to-x handle corner
		point(0.2f, 0, 0)				// z-to-x handle end
	};

	static short indices[] =
	{
		0, 1, 2, 1, 2, 3,  /* box indices*/
		4, 5, 6, 5, 6, 7,
		0, 2, 4, 2, 4, 6,
		1, 3, 5, 3, 5, 7,
		0, 1, 4, 1, 4, 5,
		2, 3, 6, 3, 6, 7,
	};

	// extend box
	Math::bbox visbox;
	visbox.begin_extend();
	IndexT i;
	for (i = 0; i < 36; i++) visbox.extend(handlePoints[i]);
	visbox.end_extend();

	// create vertex buffer
	Ptr<VertexBuffer> vbo = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vboLoader = MemoryVertexBufferLoader::Create();
	vboLoader->Setup(comps, 19, handlePoints, sizeof(handlePoints), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);
	vbo->SetLoader(vboLoader.downcast<Resources::ResourceLoader>());
	vbo->SetAsyncEnabled(false);
	vbo->Load();
	n_assert(vbo->IsLoaded());
	vbo->SetLoader(NULL);

	Ptr<IndexBuffer> ibo = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> iboLoader = MemoryIndexBufferLoader::Create();
	iboLoader->Setup(IndexType::Index16, 36, indices, sizeof(indices));
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
ScaleFeature::MeshBundle
ScaleFeature::CreateBox(const Util::Array<CoreGraphics::VertexComponent>& comps)
{
	point handlePoints[] =
	{
		point(0.1f, 0.1f, -0.1f),
		point(-0.1f, 0.1f, -0.1f),
		point(0.1f, -0.1f, -0.1f),
		point(-0.1f, -0.1f, -0.1f),
		point(0.1f, 0.1f, 0.1f),
		point(-0.1f, 0.1f, 0.1f),
		point(0.1f, -0.1f, 0.1f),
		point(-0.1f, -0.1f, 0.1f),
	};

	static short indices[] =
	{
		0, 1, 2, 1, 2, 3,  /* box indices*/
		4, 5, 6, 5, 6, 7,
		0, 2, 4, 2, 4, 6,
		1, 3, 5, 3, 5, 7,
		0, 1, 4, 1, 4, 5,
		2, 3, 6, 3, 6, 7,
	};

	// extend box
	Math::bbox visbox;
	visbox.begin_extend();
	IndexT i;
	for (i = 0; i < 8; i++) visbox.extend(handlePoints[i]);
	visbox.end_extend();

	// create vertex buffer
	Ptr<VertexBuffer> vbo = VertexBuffer::Create();
	Ptr<MemoryVertexBufferLoader> vboLoader = MemoryVertexBufferLoader::Create();
	vboLoader->Setup(comps, 8, handlePoints, sizeof(handlePoints), VertexBuffer::UsageImmutable, VertexBuffer::AccessNone);
	vbo->SetLoader(vboLoader.downcast<Resources::ResourceLoader>());
	vbo->SetAsyncEnabled(false);
	vbo->Load();
	n_assert(vbo->IsLoaded());
	vbo->SetLoader(NULL);

	Ptr<IndexBuffer> ibo = IndexBuffer::Create();
	Ptr<MemoryIndexBufferLoader> iboLoader = MemoryIndexBufferLoader::Create();
	iboLoader->Setup(IndexType::Index16, 36, indices, sizeof(indices));
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
void
ScaleFeature::UpdateTransform(const Math::matrix44 & transform)
{
	this->initialMatrix = transform;
	this->deltaMatrix = Math::matrix44::identity();
}

}