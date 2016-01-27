#pragma once
//------------------------------------------------------------------------------
/**
    @class Frame::FrameShader
    
    A FrameShader controls the rendering of an entire frame, and is
    configured by an XML file.
    
    (C) 2007 Radon Labs GmbH
    (C) 2013-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "util/dictionary.h"
#include "resources/resourceid.h"
#include "resources/managedtexture.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/shadervariableinstance.h"
#include "frame/framepassbase.h"
#include "graphics/cameraentity.h"

//------------------------------------------------------------------------------
namespace Frame
{
class FrameShader : public Core::RefCounted
{
    __DeclareClass(FrameShader);
public:
    /// constructor
    FrameShader();
    /// destructor
    virtual ~FrameShader();    
    /// discard the frame shader
    void Discard();

	/// begin rendering a frame
	void Begin();
    /// render the frame shader from the given camera
    void Render(IndexT frameIndex);
	/// end rendering frame
	void End();

    /// set the name of the frame shader
    void SetName(const Resources::ResourceId& id);
    /// get the name of the frame shader
    const Resources::ResourceId& GetName() const;

	/// add a depth-stencil target to the frame
	void AddDepthStencilTarget(const Resources::ResourceId& resId, const Ptr<CoreGraphics::DepthStencilTarget>& rt);
	/// get number of depth-stencil targets
	SizeT GetNumDepthStencilTargets() const;
	/// get depth-stencil target by index
	const Ptr<CoreGraphics::DepthStencilTarget>& GetDepthStencilTargetByIndex(IndexT i) const;
	/// return true if depth-stencil target exists with given name
	bool HasDepthStencilTarget(const Resources::ResourceId& resId) const;
	/// get depth-stencil target by name
	const Ptr<CoreGraphics::DepthStencilTarget>& GetDepthStencilTargetByName(const Resources::ResourceId& resId);

    /// add a render target to the frame
    void AddRenderTarget(const Resources::ResourceId& resId, const Ptr<CoreGraphics::RenderTarget>& rt);
    /// get number of render targets
    SizeT GetNumRenderTargets() const;
    /// get render target by index
    const Ptr<CoreGraphics::RenderTarget>& GetRenderTargetByIndex(IndexT i) const;
    /// return true if render target exists by name
    bool HasRenderTarget(const Resources::ResourceId& resId) const;
    /// get render target by name
    const Ptr<CoreGraphics::RenderTarget>& GetRenderTargetByName(const Resources::ResourceId& resId) const;

    /// add a multiple render target to the frame
    void AddMultipleRenderTarget(const Resources::ResourceId& resId, const Ptr<CoreGraphics::MultipleRenderTarget>& rt);
    /// get number of multiple render targets
    SizeT GetNumMultipleRenderTargets() const;
    /// get multiple render target by index
    const Ptr<CoreGraphics::MultipleRenderTarget>& GetMultipleRenderTargetByIndex(IndexT i) const;
    /// return true if multiple render target exists by name
    bool HasMultipleRenderTarget(const Resources::ResourceId& resId) const;
    /// get multiple render target by name
    const Ptr<CoreGraphics::MultipleRenderTarget>& GetMultipleRenderTargetByName(const Resources::ResourceId& resId) const;

    /// add a render target cube to the frame
    void AddRenderTargetCube(const Resources::ResourceId& resId, const Ptr<CoreGraphics::RenderTargetCube>& rt);
    /// get number of render target cube
    SizeT GetNumRenderTargetCubes() const;
    /// get render target cube by index
    const Ptr<CoreGraphics::RenderTargetCube>& GetRenderTargetCubeByIndex(IndexT i) const;
    /// return true if render target cube exists by name
    bool HasRenderTargetCube(const Resources::ResourceId& resId) const;
    /// get render target cube by name
    const Ptr<CoreGraphics::RenderTargetCube>& GetRenderTargetCubeByName(const Resources::ResourceId& resId) const;

    /// add a texture to the frame
    void AddTexture(const Resources::ResourceId& resId, const Ptr<Resources::ManagedTexture>& t);
    /// get number of textures
    SizeT GetNumTextures() const;
    /// get texture by index
    const Ptr<Resources::ManagedTexture>& GetTextureByIndex(IndexT i) const;
    /// return true if texture exists by name
    bool HasTexture(const Resources::ResourceId& resId) const;
    /// get texture by name
    const Ptr<Resources::ManagedTexture>& GetTextureByName(const Resources::ResourceId& resId) const;

    /// add a frame pass to the frame shader
    void AddFramePassBase(const Ptr<FramePassBase>& framePass);
    /// get number of frame passes
    SizeT GetNumFramePassBases() const;
    /// get frame pass by index
    const Ptr<FramePassBase>& GetFramePassBaseByIndex(IndexT i) const;
    /// return true if names pass exists
    bool HasFramePassBase(const Resources::ResourceId& resId) const;
    /// get frame pass by name
    const Ptr<FramePassBase>& GetFramePassBaseByName(const Resources::ResourceId& resId) const;
	/// get all frame pass bases
	const Util::Array<Ptr<FramePassBase> >& GetAllFramePassBases() const;

	/// insert pass before other pass using name, asserts name is an existing pass
	void InsertBeforePass(const Ptr<FramePassBase>& framePass, const Resources::ResourceId& before);
	/// same as above, but after 
	void InsertAfterPass(const Ptr<FramePassBase>& framePass, const Resources::ResourceId& after);

	/// called after display resize is done
	void OnDisplayResized(SizeT width, SizeT height);

private:
    Resources::ResourceId name;
    Util::Dictionary<Resources::ResourceId, Ptr<CoreGraphics::RenderTarget> > renderTargets;
	Util::Dictionary<Resources::ResourceId, Ptr<CoreGraphics::DepthStencilTarget> > depthStencilTargets;
    Util::Dictionary<Resources::ResourceId, Ptr<CoreGraphics::MultipleRenderTarget> > multipleRenderTargets;
    Util::Dictionary<Resources::ResourceId, Ptr<CoreGraphics::RenderTargetCube> > renderTargetCubes;
    Util::Dictionary<Resources::ResourceId, Ptr<Resources::ManagedTexture> > textures;
    Util::Dictionary<Resources::ResourceId, Ptr<CoreGraphics::ShaderVariableInstance> > shaderVariables;

    Util::Array<Ptr<FramePassBase> > framePasses;
    Util::Dictionary<Resources::ResourceId, IndexT> framePassIndexMap;
};

//------------------------------------------------------------------------------
/**
*/
inline void
FrameShader::SetName(const Resources::ResourceId& resId)
{
    this->name = resId;
}

