#pragma once
//------------------------------------------------------------------------------
/**
    @class Instancing::OGL4InstanceRenderer
    
    Performs instanced rendering with OpenGL 4
    
    (C) 2013 Gustav Sterbrant
*/
//------------------------------------------------------------------------------
#include "instancing/base/instancerendererbase.h"
namespace Instancing
{
class OGL4InstanceRenderer : public InstanceRendererBase
{
	__DeclareClass(OGL4InstanceRenderer);
public:
	/// constructor
	OGL4InstanceRenderer();
	/// destructor
	virtual ~OGL4InstanceRenderer();

	/// render
	void Render(const SizeT multiplier);

private:
	CoreGraphics::ShaderVariable::Semantic modelArraySemantic;
	CoreGraphics::ShaderVariable::Semantic modelViewArraySemantic;
	CoreGraphics::ShaderVariable::Semantic modelViewProjectionArraySemantic;
    CoreGraphics::ShaderVariable::Semantic objectIdArraySemantic;

	static const int MaxInstancesPerBatch = 256;
}; 
} // namespace Instancing
//------------------------------------------------------------------------------