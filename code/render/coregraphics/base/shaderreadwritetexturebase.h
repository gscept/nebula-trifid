#pragma once
//------------------------------------------------------------------------------
/**
	Implements a way to create textures as non-resources, only used as data read-write dumps for shaders.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "resources/resource.h"
#include "coregraphics/pixelformat.h"

namespace CoreGraphics
{
class Texture;
}

namespace Base
{
class ShaderReadWriteTextureBase : public Core::RefCounted
{
	__DeclareClass(ShaderReadWriteTextureBase)
public:
	/// constructor
	ShaderReadWriteTextureBase();
	/// destructor
	virtual ~ShaderReadWriteTextureBase();

	/// setup texture with fixed size
	virtual void Setup(const SizeT width, const SizeT height, const CoreGraphics::PixelFormat::Code& format, const Resources::ResourceId& id);
	/// setup texture with relative size
	void SetupWithRelativeSize(const Math::scalar relWidth, const Math::scalar relHeight, const CoreGraphics::PixelFormat::Code& format, const Resources::ResourceId& id);
	/// discard texture
	void Discard();

	/// resize texture
	void Resize(SizeT width, SizeT height);
	/// clear texture
	void Clear(const Math::float4& clearColor);

	/// get underlying texture
	const Ptr<CoreGraphics::Texture>& GetTexture() const;

protected:
	bool useRelativeSize;
	Math::scalar relWidth, relHeight;
	Ptr<CoreGraphics::Texture> texture;
	SizeT width, height;	
	CoreGraphics::PixelFormat pixelFormat;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
ShaderReadWriteTextureBase::GetTexture() const
{
	return this->texture;
}

} // namespace Base