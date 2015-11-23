#pragma once
//------------------------------------------------------------------------------
/**
	@class Dynui::ImguiRenderer
	
	Nebula renderer for  the IMGUI dynamic UI library.
	
	(C) 2012-2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/texture.h"
#include "coregraphics/shaderinstance.h"
#include "coregraphics/bufferlock.h"
#include "coregraphics/vertexbuffer.h"
#include "input/inputevent.h"
namespace Dynui
{
struct ImguiRendererParams
{
	Ptr<CoreGraphics::ShaderVariable> projVar;
	Ptr<CoreGraphics::ShaderVariable> fontVar;
};

class ImguiRenderer : public Core::RefCounted
{
	__DeclareClass(ImguiRenderer);
	__DeclareSingleton(ImguiRenderer);
public:
	/// constructor
	ImguiRenderer();
	/// destructor
	virtual ~ImguiRenderer();

	/// set the screen dimensions to use when rendering the UI (all vertices will be mapped to these values)
	void SetRectSize(SizeT width, SizeT height);
	
	/// setup the imgui renderer, call SetRectSize prior to this
	void Setup();
	/// discard imgui renderer
	void Discard();
	
	/// render frame
	void Render();
	/// handle event
	bool HandleInput(const Input::InputEvent& event);

	/// get buffer lock
	const Ptr<CoreGraphics::BufferLock>& GetBufferLock() const;
	/// get vertex buffer pointer
	byte* GetVertexPtr() const;
	/// get vertex buffer
	const Ptr<CoreGraphics::VertexBuffer>& GetVertexBuffer() const;
	/// get shader
    const Ptr<CoreGraphics::Shader>& GetShader() const;
	/// get font texture
	const Ptr<CoreGraphics::Texture>& GetFontTexture() const;
	/// get shader params
	const ImguiRendererParams& GetParams() const;

private:

	ImguiRendererParams params;
	Ptr<CoreGraphics::Shader> uiShader;
	Ptr<CoreGraphics::Texture> fontTexture;
	Ptr<CoreGraphics::VertexBuffer> vbo;
	Ptr<CoreGraphics::BufferLock> bufferLock;
	byte* vertexPtr;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::BufferLock>&
ImguiRenderer::GetBufferLock() const
{
	return this->bufferLock;
}

//------------------------------------------------------------------------------
/**
*/
inline byte*
ImguiRenderer::GetVertexPtr() const
{
	return this->vertexPtr;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::VertexBuffer>&
ImguiRenderer::GetVertexBuffer() const
{
	return this->vbo;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Shader>&
ImguiRenderer::GetShader() const
{
	return this->uiShader;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::Texture>&
ImguiRenderer::GetFontTexture() const
{
	return this->fontTexture;
}

//------------------------------------------------------------------------------
/**
*/
inline const ImguiRendererParams&
ImguiRenderer::GetParams() const
{
	return this->params;
}

} // namespace Dynui