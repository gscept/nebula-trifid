#pragma once
//------------------------------------------------------------------------------
/**
    @class OpenGL4::OGL4VertexLayout
  
    OpenGL4 implementation of vertex layout.

	In OpenGL, the semantics and semantic locations of vertex attributes are completely ignored.
    
    (C) 2007 Radon Labs GmbH
*/    
#include "coregraphics/base/vertexlayoutbase.h"
#include "coregraphics/base/vertexcomponentbase.h"
#include "coregraphics/renderdevice.h"

//------------------------------------------------------------------------------
namespace OpenGL4
{
class OGL4VertexLayout : public Base::VertexLayoutBase
{
    __DeclareClass(OGL4VertexLayout);
public:
    /// constructor
    OGL4VertexLayout();
    /// destructor
    virtual ~OGL4VertexLayout();

    /// setup the vertex layout
    void Setup(const Util::Array<CoreGraphics::VertexComponent>& c);
    /// discard the vertex layout object
    void Discard();

	/// set the vertex buffer associated with the stream index
	void SetStreamBuffer(IndexT streamIndex, GLuint vertexBuffer);
    /// set the index buffer associated with this vertex array object
    void SetIndexBuffer(const Ptr<CoreGraphics::IndexBuffer>& buffer);

    /// get opengl vertex array object
    const GLuint& GetOGL4VertexArrayObject() const;

	/// applies layout before rendering
	void Apply();
        
private:

	GLuint vao;
	GLuint vertexStreams[CoreGraphics::RenderDevice::MaxNumVertexStreams];
	static const SizeT maxElements = 24;
	Util::String semanticName[32];	
};


//------------------------------------------------------------------------------
/**
*/
inline void 
OGL4VertexLayout::SetStreamBuffer( IndexT streamIndex, GLuint vertexBuffer )
{
	this->vertexStreams[streamIndex] = vertexBuffer;
}

//------------------------------------------------------------------------------
/**
*/
inline const GLuint& 
OGL4VertexLayout::GetOGL4VertexArrayObject() const
{
    return this->vao;
}

} // namespace OpenGL4
//------------------------------------------------------------------------------
