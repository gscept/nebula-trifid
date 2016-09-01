//------------------------------------------------------------------------------
// rendertexturebase.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "rendertexturebase.h"
#include "coregraphics/displaydevice.h"
#include "resources/resourcemanager.h"

using namespace Resources;
using namespace CoreGraphics;
namespace Base
{

__ImplementClass(Base::RenderTextureBase, 'RTEB', Core::RefCounted);
//------------------------------------------------------------------------------
/**
*/
RenderTextureBase::RenderTextureBase() :
	width(0),
	height(0),
	depth(0),
	relativeSize(false),
	dynamicSize(false),
	msaaEnabled(false),
	usage(InvalidAttachment)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
RenderTextureBase::~RenderTextureBase()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTextureBase::Setup()
{
	n_assert(this->width > 0 && this->height > 0 && this->depth > 0);
	n_assert(this->type == Texture::Texture2D || this->type == Texture::TextureCube);
	n_assert(this->usage != InvalidAttachment);
	if (this->relativeSize)
	{
		const DisplayMode& mode = DisplayDevice::Instance()->GetDisplayMode();
		this->width = SizeT(mode.GetWidth() * this->widthScale);
		this->height = SizeT(mode.GetHeight() * this->heightScale);
		this->depth = 1;
	}
	else if (this->dynamicSize)
	{
		// add scale factor here
		const DisplayMode& mode = DisplayDevice::Instance()->GetDisplayMode();
		this->width = SizeT(mode.GetWidth() * this->widthScale);
		this->height = SizeT(mode.GetHeight() * this->heightScale);
		this->depth = 1;
	}

	// setup texture resource
	if (this->resourceId.IsValid())
	{
		this->texture = ResourceManager::Instance()->CreateUnmanagedResource(this->resourceId, Texture::RTTI).downcast<Texture>();
	}
	else
	{
		// just create a texture natively without managing it
		this->texture = CoreGraphics::Texture::Create();
	}

	// setup texture
	/*
	this->texture = Texture::Create();
	this->texture->type = this->type;
	this->texture->pixelFormat = this->format;
	this->texture->width = this->width;
	this->texture->height = this->height;
	this->texture->depth = this->depth;
	this->texture->numMipLevels = 1;
	this->texture->skippedMips = 0;
	this->texture->isRenderTargetAttachment = true;
	*/
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTextureBase::Discard()
{
	n_assert(this->texture.isvalid());
	this->texture->Unload();
	this->texture = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
RenderTextureBase::Resize()
{
	n_assert(this->width > 0 && this->height > 0 && this->depth > 0);
	n_assert(this->type == Texture::Texture2D || this->type == Texture::TextureCube);
	n_assert(this->usage != InvalidAttachment);
	if (this->relativeSize)
	{
		const DisplayMode& mode = DisplayDevice::Instance()->GetDisplayMode();
		this->width = SizeT(mode.GetWidth() * this->widthScale);
		this->height = SizeT(mode.GetHeight() * this->heightScale);
		this->depth = 1;
	}
	else if (this->dynamicSize)
	{
		// add scale factor here
		const DisplayMode& mode = DisplayDevice::Instance()->GetDisplayMode();
		this->width = SizeT(mode.GetWidth() * this->widthScale);
		this->height = SizeT(mode.GetHeight() * this->heightScale);
		this->depth = 1;
	}

	// update texture
	this->texture->type = this->type;
	this->texture->pixelFormat = this->format;
	this->texture->width = this->width;
	this->texture->height = this->height;
	this->texture->depth = this->depth;
	this->texture->numMipLevels = 1;
	this->texture->skippedMips = 0;
	this->texture->isRenderTargetAttachment = true;
}


} // namespace Base