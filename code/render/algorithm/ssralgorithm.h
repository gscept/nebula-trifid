#pragma once
//------------------------------------------------------------------------------
/**
	Screen space reflection algorithm class.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "algorithmbase.h"
#include "coregraphics/shader.h"

namespace Algorithm
{
class SSRAlgorithm : public AlgorithmBase
{
	__DeclareClass(SSRAlgorithm);
public:
	/// constructor
	SSRAlgorithm();
	/// destructor
	virtual ~SSRAlgorithm();

	/// setup algorithm
	void Setup();
	/// discards resources
	void Discard();

	/// execute algorithm
	void Render();
private:

	Ptr<CoreGraphics::Shader> ssrShader;
	Ptr<CoreGraphics::Texture> output;

	Ptr<CoreGraphics::ShaderVariable> depthBuffer;
	Ptr<CoreGraphics::ShaderVariable> colorBuffer;
	Ptr<CoreGraphics::ShaderVariable> specularBuffer;
	Ptr<CoreGraphics::ShaderVariable> normalBuffer;
	Ptr<CoreGraphics::ShaderVariable> emissiveBuffer;

	Ptr<CoreGraphics::ShaderVariable> resolution;
	Ptr<CoreGraphics::ShaderVariable> invResolution;
};

__RegisterClass(SSRAlgorithm);
} // namespace Algorithm