//------------------------------------------------------------------------------
//  ogl4shaprenderer.cc
//  (C) 2007 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "coregraphics/ogl4/ogl4shaperenderer.h"
#include "coregraphics/ogl4/ogl4types.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/transformdevice.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/streammeshloader.h"
#include "coregraphics/mesh.h"
#include "coregraphics/vertexlayoutserver.h"
#include "coregraphics/vertexcomponent.h"
#include "threading/thread.h"
#include "coregraphics/shaderserver.h"
#include "resources/resourceid.h"
#include "resources/resourcemanager.h"
#include "models/modelinstance.h"
#include "coregraphics/shadersemantics.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"

namespace OpenGL4
{
__ImplementClass(OpenGL4::OGL4ShapeRenderer, 'O4SR', Base::ShapeRendererBase);

using namespace Base;
using namespace Threading;
using namespace Math;
using namespace CoreGraphics;
using namespace Threading;
using namespace Resources;
using namespace Models;

//------------------------------------------------------------------------------
/**
*/
OGL4ShapeRenderer::OGL4ShapeRenderer() :
	vertexBufferPtr(0),
	indexBufferPtr(0)
{
	this->shapeMeshes.Clear();
}

//------------------------------------------------------------------------------
/**
*/
OGL4ShapeRenderer::~OGL4ShapeRenderer()
{
    n_assert(!this->IsOpen());
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShapeRenderer::Open()
{
    n_assert(!this->IsOpen());
    n_assert(!this->shapeShader.isvalid());

    // call parent class
    ShapeRendererBase::Open();

    // create shape shader instance
    this->shapeShader = ShaderServer::Instance()->CreateShaderInstance(ResourceId("shd:simple"));
	this->shapeMeshes.SetSize(CoreGraphics::RenderShape::NumShapeTypes);

	// create default shapes (basically load them from the models)
	this->CreateBoxShape();
	this->CreateCylinderShape();
	this->CreateSphereShape();
	this->CreateTorusShape();
	this->CreateConeShape();

    // lookup ModelViewProjection shader variable
	this->model = this->shapeShader->GetVariableBySemantic(ShaderVariable::Name("ShapeModel"));
	this->diffuseColor = this->shapeShader->GetVariableBySemantic(ShaderVariable::Name("MatDiffuse"));

	// create feature masks
	this->depthFeatureBits[RenderShape::AlwaysOnTop] = ShaderServer::Instance()->FeatureStringToMask("Static");    
	this->depthFeatureBits[RenderShape::CheckDepth] = ShaderServer::Instance()->FeatureStringToMask("Static|Alt0");
	this->depthFeatureBits[RenderShape::Wireframe] = ShaderServer::Instance()->FeatureStringToMask("Static|Alt1");

	// setup vbo
	Util::Array<VertexComponent> comps;
	comps.Append(VertexComponent(VertexComponent::Position, 0, VertexComponent::Float4, 0));
	comps.Append(VertexComponent(VertexComponent::Color, 0, VertexComponent::Float4, 0));
	Ptr<MemoryVertexBufferLoader> vboLoader = MemoryVertexBufferLoader::Create();
	vboLoader->Setup(comps, MaxNumVertices, NULL, 0, VertexBuffer::UsageDynamic, VertexBuffer::AccessWrite, VertexBuffer::BufferTriple, VertexBuffer::SyncingCoherentPersistent);

	// create vbo
	this->vbo = VertexBuffer::Create();
	this->vbo->SetLoader(vboLoader.upcast<ResourceLoader>());
	this->vbo->SetAsyncEnabled(false);
	this->vbo->Load();
	n_assert(this->vbo->IsLoaded());
	this->vbo->SetLoader(NULL);

	// setup ibo
	Ptr<MemoryIndexBufferLoader> iboLoader = MemoryIndexBufferLoader::Create();
	iboLoader->Setup(IndexType::Index32, MaxNumIndices, NULL, 0, IndexBuffer::UsageDynamic, IndexBuffer::AccessWrite, IndexBuffer::BufferTriple, IndexBuffer::SyncingCoherentPersistent);

	// create ibo
	this->ibo = IndexBuffer::Create();
	this->ibo->SetLoader(iboLoader.upcast<ResourceLoader>());
	this->ibo->SetAsyncEnabled(false);
	this->ibo->Load();
	n_assert(this->ibo->IsLoaded());
	this->ibo->SetLoader(NULL);

	// create buffer locks
	this->vboLock = BufferLock::Create();
	this->iboLock = BufferLock::Create();

	// map buffers
	this->vertexBufferPtr = (byte*)this->vbo->Map(VertexBuffer::MapWrite);
	this->indexBufferPtr = (byte*)this->ibo->Map(IndexBuffer::MapWrite);
	n_assert(0 != this->vertexBufferPtr);
	n_assert(0 != this->indexBufferPtr);

	// setup primitive group
	this->primGroup.SetBaseIndex(0);
	this->primGroup.SetBaseVertex(0);
	this->primGroup.SetPrimitiveTopology(PrimitiveTopology::TriangleList);

    // set buffer index
    this->vbBufferIndex = 0;
	this->ibBufferIndex = 0;

	//glEnable(GL_LINE_SMOOTH);
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShapeRenderer::Close()
{
    n_assert(this->IsOpen());
    n_assert(this->shapeShader.isvalid());

    this->diffuseColor = 0;
	this->model = 0;

	// unload shape meshes
	ResourceManager::Instance()->DiscardManagedResource(this->shapeMeshes[RenderShape::Box].upcast<ManagedResource>());
	ResourceManager::Instance()->DiscardManagedResource(this->shapeMeshes[RenderShape::Sphere].upcast<ManagedResource>());
	ResourceManager::Instance()->DiscardManagedResource(this->shapeMeshes[RenderShape::Cylinder].upcast<ManagedResource>());
	ResourceManager::Instance()->DiscardManagedResource(this->shapeMeshes[RenderShape::Torus].upcast<ManagedResource>());
	ResourceManager::Instance()->DiscardManagedResource(this->shapeMeshes[RenderShape::Cone].upcast<ManagedResource>());
	this->shapeMeshes.Clear();

    // discard shape shader
    this->shapeShader->Discard();
    this->shapeShader = 0;

	// unload dynamic buffers
	this->vbo->Unmap();
	this->ibo->Unmap();
	this->vbo->Unload();
	this->vbo = 0;
	this->ibo->Unload();
	this->ibo = 0;
	this->vertexBufferPtr = this->indexBufferPtr = 0;

    // call parent class
    ShapeRendererBase::Close();
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShapeRenderer::DrawShapes()
{
    n_assert(this->IsOpen());

	OGL4RenderDevice* renderDevice = OGL4RenderDevice::Instance();

	glLineWidth(2.5f);
	glPointSize(2.5f);

	renderDevice->SetPassShader(this->shapeShader);
    for (int depthType = 0; depthType<RenderShape::NumDepthFlags; depthType++)
    {
	    if (this->shapes[depthType].Size() > 0)
	    {	
			this->shapeShader->SelectActiveVariation(depthFeatureBits[depthType]);	
	        this->shapeShader->Begin();
	        this->shapeShader->BeginPass(0);	
	
	        // render individual shapes
	        IndexT i;
	        for (i = 0; i < this->shapes[depthType].Size(); i++)
	        {
	            const RenderShape& curShape = this->shapes[depthType][i];
	            n_assert(InvalidThreadId != curShape.GetThreadId());
	            switch (curShape.GetShapeType())
	            {
	                case RenderShape::Primitives:
	                    this->DrawPrimitives(curShape.GetModelTransform(),
	                                         curShape.GetTopology(),
	                                         curShape.GetNumPrimitives(),
	                                         curShape.GetVertexData(),
	                                         curShape.GetVertexWidth(),
                                             curShape.GetVertexLayout(),
	                                         curShape.GetColor());
	                    break;
	
	                case RenderShape::IndexedPrimitives:
	                    this->DrawIndexedPrimitives(curShape.GetModelTransform(),
	                                                curShape.GetTopology(),
	                                                curShape.GetNumPrimitives(),
	                                                curShape.GetVertexData(),
	                                                curShape.GetNumVertices(),
	                                                curShape.GetVertexWidth(),
                                                    curShape.GetVertexLayout(),
	                                                curShape.GetIndexData(),
	                                                curShape.GetIndexType(),
	                                                curShape.GetColor());
	                    break;
	
                    case RenderShape::RenderMesh:
                        this->DrawMesh(curShape.GetModelTransform(), curShape.GetMesh(), curShape.GetColor());
                        break;
	                default:
	                    this->DrawSimpleShape(curShape.GetModelTransform(), curShape.GetShapeType(), curShape.GetColor());
	                    break;
	            }
	        }

			this->shapeShader->EndPass();
			this->shapeShader->End();
		}
    }
	renderDevice->SetPassShader(0);

    //glDisable(GL_LINE_SMOOTH);

	// delete the shapes of my own thread id, all other shapes
	// are from other threads and will be deleted through DeleteShapesByThreadId()
	this->DeleteShapesByThreadId(Thread::GetMyThreadId());
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShapeRenderer::DrawSimpleShape(const matrix44& modelTransform, RenderShape::Type shapeType, const float4& color)
{
    n_assert(this->shapeMeshes[shapeType].isvalid());
    n_assert(shapeType < RenderShape::NumShapeTypes);

	Ptr<RenderDevice> renderDevice = RenderDevice::Instance();

    // resolve model-view-projection matrix and update shader
	this->model->SetMatrix(modelTransform);
    this->diffuseColor->SetFloat4(color);
    this->shapeShader->Commit();

	Ptr<Mesh> mesh = this->shapeMeshes[shapeType]->GetMesh();	
	Ptr<VertexBuffer> vb = mesh->GetVertexBuffer();
	Ptr<IndexBuffer> ib = mesh->GetIndexBuffer();
	PrimitiveGroup group = mesh->GetPrimitiveGroupAtIndex(0);

	// setup render device
	renderDevice->SetStreamVertexBuffer(0, vb, 0);
	renderDevice->SetVertexLayout(vb->GetVertexLayout());
	renderDevice->SetIndexBuffer(ib);
	renderDevice->SetPrimitiveGroup(group);

	// draw
	renderDevice->Draw();

	// perform post-draw stuff
    this->shapeShader->PostDraw();
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShapeRenderer::DrawPrimitives(const matrix44& modelTransform, 
                                  PrimitiveTopology::Code topology,
                                  SizeT numPrimitives,
                                  const void* vertices,
                                  SizeT vertexWidth,
                                  const Ptr<VertexLayout>& layout,
                                  const Math::float4& color)
{
    n_assert(0 != vertices);
	n_assert(vertexWidth <= MaxVertexWidth);

	// get device
	Ptr<OGL4RenderDevice> renderDevice = OGL4RenderDevice::Instance();

	// calculate vertex count
    SizeT vertexCount = PrimitiveTopology::NumberOfVertices(topology, numPrimitives);
	vertexCount = Math::n_min(vertexCount, MaxNumVertices);

    // unlock buffer to avoid stomping data
	this->vboLock->WaitForRange(MaxNumVertices * MaxVertexWidth * this->vbBufferIndex, vertexCount * vertexWidth * sizeof(float));
    memcpy(this->vertexBufferPtr + MaxNumVertices * MaxVertexWidth * this->vbBufferIndex, vertices, vertexCount * vertexWidth * sizeof(float));

    // resolve model-view-projection matrix and update shader
	this->model->SetMatrix(modelTransform);
    this->diffuseColor->SetFloat4(color);
    this->shapeShader->Commit();

	// set vertex offset in primitive group
	this->primGroup.SetBaseVertex(MaxNumVertices * this->vbBufferIndex);
	this->primGroup.SetNumVertices(vertexCount);
	this->primGroup.SetNumIndices(0);
	this->primGroup.SetPrimitiveTopology(topology);

	// setup render device and draw
	renderDevice->SetStreamSource(0, this->vbo, 0);
    if (layout.isvalid())   renderDevice->SetVertexLayout(layout);
    else                    renderDevice->SetVertexLayout(this->vbo->GetVertexLayout());
	renderDevice->SetIndexBuffer(NULL);
	renderDevice->SetPrimitiveGroup(this->primGroup);
	renderDevice->Draw();

    // end drawing
    this->shapeShader->PostDraw();

    // place a lock and increment buffer count
	this->vboLock->LockRange(MaxNumVertices * MaxVertexWidth * this->vbBufferIndex, vertexCount * vertexWidth * sizeof(float));
	this->vbBufferIndex = (this->vbBufferIndex + 1) % 3;
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShapeRenderer::DrawIndexedPrimitives(const matrix44& modelTransform,
                                         PrimitiveTopology::Code topology,
                                         SizeT numPrimitives,
                                         const void* vertices,
                                         SizeT numVertices,
                                         SizeT vertexWidth,
                                         const Ptr<VertexLayout>& layout,
                                         const void* indices,
                                         IndexType::Code indexType,
                                         const float4& color)
{
    n_assert(0 != vertices);
    n_assert(0 != indices);
	n_assert(vertexWidth <= MaxVertexWidth);

	// get device
	Ptr<OGL4RenderDevice> renderDevice = OGL4RenderDevice::Instance();

	// calculate index count and size of index type
    SizeT indexCount = PrimitiveTopology::NumberOfVertices(topology, numPrimitives);
	SizeT indexSize = CoreGraphics::IndexType::SizeOf(indexType);
	SizeT vertexCount = Math::n_min(numVertices, MaxNumVertices);
	indexCount = Math::n_min(indexCount, MaxNumIndices);

    // unlock buffer and copy data
	this->vboLock->WaitForRange(MaxNumVertices * MaxVertexWidth * this->vbBufferIndex, vertexCount * vertexWidth * sizeof(float));
	this->iboLock->WaitForRange(MaxNumIndices * MaxIndexWidth * this->ibBufferIndex, indexCount * indexSize);
	memcpy(this->vertexBufferPtr + MaxNumVertices * MaxVertexWidth * this->vbBufferIndex, vertices, vertexCount * vertexWidth * sizeof(float));
	memcpy(this->indexBufferPtr + MaxNumIndices * MaxIndexWidth * this->ibBufferIndex, indices, indexCount * indexSize);

    // resolve model-view-projection matrix and update shader
	this->model->SetMatrix(modelTransform);
    this->diffuseColor->SetFloat4(color);
    this->shapeShader->Commit();

	// set vertex offset in primitive group
	this->primGroup.SetBaseVertex(MaxNumVertices * this->vbBufferIndex);
	this->primGroup.SetNumVertices(0);										// indices decides how many primitives we draw
	this->primGroup.SetBaseIndex(MaxNumIndices * this->vbBufferIndex);
	this->primGroup.SetNumIndices(indexCount);
	this->primGroup.SetPrimitiveTopology(topology);

	// setup render device and draw
	renderDevice->SetStreamSource(0, this->vbo, 0);
    if (layout.isvalid())   renderDevice->SetVertexLayout(layout);
    else                    renderDevice->SetVertexLayout(this->vbo->GetVertexLayout());
	renderDevice->SetIndexBuffer(this->ibo);
	renderDevice->SetPrimitiveGroup(this->primGroup);
	renderDevice->Draw();

    // end drawing
    this->shapeShader->PostDraw();

    // lock buffer and increment buffer count
	this->vboLock->LockRange(MaxNumVertices * MaxVertexWidth * this->vbBufferIndex, numVertices * vertexWidth * sizeof(float));
	this->iboLock->LockRange(MaxNumIndices * MaxIndexWidth * this->ibBufferIndex, indexCount * indexSize);
	this->vbBufferIndex = (this->vbBufferIndex + 1) % 3;
	this->ibBufferIndex = (this->ibBufferIndex + 1) % 3;
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShapeRenderer::DrawMesh(const Math::matrix44& modelTransform, const Ptr<CoreGraphics::Mesh>& mesh, const Math::float4& color)
{
    n_assert(mesh.isvalid());
    Ptr<RenderDevice> renderDevice = RenderDevice::Instance();

    // resolve model-view-projection matrix and update shader
    TransformDevice* transDev = TransformDevice::Instance();
    this->model->SetMatrix(modelTransform);
    this->diffuseColor->SetFloat4(color);
    this->shapeShader->Commit();

    // draw shape
    n_assert(RenderDevice::Instance()->IsInBeginFrame());

    Ptr<CoreGraphics::VertexBuffer> vb = mesh->GetVertexBuffer();
    Ptr<CoreGraphics::IndexBuffer> ib = mesh->GetIndexBuffer();
	PrimitiveGroup group = mesh->GetPrimitiveGroupAtIndex(0);

	// setup render device
	renderDevice->SetStreamVertexBuffer(0, vb, 0);
	renderDevice->SetVertexLayout(vb->GetVertexLayout());
	renderDevice->SetIndexBuffer(ib);
	renderDevice->SetPrimitiveGroup(group);
	renderDevice->Draw();

    // end drawing
    this->shapeShader->PostDraw();
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShapeRenderer::CreateBoxShape()
{
	Ptr<ManagedMesh> mesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, "msh:system/box.nvx2", StreamMeshLoader::Create()).downcast<ManagedMesh>();
	this->shapeMeshes[RenderShape::Box] = mesh;
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShapeRenderer::CreateSphereShape()
{
	Ptr<ManagedMesh> mesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, "msh:system/sphere.nvx2", StreamMeshLoader::Create()).downcast<ManagedMesh>();
	this->shapeMeshes[RenderShape::Sphere] = mesh;
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShapeRenderer::CreateCylinderShape()
{
	Ptr<ManagedMesh> mesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, "msh:system/cylinder.nvx2", StreamMeshLoader::Create()).downcast<ManagedMesh>();
	this->shapeMeshes[RenderShape::Cylinder] = mesh;
}

//------------------------------------------------------------------------------
/**
*/
void 
OGL4ShapeRenderer::CreateTorusShape()
{
	Ptr<ManagedMesh> mesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, "msh:system/torus.nvx2", StreamMeshLoader::Create()).downcast<ManagedMesh>();
	this->shapeMeshes[RenderShape::Torus] = mesh;
}

//------------------------------------------------------------------------------
/**
*/
void
OGL4ShapeRenderer::CreateConeShape()
{
	Ptr<ManagedMesh> mesh = ResourceManager::Instance()->CreateManagedResource(Mesh::RTTI, "msh:system/cone.nvx2", StreamMeshLoader::Create()).downcast<ManagedMesh>();
	this->shapeMeshes[RenderShape::Cone] = mesh;
}

} // namespace OpenGL4
