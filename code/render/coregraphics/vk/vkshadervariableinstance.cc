//------------------------------------------------------------------------------
// vkshadervariableinstance.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshadervariableinstance.h"

using namespace CoreGraphics;
namespace Vulkan
{

__ImplementClass(Vulkan::VkShaderVariableInstance, 'VKVI', Base::ShaderVariableInstanceBase);
//------------------------------------------------------------------------------
/**
*/
VkShaderVariableInstance::VkShaderVariableInstance() :
	bufferBinding(NULL)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShaderVariableInstance::~VkShaderVariableInstance()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::BindToUniformBuffer(const Ptr<CoreGraphics::ConstantBuffer>& buffer, uint32_t offset, uint32_t size, int8_t* defaultValue)
{
	this->bufferBinding = n_new(BufferBinding);
	this->bufferBinding->uniformBuffer = buffer;
	this->bufferBinding->offset = offset;
	this->bufferBinding->size = size;
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::BindToUniformBuffer(const Ptr<CoreGraphics::ConstantBuffer>& buffer, uint32_t offset, uint32_t size)
{
	this->bufferBinding = n_new(BufferBinding);
	this->bufferBinding->uniformBuffer = buffer;
	this->bufferBinding->offset = offset;
	this->bufferBinding->size = size;
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::Cleanup()
{
	// unbind any handles
	if (this->bufferBinding)
	{
		n_delete(this->bufferBinding);
		this->bufferBinding = 0;
	}
	ShaderVariableInstanceBase::Cleanup();
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::Apply()
{
	n_assert(this->shaderVariable.isvalid());
	bool bufferBound = this->bufferBinding != NULL;
	switch (this->value.GetType())
	{
	case Util::Variant::Object:
	{
		Core::RefCounted* obj = this->value.GetObject();
		if (obj != 0)
		{
			switch (this->type)
			{
			case TextureObjectType:
				this->shaderVariable->SetTexture((CoreGraphics::Texture*)this->value.GetObject());
				break;
			case ConstantBufferObjectType:
				this->shaderVariable->SetConstantBuffer((CoreGraphics::ConstantBuffer*)this->value.GetObject());
				break;
			case ReadWriteImageObjectType:
				if (obj->IsA(ShaderReadWriteTexture::RTTI))
				{
					this->shaderVariable->SetShaderReadWriteTexture((ShaderReadWriteTexture*)obj);
				}
				else
				{
					this->shaderVariable->SetShaderReadWriteTexture((Texture*)obj);
				}
				break;
			case ReadWriteBufferObjectType:
				this->shaderVariable->SetShaderReadWriteBuffer((CoreGraphics::ShaderReadWriteBuffer*)this->value.GetObject());
				break;
			}
			break;
		}
	}
	default:
		if (!bufferBound) ShaderVariableInstanceBase::Apply();
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::ApplyTo(const Ptr<CoreGraphics::ShaderVariable>& var)
{
	n_assert(var.isvalid());
	bool bufferBound = this->bufferBinding != NULL;
	switch (this->value.GetType())
	{
	case Util::Variant::Object:
	{
		Core::RefCounted* obj = this->value.GetObject();
		if (obj != 0)
		{
			switch (this->type)
			{
			case TextureObjectType:
				var->SetTexture((CoreGraphics::Texture*)this->value.GetObject());
				break;
			case ConstantBufferObjectType:
				var->SetConstantBuffer((CoreGraphics::ConstantBuffer*)this->value.GetObject());
				break;
			case ReadWriteImageObjectType:
				if (obj->IsA(ShaderReadWriteTexture::RTTI))
				{
					var->SetShaderReadWriteTexture((ShaderReadWriteTexture*)obj);
				}
				else
				{
					var->SetShaderReadWriteTexture((Texture*)obj);
				}
				break;
			case ReadWriteBufferObjectType:
				var->SetShaderReadWriteBuffer((CoreGraphics::ShaderReadWriteBuffer*)this->value.GetObject());
				break;
			}
		}
	}
	default:
		if (!bufferBound) ShaderVariableInstanceBase::Apply();
		break;
	}
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetInt(int value)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->Update(value, this->bufferBinding->offset, sizeof(int));
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetIntArray(const int* values, SizeT count)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->UpdateArray(values, this->bufferBinding->offset, sizeof(int), count);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetFloat(float value)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->Update(value, this->bufferBinding->offset, sizeof(float));
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetFloatArray(const float* values, SizeT count)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->UpdateArray(values, this->bufferBinding->offset, sizeof(float), count);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetFloat2(const Math::float2& value)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->Update(value, this->bufferBinding->offset, sizeof(Math::float2));
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetFloat2Array(const Math::float2* values, SizeT count)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->UpdateArray(values, this->bufferBinding->offset, sizeof(Math::float2), count);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetFloat4(const Math::float4& value)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->Update(value, this->bufferBinding->offset, sizeof(Math::float4));
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetFloat4Array(const Math::float4* values, SizeT count)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->UpdateArray(values, this->bufferBinding->offset, sizeof(Math::float4), count);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetMatrix(const Math::matrix44& value)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->Update(value, this->bufferBinding->offset, sizeof(Math::matrix44));
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetMatrixArray(const Math::matrix44* values, SizeT count)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->UpdateArray(values, this->bufferBinding->offset, sizeof(Math::matrix44), count);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetBool(bool value)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->Update(value, this->bufferBinding->offset, sizeof(bool));
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetBoolArray(const bool* values, SizeT count)
{
	n_assert(this->bufferBinding != NULL);
	this->bufferBinding->uniformBuffer->UpdateArray(values, this->bufferBinding->offset, sizeof(bool), count);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariableInstance::SetValue(const Util::Variant& v)
{
	bool bufferBound = this->bufferBinding != NULL;
	switch (bufferBound)
	{
	case true:
		switch (v.GetType())
		{
		case Util::Variant::Int:
			this->bufferBinding->uniformBuffer->Update(v.GetInt(), this->bufferBinding->offset, sizeof(int));
			break;
		case Util::Variant::UInt:
			this->bufferBinding->uniformBuffer->Update(v.GetUInt(), this->bufferBinding->offset, sizeof(uint));
			break;
		case Util::Variant::Float:
			this->bufferBinding->uniformBuffer->Update(v.GetFloat(), this->bufferBinding->offset, sizeof(float));
			break;
		case Util::Variant::Bool:
			this->bufferBinding->uniformBuffer->Update(v.GetBool(), this->bufferBinding->offset, sizeof(bool));
			break;
		case Util::Variant::Float2:
			this->bufferBinding->uniformBuffer->Update(v.GetFloat2(), this->bufferBinding->offset, sizeof(Math::float2));
			break;
		case Util::Variant::Float4:
			this->bufferBinding->uniformBuffer->Update(v.GetFloat4(), this->bufferBinding->offset, sizeof(Math::float4));
			break;
		case Util::Variant::Matrix44:
			this->bufferBinding->uniformBuffer->Update(v.GetMatrix44(), this->bufferBinding->offset, sizeof(Math::matrix44));
			break;
		case Util::Variant::IntArray:
		{
			const Util::Array<int>& arr = v.GetIntArray();
			this->bufferBinding->uniformBuffer->UpdateArray(&arr[0], this->bufferBinding->offset, sizeof(int), arr.Size());
			break;
		}
		case Util::Variant::FloatArray:
		{
			const Util::Array<float>& arr = v.GetFloatArray();
			this->bufferBinding->uniformBuffer->UpdateArray(&arr[0], this->bufferBinding->offset, sizeof(float), arr.Size());
			break;
		}
		case Util::Variant::BoolArray:
		{
			const Util::Array<bool>& arr = v.GetBoolArray();
			this->bufferBinding->uniformBuffer->UpdateArray(&arr[0], this->bufferBinding->offset, sizeof(bool), arr.Size());
			break;
		}
		case Util::Variant::Float2Array:
		{
			const Util::Array<Math::float2>& arr = v.GetFloat2Array();
			this->bufferBinding->uniformBuffer->UpdateArray(&arr[0], this->bufferBinding->offset, sizeof(Math::float2), arr.Size());
			break;
		}
		case Util::Variant::Float4Array:
		{
			const Util::Array<Math::float4>& arr = v.GetFloat4Array();
			this->bufferBinding->uniformBuffer->UpdateArray(&arr[0], this->bufferBinding->offset, sizeof(Math::float4), arr.Size());
			break;
		}
		case Util::Variant::Matrix44Array:
		{
			const Util::Array<Math::matrix44>& arr = v.GetMatrix44Array();
			this->bufferBinding->uniformBuffer->UpdateArray(&arr[0], this->bufferBinding->offset, sizeof(Math::matrix44), arr.Size());
			break;
		}
		}
	case false:
		ShaderVariableInstanceBase::SetValue(v);
		break;
	}
}

} // namespace Vulkan