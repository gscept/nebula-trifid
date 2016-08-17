#pragma once
//------------------------------------------------------------------------------
/**
    @class Rocket::RocketRenderer
    
    Implements a librocket
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "Rocket/Core.h"
#include "core/refcounted.h"
#include "coregraphics/primitivegroup.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shadervariable.h"
#include "math/float4.h"
#include "math/rectangle.h"

namespace CoreGraphics
{
class RenderDevice;
class ShaderState;
class ShaderVariation;
class VertexBuffer;
class IndexBuffer;
class VertexLayout;
class Texture;
}

namespace Resources
{
	class ManagedTexture;
}

namespace LibRocket
{
class RocketRenderer : public Rocket::Core::RenderInterface
{
public:
	/// constructor
	RocketRenderer();
	/// destructor
	virtual ~RocketRenderer();

	/// called by Rocket to compile geometry in a reusable buffer
	Rocket::Core::CompiledGeometryHandle CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture);
	/// called by Rocket to render temporary geometry
	void RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation);
	/// called by Rocket to render compiled geometry
	void RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& trnslation);
	/// called by Rocket when it wants to release geometry
	void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry);

	/// called by Rocket when it wants to enable scissoring
	void EnableScissorRegion(bool enable);
	/// called by Rocket when it wants to change the scissor region
	void SetScissorRegion(int x, int y, int width, int height);

	/// called by Rocket when it wants to load a texture
	bool LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source);
	/// called by Rocket when a texture should be generated from a sequence of pixels
	bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const ubyte* source, const Rocket::Core::Vector2i& source_dimensions);
	/// called by Rocket when a texture gets released
	void ReleaseTexture(Rocket::Core::TextureHandle texture);

private:

	Ptr<CoreGraphics::RenderDevice> renderDevice;
	Ptr<CoreGraphics::ShaderState> shader;
	
	Ptr<CoreGraphics::ShaderVariable> diffMap;
	Ptr<CoreGraphics::ShaderVariable> modelVar;

	Ptr<Resources::ManagedTexture> whiteTexture;
	Math::rectangle<int> scissor;

	CoreGraphics::ShaderFeature::Mask defaultVariation;
	CoreGraphics::ShaderFeature::Mask scissorVariation;

	struct NebulaCompiledGeometry
	{
		Ptr<CoreGraphics::VertexBuffer> vb;
		Ptr<CoreGraphics::IndexBuffer> ib;
		Ptr<CoreGraphics::Texture> texture;
		CoreGraphics::PrimitiveGroup primGroup;
	};

	struct NebulaTexture
	{
		Ptr<CoreGraphics::Texture> tex;
	};

	struct NebulaVertex
	{
		float x, y;
		float u, v;
		float r, g, b, a;
	};	
}; 
} // namespace Rocket
//------------------------------------------------------------------------------