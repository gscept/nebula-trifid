//------------------------------------------------------------------------------
// vkshadervariable.cc
// (C) 2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "vkshadervariable.h"
#include "coregraphics/constantbuffer.h"

namespace Vulkan
{

__ImplementClass(Vulkan::VkShaderVariable, 'VKSV', Base::ShaderVariableBase);
//------------------------------------------------------------------------------
/**
*/
VkShaderVariable::VkShaderVariable() :
	bufferBinding(0),
	var(0),
	block(0),
	buffer(0)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
VkShaderVariable::~VkShaderVariable()
{
	if (this->bufferBinding) delete this->bufferBinding;
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::BindToUniformBuffer(const Ptr<CoreGraphics::ConstantBuffer>& buffer, uint32_t offset, uint32_t size, int8_t* defaultValue)
{
	this->bufferBinding = new BufferBinding;
	this->bufferBinding->uniformBuffer = buffer;
	this->bufferBinding->offset = offset;
	this->bufferBinding->size = size;
	this->bufferBinding->defaultValue = defaultValue;

	// make sure that the buffer is updated (data is array since we have a char*)
	buffer->UpdateArray(defaultValue, offset, size, 1);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::Setup(AnyFX::VkVariable* var)
{
	n_assert(0 == this->var);

	Util::String name = var->name.c_str();
	this->SetName(name);
	switch (var->type)
	{
	case AnyFX::Double:
	case AnyFX::Float:
		this->SetType(FloatType);
		break;
	case AnyFX::Short:
	case AnyFX::Integer:
	case AnyFX::UInteger:
		this->SetType(IntType);
		break;
	case AnyFX::Bool:
		this->SetType(BoolType);
		break;
	case AnyFX::Float3:
	case AnyFX::Float4:
	case AnyFX::Double3:
	case AnyFX::Double4:
	case AnyFX::Integer3:
	case AnyFX::Integer4:
	case AnyFX::UInteger3:
	case AnyFX::UInteger4:
	case AnyFX::Short3:
	case AnyFX::Short4:
	case AnyFX::Bool3:
	case AnyFX::Bool4:
		this->SetType(VectorType);
		break;
	case AnyFX::Float2:
	case AnyFX::Double2:
	case AnyFX::Integer2:
	case AnyFX::UInteger2:
	case AnyFX::Short2:
	case AnyFX::Bool2:
		this->SetType(Vector2Type);
		break;
	case AnyFX::Matrix2x2:
	case AnyFX::Matrix2x3:
	case AnyFX::Matrix2x4:
	case AnyFX::Matrix3x2:
	case AnyFX::Matrix3x3:
	case AnyFX::Matrix3x4:
	case AnyFX::Matrix4x2:
	case AnyFX::Matrix4x3:
	case AnyFX::Matrix4x4:
		this->SetType(MatrixType);
		break;
	case AnyFX::Sampler1D:
	case AnyFX::Sampler1DArray:
	case AnyFX::Sampler2D:
	case AnyFX::Sampler2DArray:
	case AnyFX::Sampler2DMS:
	case AnyFX::Sampler2DMSArray:
	case AnyFX::Sampler3D:
	case AnyFX::SamplerCube:
	case AnyFX::SamplerCubeArray:
	case AnyFX::Image1D:
	case AnyFX::Image1DArray:
	case AnyFX::Image2D:
	case AnyFX::Image2DArray:
	case AnyFX::Image2DMS:
	case AnyFX::Image2DMSArray:
	case AnyFX::Image3D:
	case AnyFX::ImageCube:
	case AnyFX::ImageCubeArray:
	case AnyFX::Texture1D:
	case AnyFX::Texture1DArray:
	case AnyFX::Texture2D:
	case AnyFX::Texture2DArray:
	case AnyFX::Texture2DMS:
	case AnyFX::Texture2DMSArray:
	case AnyFX::Texture3D:
	case AnyFX::TextureCube:
	case AnyFX::TextureCubeArray:
		this->SetType(TextureType);
		break;
	}
	this->var = var;
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::Setup(AnyFX::VkVarbuffer* var)
{
	n_assert(0 == this->buffer);
	Util::String name = var->name.c_str();
	this->SetName(name);
	this->buffer = var;
	this->SetType(BufferType);
}

//------------------------------------------------------------------------------
/**
*/
void
VkShaderVariable::Setup(AnyFX::VkVarblock* var)
{
	n_assert(0 == this->block);
	Util::String name = var->name.c_str();
	this->SetName(name);
	this->block = var;
	this->SetType(BufferType);
}


} // namespace Vulkan