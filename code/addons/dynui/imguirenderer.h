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

	/// get buffer lock for vertex buffer
	const Ptr<CoreGraphics::BufferLock>& GetVertexBufferLock() const;
	/// get buffer lock for index buffer
	const Ptr<CoreGraphics::BufferLock>& GetIndexBufferLock() const;
	/// get vertex buffer pointer
	byte* GetVertexPtr() const;
	/// get vertex buffer pointer
	byte* GetIndexPtr() const;
	/// get vertex buffer
	const Ptr<CoreGraphics::VertexBuffer>& GetVertexBuffer() const;
	/// get index buffer
	const Ptr<CoreGraphics::IndexBuffer>& GetIndexBuffer() const;
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
	Ptr<CoreGraphics::IndexBuffer> ibo;
	Ptr<CoreGraphics::BufferLock> vboBufferLock;
	Ptr<CoreGraphics::BufferLock> iboBufferLock;
	byte* vertexPtr;
	byte* indexPtr;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::BufferLock>&
ImguiRenderer::GetVertexBufferLock() const
{
	return this->vboBufferLock;
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::BufferLock>&
ImguiRenderer::GetIndexBufferLock() const
{
	return this->iboBufferLock;
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
inline byte*
ImguiRenderer::GetIndexPtr() const
{
	return this->indexPtr;
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
inline const Ptr<CoreGraphics::IndexBuffer>&
ImguiRenderer::GetIndexBuffer() const
{
	return this->ibo;
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