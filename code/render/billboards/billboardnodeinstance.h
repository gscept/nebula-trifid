#pragma once
//------------------------------------------------------------------------------
/**
    @class Models::BillboardNodeInstance
    
    A billboard node instance is represented as an individual renderable instance.
    
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "models/nodes/statenodeinstance.h"
#include "coregraphics/indexbuffer.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/vertexlayout.h"
#include "coregraphics/primitivegroup.h"
namespace Billboards
{
class BillboardNodeInstance : public Models::StateNodeInstance
{
	__DeclareClass(BillboardNodeInstance);
public:
	/// constructor
	BillboardNodeInstance();
	/// destructor
	virtual ~BillboardNodeInstance();

	/// called when visibility resolves
	virtual void OnVisibilityResolve(IndexT resolveIndex, float distToViewer);
	/// called when we render the billboard node
	virtual void Render();
	

protected:
	/// called when the node sets up
	virtual void Setup(const Ptr<Models::ModelInstance>& inst, const Ptr<Models::ModelNode>& node, const Ptr<Models::ModelNodeInstance>& parentNodeInst);
	/// called when removed
	virtual void Discard();

	Ptr<CoreGraphics::VertexBuffer> vb;
	Ptr<CoreGraphics::IndexBuffer> ib;
	CoreGraphics::PrimitiveGroup primGroup;
}; 
} // namespace Models
//------------------------------------------------------------------------------