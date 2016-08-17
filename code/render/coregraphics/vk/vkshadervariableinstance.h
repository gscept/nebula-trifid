#pragma once
//------------------------------------------------------------------------------
/**
	Implements a shader variable instance in Vulkan.
	
	(C) 2016 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/refcounted.h"
#include "coregraphics/base/shadervariableinstancebase.h"
namespace Vulkan
{
class VkShaderVariableInstance : public Base::ShaderVariableInstanceBase
{
	__DeclareClass(VkShaderVariableInstance);
public:
	/// constructor
	VkShaderVariableInstance();
	/// destructor
	virtual ~VkShaderVariableInstance();

	/// bind variable to uniform buffer
	void BindToUniformBuffer(const Ptr<CoreGraphics::ConstantBuffer>& buffer, uint32_t offset, uint32_t size, int8_t* defaultValue);
	/// bind variable to uniform buffer
	void BindToUniformBuffer(const Ptr<CoreGraphics::ConstantBuffer>& buffer, uint32_t offset, uint32_t size);

	/// apply local value to shader variable
	void Apply();
	/// apply local value to specific shader variable
	void ApplyTo(const Ptr<CoreGraphics::ShaderVariable>& var);

	/// set int value
	void SetInt(int value);
	/// set int array values
	void SetIntArray(const int* values, SizeT count);
	/// set float value
	void SetFloat(float value);
	/// set float array values
	void SetFloatArray(const float* values, SizeT count);
	/// set vector value
	void SetFloat2(const Math::float2& value);
	/// set vector array values
	void SetFloat2Array(const Math::float2* values, SizeT count);
	/// set vector value
	void SetFloat4(const Math::float4& value);
	/// set vector array values
	void SetFloat4Array(const Math::float4* values, SizeT count);
	/// set matrix value
	void SetMatrix(const Math::matrix44& value);
	/// set matrix array values
	void SetMatrixArray(const Math::matrix44* values, SizeT count);
	/// set bool value
	void SetBool(bool value);
	/// set bool array values
	void SetBoolArray(const bool* values, SizeT count);
	/// set value directly
	void SetValue(const Util::Variant& v);

protected:
	friend class Base::ShaderVariableBase;

	/// cleans up instance
	void Cleanup();

private:
	struct BufferBinding
	{
		Ptr<CoreGraphics::ConstantBuffer> uniformBuffer;
		uint32_t offset;
		uint32_t size;
	}*bufferBinding;
};
} // namespace Vulkan