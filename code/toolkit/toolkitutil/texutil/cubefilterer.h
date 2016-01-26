#pragma once
//------------------------------------------------------------------------------
/**
	@class ToolkitUtil::CubeFilterer
	
	Takes an input CoreGraphics::Texture and filters it if it's a cubemap.
	
	(C) 2015 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/texture.h"
#include "CubeMapGen/CCubeMapProcessor.h"
#include "io/uri.h"
namespace ToolkitUtil
{

class CubeFilterer : public Core::RefCounted
{
	__DeclareClass(CubeFilterer);
public:
	/// constructor
	CubeFilterer();
	/// destructor
	virtual ~CubeFilterer();

	/// set texture to be used as source
	void SetCubemap(const Ptr<CoreGraphics::Texture>& cubeMap);
	/// set the output texture name
	void SetOutputFile(const IO::URI& output);
	/// set the output size (squared size)
	void SetOutputSize(uint size);

	/// set specular power
	void SetSpecularPower(uint power);
	/// set if we should generate mips
	void SetGenerateMips(bool b);

	/// generates sampled cube map and saves into output, the third argument is a function pointer to a function which handles progress
	void Filter(bool irradiance, void* messageHandler, void(*CubeFilterer_Progress)(const Util::String&, void*));

private:
	Ptr<CoreGraphics::Texture> cubeMap;
	IO::URI output;
	bool generateMips;
	uint power;
	uint size;

	CCubeMapProcessor processor;
};

//------------------------------------------------------------------------------
/**
*/
inline void
CubeFilterer::SetCubemap(const Ptr<CoreGraphics::Texture>& cubeMap)
{
	this->cubeMap = cubeMap;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CubeFilterer::SetOutputFile(const IO::URI& output)
{
	this->output = output;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CubeFilterer::SetOutputSize(uint size)
{
	this->size = size;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CubeFilterer::SetSpecularPower(uint power)
{
	this->power = power;
}

//------------------------------------------------------------------------------
/**
*/
inline void
CubeFilterer::SetGenerateMips(bool b)
{
	this->generateMips = b;
}

} // namespace ToolkitUtil