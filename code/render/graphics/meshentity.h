#pragma once
//------------------------------------------------------------------------------
/**
	Implements a renderable entity with a specified vertex and index buffer.
	This is different from a model entity, because it can be used for dynamic meshes
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "graphicsentity.h"
#include "models/model.h"
#include "coregraphics/primitivegroup.h"
#include "coregraphics/vertexbuffer.h"
#include "coregraphics/indexbuffer.h"

namespace CoreGraphics
{
class PrimitiveGroup;
}

namespace Graphics
{
class MeshEntity : public GraphicsEntity
{
	__DeclareClass(MeshEntity);
public:
	/// constructor
	MeshEntity();
	/// destructor
	virtual ~MeshEntity();

	/// set vertex components
	void SetVertexComponents(const Util::Array<CoreGraphics::VertexComponent>& comps);
	/// set the vertex buffer to be used
	void SetVertexBuffer(IndexT index, const Ptr<CoreGraphics::VertexBuffer>& vbo);
	/// set the index buffer to be used
	void SetIndexBuffer(const Ptr<CoreGraphics::IndexBuffer>& ibo);
	/// add a node to be rendered, this is a piece of the vertex/index buffer with a surface
	void AddNode(const Util::StringAtom& name, const CoreGraphics::PrimitiveGroup& group, const Resources::ResourceId& surface, const Math::bbox& box);

	/// get model instance
	const Ptr<Models::ModelInstance>& GetModelInstance() const;

	/// sets picking id
	void SetPickingId(const IndexT& i);
	/// gets picking id
	const IndexT& GetPickingId() const;

	/// apply state
	void ApplyState(const CoreGraphics::PrimitiveGroup& prim);

	/// activate entity
	void OnActivate();
	/// deactivate entity
	void OnDeactivate();

	/// hide billboard
	void OnHide();
	/// show billboard
	void OnShow();

private:
	friend class MeshNode;

	struct __StagingModelNode
	{
		Util::StringAtom name;
		CoreGraphics::PrimitiveGroup group;
		Resources::ResourceId surface;
		Math::bbox boundingbox;
	};

	/// resolve visibility
	void OnResolveVisibility(IndexT frameIndex, bool updateLod = false);
	/// update transforms
	void OnTransformChanged();
	/// notify visibility
	void OnNotifyCullingVisible(const Ptr<GraphicsEntity>& observer, IndexT frameIndex);
	/// prepare rendering
	void OnRenderBefore(IndexT frameIndex);

	IndexT pickingId;

	Util::Array<__StagingModelNode> stagingNodes;
	Ptr<Models::Model> model;
	Ptr<Models::ModelInstance> modelInstance;

	typedef Util::KeyValuePair<IndexT, Ptr<CoreGraphics::VertexBuffer>> VboBinding;
	Util::Array<VboBinding> vbos;
	Ptr<CoreGraphics::IndexBuffer> ibo;

	Util::Array<CoreGraphics::VertexComponent> components;
	Ptr<CoreGraphics::VertexLayout> layout;
};

//------------------------------------------------------------------------------
/**
*/
inline void
MeshEntity::SetVertexComponents(const Util::Array<CoreGraphics::VertexComponent>& comps)
{
	this->components = comps;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MeshEntity::SetVertexBuffer(IndexT index, const Ptr<CoreGraphics::VertexBuffer>& vbo)
{
	n_assert(vbo.isvalid());
	this->vbos.Append(VboBinding{ index, vbo });
}

//------------------------------------------------------------------------------
/**
*/
inline void
MeshEntity::SetIndexBuffer(const Ptr<CoreGraphics::IndexBuffer>& ibo)
{
	n_assert(ibo.isvalid());
	this->ibo = ibo;
}

//------------------------------------------------------------------------------
/**
*/
inline void
MeshEntity::SetPickingId(const IndexT& i)
{
	this->pickingId = i;
}

//------------------------------------------------------------------------------
/**
*/
inline const IndexT&
MeshEntity::GetPickingId() const
{
	return this->pickingId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Models::ModelInstance>&
MeshEntity::GetModelInstance() const
{
	return this->modelInstance;
}


} // namespace Graphics