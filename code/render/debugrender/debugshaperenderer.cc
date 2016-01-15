//------------------------------------------------------------------------------
//  debugshaperenderer.cc
//  (C) 2008 Radon Labs GmbH
//  (C) 2013-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "debugrender/debugshaperenderer.h"
#include "io/memorystream.h"
#include "debugrender/debugrenderprotocol.h"
#include "graphics/graphicsinterface.h"
#include "threading/thread.h"

namespace Debug
{
__ImplementClass(Debug::DebugShapeRenderer, 'DBSR', Core::RefCounted);
__ImplementSingleton(Debug::DebugShapeRenderer);

using namespace Util;
using namespace CoreGraphics;
using namespace Math;
using namespace IO;
using namespace Threading;

//------------------------------------------------------------------------------
/**
*/
DebugShapeRenderer::DebugShapeRenderer()
{
    __ConstructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
DebugShapeRenderer::~DebugShapeRenderer()
{
    __DestructSingleton;
}

//------------------------------------------------------------------------------
/**
*/
void
DebugShapeRenderer::DrawBox(const matrix44& modelTransform, const float4& color, RenderShape::RenderFlag depthFlag)
{
    this->shapes.Append(RenderShape(Thread::GetMyThreadId(), RenderShape::Box, depthFlag, modelTransform, color));
}

//------------------------------------------------------------------------------
/**
*/
void
DebugShapeRenderer::DrawSphere(const matrix44& modelTransform, const float4& color, RenderShape::RenderFlag depthFlag)
{
    this->shapes.Append(RenderShape(Thread::GetMyThreadId(), RenderShape::Sphere, depthFlag, modelTransform, color));
}

//------------------------------------------------------------------------------
/**
*/
void
DebugShapeRenderer::DrawCylinder(const matrix44& modelTransform, const float4& color, RenderShape::RenderFlag depthFlag)
{
    this->shapes.Append(RenderShape(Thread::GetMyThreadId(), RenderShape::Cylinder, depthFlag, modelTransform, color));
}

//------------------------------------------------------------------------------
/**
*/
void
DebugShapeRenderer::DrawTorus(const matrix44& modelTransform, const float4& color, RenderShape::RenderFlag depthFlag)
{
    this->shapes.Append(RenderShape(Thread::GetMyThreadId(), RenderShape::Torus, depthFlag, modelTransform, color));
}


//------------------------------------------------------------------------------
/**
*/
void
DebugShapeRenderer::DrawCone(const Math::matrix44& modelTransform, const Math::float4& color, RenderShape::RenderFlag depthFlag)
{
	this->shapes.Append(RenderShape(Thread::GetMyThreadId(), RenderShape::Cone, depthFlag, modelTransform, color));
}

//------------------------------------------------------------------------------
/**
    NOTE: this method copies the vertex data to a temporary buffer.
*/
void
DebugShapeRenderer::DrawPrimitives(const matrix44& modelTransform, PrimitiveTopology::Code topology, SizeT numPrimitives, const CoreGraphics::RenderShape::RenderShapeVertex* vertices, const float4& color, RenderShape::RenderFlag depthFlag)
{
    RenderShape shape;
    shape.SetupPrimitives(Thread::GetMyThreadId(), 
                          modelTransform, 
                          topology, 
                          numPrimitives, 
                          vertices, 
                          color, 
                          depthFlag);
    this->shapes.Append(shape);
}

//------------------------------------------------------------------------------
/**
    NOTE: this method copies the vertex and index data to a temporary buffer.
*/
void
DebugShapeRenderer::DrawIndexedPrimitives(const matrix44& modelTransform, PrimitiveTopology::Code topology, SizeT numPrimitives, const CoreGraphics::RenderShape::RenderShapeVertex* vertices, SizeT numVertices, const void* indices, IndexType::Code indexType, const float4& color, RenderShape::RenderFlag depthFlag)
{
    RenderShape shape;
    shape.SetupIndexedPrimitives(Thread::GetMyThreadId(), 
                                 modelTransform, 
                                 topology, 
                                 numPrimitives, 
                                 vertices, 
                                 numVertices, 
                                 indices, 
                                 indexType, 
                                 color,
								 depthFlag);
    this->shapes.Append(shape);
}


//------------------------------------------------------------------------------
/**
*/
void 
DebugShapeRenderer::DrawMesh( const Math::matrix44& modelTransform, const Ptr<CoreGraphics::Mesh>& mesh, const Math::float4& color, CoreGraphics::RenderShape::RenderFlag depthFlag /*= CoreGraphics::RenderShape::CheckDepth*/ )
{
    RenderShape shape;
    shape.SetupMesh(Thread::GetMyThreadId(), modelTransform, mesh, color, depthFlag);
    this->shapes.Append(shape);
}

//------------------------------------------------------------------------------
/**    
*/
void
DebugShapeRenderer::OnFrame()
{
    // send a RenderDebugShape message to the render thread, send this
    // every frame, whether there are any shapes or not as this will
    // also cleanup the shapes from the previous frame in the
    // render thread
    Ptr<RenderDebugShapes> msg = RenderDebugShapes::Create();
    msg->SetThreadId(Thread::GetMyThreadId());
    msg->SetShapes(this->shapes);
    Graphics::GraphicsInterface::Instance()->Send(msg.cast<Messaging::Message>());
    this->shapes.Clear();
}

} // namespace Debug
