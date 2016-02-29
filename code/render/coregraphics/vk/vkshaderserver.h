#pragma once
//------------------------------------------------------------------------------
/**
	Implements the shader server used by Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shaderserverbase.h"
namespace Vulkan
{
class VkShaderServer : public Base::ShaderServerBase
{
	__DeclareClass(VkShaderServer);
public:
	/// constructor
	VkShaderServer();
	/// destructor
	virtual ~VkShaderServer();

	/// open the shader server
	bool Open();
	/// close the shader server
	void Close();

	/// reloads a shader
	void ReloadShader(Ptr<CoreGraphics::Shader> shader);
	/// explicitly loads a shader by resource id
	void LoadShader(const Resources::ResourceId& shdName);

private:
	AnyFX::EffectFactory* factory;
};
} // namespace Vulkan