//------------------------------------------------------------------------------
/**
*/
inline const Resources::ResourceId&
FrameShader::GetName() const
{
    return this->name;
}

//------------------------------------------------------------------------------
/**
*/
inline void 
FrameShader::AddDepthStencilTarget( const Resources::ResourceId& resId, const Ptr<CoreGraphics::DepthStencilTarget>& rt )
{
	n_assert(!this->depthStencilTargets.Contains(resId));
	this->depthStencilTargets.Add(resId, rt);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT 
FrameShader::GetNumDepthStencilTargets() const
{
	return this->depthStencilTargets.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::DepthStencilTarget>& 
FrameShader::GetDepthStencilTargetByIndex( IndexT i ) const
{
	return this->depthStencilTargets.ValueAtIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
inline bool 
FrameShader::HasDepthStencilTarget( const Resources::ResourceId& resId ) const
{
	return this->depthStencilTargets.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::DepthStencilTarget>& 
FrameShader::GetDepthStencilTargetByName( const Resources::ResourceId& resId )
{
	return this->depthStencilTargets[resId];
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameShader::AddRenderTarget(const Resources::ResourceId& resId, const Ptr<CoreGraphics::RenderTarget>& rt)
{
    n_assert(!this->renderTargets.Contains(resId));
    this->renderTargets.Add(resId, rt);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
FrameShader::GetNumRenderTargets() const
{
    return this->renderTargets.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTarget>& 
FrameShader::GetRenderTargetByIndex(IndexT i) const
{
    return this->renderTargets.ValueAtIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
FrameShader::HasRenderTarget(const Resources::ResourceId& resId) const
{
    return this->renderTargets.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTarget>&
FrameShader::GetRenderTargetByName(const Resources::ResourceId& resId) const
{
    return this->renderTargets[resId];
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameShader::AddMultipleRenderTarget(const Resources::ResourceId& resId, const Ptr<CoreGraphics::MultipleRenderTarget>& rt)
{
    n_assert(!this->multipleRenderTargets.Contains(resId));
    this->multipleRenderTargets.Add(resId, rt);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
FrameShader::GetNumMultipleRenderTargets() const
{
    return this->multipleRenderTargets.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::MultipleRenderTarget>& 
FrameShader::GetMultipleRenderTargetByIndex(IndexT i) const
{
    return this->multipleRenderTargets.ValueAtIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
FrameShader::HasMultipleRenderTarget(const Resources::ResourceId& resId) const
{
    return this->multipleRenderTargets.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::MultipleRenderTarget>&
FrameShader::GetMultipleRenderTargetByName(const Resources::ResourceId& resId) const
{
    return this->multipleRenderTargets[resId];
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameShader::AddRenderTargetCube(const Resources::ResourceId& resId, const Ptr<CoreGraphics::RenderTargetCube>& rt)
{
    n_assert(!this->renderTargetCubes.Contains(resId));
    this->renderTargetCubes.Add(resId, rt);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
FrameShader::GetNumRenderTargetCubes() const
{
    return this->renderTargetCubes.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTargetCube>& 
FrameShader::GetRenderTargetCubeByIndex(IndexT i) const
{
    return this->renderTargetCubes.ValueAtIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
FrameShader::HasRenderTargetCube(const Resources::ResourceId& resId) const
{
    return this->renderTargetCubes.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::RenderTargetCube>&
FrameShader::GetRenderTargetCubeByName(const Resources::ResourceId& resId) const
{
    return this->renderTargetCubes[resId];
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameShader::AddTexture(const Resources::ResourceId& resId, const Ptr<Resources::ManagedTexture>& t)
{
    n_assert(!this->textures.Contains(resId));
    this->textures.Add(resId, t);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
FrameShader::GetNumTextures() const
{
    return this->textures.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Resources::ManagedTexture>& 
FrameShader::GetTextureByIndex(IndexT i) const
{
    return this->textures.ValueAtIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
inline bool
FrameShader::HasTexture(const Resources::ResourceId& resId) const
{
    return this->textures.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<Resources::ManagedTexture>&
FrameShader::GetTextureByName(const Resources::ResourceId& resId) const
{
    return this->textures[resId];
}

//------------------------------------------------------------------------------
/**
*/
inline void
FrameShader::AddFramePassBase(const Ptr<FramePassBase>& framePass)
{
    this->framePasses.Append(framePass);
    this->framePassIndexMap.Add(framePass->GetName(), this->framePasses.Size() - 1);
}

//------------------------------------------------------------------------------
/**
*/
inline SizeT
FrameShader::GetNumFramePassBases() const
{
    return this->framePasses.Size();
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<FramePassBase>&
FrameShader::GetFramePassBaseByIndex(IndexT i) const
{
    return this->framePasses[i];
}

//------------------------------------------------------------------------------
/**
*/
inline bool
FrameShader::HasFramePassBase(const Resources::ResourceId& resId) const
{
    return this->framePassIndexMap.Contains(resId);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<FramePassBase>&
FrameShader::GetFramePassBaseByName(const Resources::ResourceId& resId) const
{
    return this->framePasses[this->framePassIndexMap[resId]];
}

//------------------------------------------------------------------------------
/**
*/
inline const Util::Array<Ptr<FramePassBase> >&
FrameShader::GetAllFramePassBases() const
{
	return this->framePasses;
}

} // namespace Frame
//------------------------------------------------------------------------------
