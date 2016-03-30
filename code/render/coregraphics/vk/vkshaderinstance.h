#pragma once
//------------------------------------------------------------------------------
/**
	Implements a shader instance (local variables and such) in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shaderinstancebase.h"
#include "lowlevel/afxapi.h"

namespace Vulkan
{
class VkShaderInstance : public Base::ShaderInstanceBase
{
	__DeclareClass(VkShaderInstance);
public:
	/// constructor
	VkShaderInstance();
	/// destructor
	virtual ~VkShaderInstance();
private:
	friend class Base::ShaderBase;
	friend class VkShader;

	/// setup the shader instance from its original shader object
	virtual void Setup(const Ptr<CoreGraphics::Shader>& origShader);

	/// setup descriptor sets which will be maintained by this shader instance
	void SetupDescriptorSets();

	struct DeferredVariableToBufferBind
	{
		unsigned offset;
		unsigned size;
		unsigned arraySize;
	};
	typedef Util::KeyValuePair<DeferredVariableToBufferBind, Ptr<CoreGraphics::ConstantBuffer>> VariableBufferBinding;
	Util::Dictionary<Util::StringAtom, VariableBufferBinding> uniformVariableBinds;

	typedef Util::KeyValuePair<Ptr<CoreGraphics::ShaderVariable>, Ptr<CoreGraphics::ConstantBuffer>> BlockBufferBinding;
	Util::Array<BlockBufferBinding> blockToBufferBindings;

	Util::FixedArray<VkDescriptorSet> sets;
	Util::Array<Ptr<CoreGraphics::ConstantBuffer>> uniformBuffers;
	Util::Dictionary<Util::StringAtom, Ptr<CoreGraphics::ConstantBuffer>> uniformBuffersByName;

	AnyFX::ShaderEffect* effect;

	Util::FixedArray<VkDescriptorSet> descriptorSets;
};
} // namespace Vulkan