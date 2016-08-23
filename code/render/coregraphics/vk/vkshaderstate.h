#pragma once
//------------------------------------------------------------------------------
/**
	Implements a shader instance (local variables and such) in Vulkan.

	To get the shader state to be modified, one has to modify the value of a shader variable.
	To get the shader state to apply a specific descriptor set (0 is enabled by default), 
	one has to explain that the shader state should commit that descriptor set.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shaderstatebase.h"
#include "lowlevel/afxapi.h"

namespace Vulkan
{
class VkShaderState : public Base::ShaderStateBase
{
	__DeclareClass(VkShaderState);
public:
	/// constructor
	VkShaderState();
	/// destructor
	virtual ~VkShaderState();

	/// discard the shader instance, must be called when instance no longer needed
	void Discard();

	/// begin all uniform buffers for a synchronous update
	void BeginUpdateSync();
	/// end buffer updates for all uniform buffers
	void EndUpdateSync();
	/// apply shader from which this state was created
	void Apply();
	/// commit changes before rendering
	void Commit();

	/// add descriptor set write, which will be performed on the next begin
	void AddDescriptorWrite(const VkWriteDescriptorSet& write);
	/// set which descriptor sets this state should apply automatically (0 is true by default)
	void SetApplyDescriptorSet(const IndexT& set, bool b);

	/// get uniform buffer by index
	const Ptr<CoreGraphics::ConstantBuffer>& GetConstantBuffer(IndexT i) const;
	/// get uniform buffer by name
	const Ptr<CoreGraphics::ConstantBuffer>& GetConstantBuffer(const Util::StringAtom& name) const;
	/// get number of uniform buffers
	const SizeT GetNumConstantBuffers() const;
private:
	friend class Base::ShaderBase;
	friend class VkShader;

	/// setup the shader instance from its original shader object
	void Setup(const Ptr<CoreGraphics::Shader>& origShader);
	/// setup the shader instance from its original shader object
	void Setup(const Ptr<CoreGraphics::Shader>& origShader, const Util::Array<IndexT>& groups);

	/// sets up variables
	void SetupVariables(const Util::Array<IndexT>& groups);
	/// setup uniform buffers for shader state
	void SetupUniformBuffers(const Util::Array<IndexT>& groups);

	/// setup descriptor sets which will be maintained by this shader instance
	void SetupDescriptorSets();

	/// update descriptor sets
	void UpdateDescriptorSets();

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


	struct DescriptorSetBinding
	{
		VkDescriptorSet set;
		VkPipelineLayout layout;
		IndexT slot;
	};
	Util::FixedArray<VkDescriptorSet> sets;
	Util::FixedArray<DescriptorSetBinding> setBindnings;
	Util::Array<VkWriteDescriptorSet> pendingSetWrites;

	Util::Array<uint32_t> offsets;
	Util::Dictionary<Util::String, uint32_t> offsetsByName;
	Util::Dictionary<uint32_t, Util::Array<uint32_t>> offsetsByGroup;
	Util::Dictionary<Ptr<CoreGraphics::ConstantBuffer>, uint32_t> instances;

	uint8_t* pushData;
	uint32_t pushSize;
	VkPipelineLayout pushLayout;

	AnyFX::ShaderEffect* effect;
};

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ConstantBuffer>&
VkShaderState::GetConstantBuffer(IndexT i) const
{
	return this->shader->buffers.ValueAtIndex(i);
}

//------------------------------------------------------------------------------
/**
*/
inline const Ptr<CoreGraphics::ConstantBuffer>&
VkShaderState::GetConstantBuffer(const Util::StringAtom& name) const
{
	return this->shader->buffers[name];
}

//------------------------------------------------------------------------------
/**
*/
inline const SizeT
VkShaderState::GetNumConstantBuffers() const
{
	return this->shader->buffers.Size();
}
} // namespace Vulkan