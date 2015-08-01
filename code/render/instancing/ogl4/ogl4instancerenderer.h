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
	CoreGraphics::ShaderVariable::Name modelArraySemantic;
    CoreGraphics::ShaderVariable::Name modelViewArraySemantic;
    CoreGraphics::ShaderVariable::Name modelViewProjectionArraySemantic;
    CoreGraphics::ShaderVariable::Name objectIdArraySemantic;

	static const int MaxInstancesPerBatch = 256;
}; 
} // namespace Instancing
//------------------------------------------------------------------------------