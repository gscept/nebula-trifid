#pragma once
//------------------------------------------------------------------------------
/**
    @class Graphics::BillboardEntity
    
    Represents a billboard entity.
    
    (C) 2013-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "graphics/graphicsentity.h"
#include "coregraphics/shaderinstance.h"
#include "resources/managedtexture.h"
#include "resources/resourceid.h"
#include "models/model.h"
#include "billboards/billboardnode.h"
#include "models/modelinstance.h"
#include "materials/materialvariableinstance.h"
namespace Graphics
{
class BillboardEntity : public Graphics::GraphicsEntity
{
	__DeclareClass(BillboardEntity);
public:
	/// constructor
	BillboardEntity();
	/// destructor
	virtual ~BillboardEntity();

	/// activate entity
	void OnActivate();
	/// deactivate entity
	void OnDeactivate();

	/// hide billboard
	void OnHide();
	/// show billboard
	void OnShow();

	/// sets billboard to be view aligned
	void SetViewAligned(bool b);
	/// gets billboard flag
	const bool GetViewAligned() const;

	/// set picking id of model entity
	void SetPickingId(IndexT i);
	/// get picking id of model entity
	const IndexT GetPickingId() const;

	/// sets billboard texture
	void SetTexture(const Resources::ResourceId& texture);
	/// gets billboard texture
	const Resources::ResourceId& GetTexture() const;
    /// sets billboard color
    void SetColor(const Math::float4& color);
    /// gets billboard color
    const Math::float4& GetColor() const;

    /// handle a message
    virtual void HandleMessage(const Ptr<Messaging::Message>& msg);

private:

	/// resolve visibility
	void OnResolveVisibility(IndexT frameIndex, bool updateLod = false);
	/// update transforms
	void OnTransformChanged();
	/// notify visibility
	void OnNotifyCullingVisible(const Ptr<GraphicsEntity>& observer, IndexT frameIndex);
	/// prepare rendering
	void OnRenderBefore(IndexT frameIndex);
	
	IndexT pickingId;
	bool viewAligned;
	Resources::ResourceId resource;
    Math::float4 color;
    
	Ptr<Models::ModelInstance> modelInstance;
	Ptr<Resources::ManagedTexture> texture;
    Ptr<Materials::ManagedSurfaceMaterial> managedMaterial;
    Ptr<Materials::SurfaceMaterial> material;
    Ptr<Materials::SurfaceConstant> colorVariable;
    Ptr<Materials::SurfaceConstant> textureVariable;

	static Ptr<Models::Model> billboardModel;
	static Ptr<Billboards::BillboardNode> billboardNode;
}; 

//------------------------------------------------------------------------------
/**
*/
inline void 
BillboardEntity::SetViewAligned( bool b )
{
	this->viewAligned = b;
}

//------------------------------------------------------------------------------
/**
*/
inline const bool 
BillboardEntity::GetViewAligned() const
{
	return this->viewAligned;
}

//------------------------------------------------------------------------------
/**
*/
inline const IndexT 
BillboardEntity::GetPickingId() const
{
	return this->pickingId;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
BillboardEntity::SetPickingId( IndexT i )
{
	this->pickingId = i;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
BillboardEntity::SetTexture( const Resources::ResourceId& texture )
{
	n_assert(texture.IsValid());
	this->resource = texture;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId& 
BillboardEntity::GetTexture() const
{
	return this->resource;
}

//------------------------------------------------------------------------------
/**
*/

//------------------------------------------------------------------------------
/**
*/
inline const Math::float4& 
BillboardEntity::GetColor() const
{
    return this->color;
}

} // namespace Graphics
//------------------------------------------------------------------------------